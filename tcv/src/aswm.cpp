#include "aswm.h"
#include "GetTimeMs64.h"

double get_weighted_mean(double weights[], uchar window[], uchar size)
{
	double acc = 0;
	double wacc = 0;
	for (int i = 0; i < size; i++)
	{
		wacc += weights[i] * window[i];
		acc += weights[i];
	}
	return wacc / acc;
}

double get_deviation(double weights[], uchar window[], double mean, uchar size)
{
	double acc = 0;
	double wacc = 0;
	for (int i = 0; i < size; i++)
	{
		wacc += weights[i] * pow(window[i] - mean, 2);
		acc += weights[i];
	}
	return sqrt(wacc / acc);
}

extern cv::Mat aswm_noise;

#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

int aswm_filter(cv::Mat image)
{
	uchar *p_img = (uchar *)(image.data);
	int total_iterations = 0;

	uint64 start = GetTimeMs64();

	for (int i = 0; i < image.cols; i++)
	{
		for (int j = 0; j < image.rows; j++)
		{
			uchar window[9];
			double weights[9] = { 1,1,1,1,1,1,1,1,1 };

			window[0] = p_img[image.cols * (j - 1) + i - 1];
			window[1] = p_img[image.cols * (j - 1) + i];
			window[2] = p_img[image.cols * (j - 1) + i + 1];

			window[3] = p_img[image.cols * j + i - 1];
			window[4] = p_img[image.cols * j + i]; //center
			window[5] = p_img[image.cols * j + i + 1];

			window[6] = p_img[image.cols * (j + 1) + i - 1];
			window[7] = p_img[image.cols * (j + 1) + i];
			window[8] = p_img[image.cols * (j + 1) + i + 1];

			double mean = get_weighted_mean(weights, window, 9);
			int iter = 0;
			while (true)
			{
				iter++;
				//estimate the weights
				for (int l = 0; l < 9; l++)
				{
					weights[l] = 1 / (abs(window[l] - mean) + 0.1);
				}

				double new_mean = get_weighted_mean(weights, window, 9);
				double diff = abs(new_mean - mean);
				mean = new_mean;
				if (diff < 0.1)
					break;
			}
			total_iterations += iter;

			double deviation = get_deviation(weights, window, mean, 9);

			if (abs(window[4] - mean) > deviation*6.5)
			{
				qsort(window, 9, sizeof(uchar), compare_uchar);
				p_img[image.cols * j + i] = window[4];
			}
		}
	}

	uint64 end = GetTimeMs64();

	cout << "aswm time: \t" << end - start << endl;
	cout << "aswm iterations: \t" << total_iterations << endl;


	return total_iterations;
}

int aswm_mod_filter(cv::Mat image)
{
	uchar *p_img = (uchar *)(image.data);
	int total_iterations = 0;

	uint64 start = GetTimeMs64();

	for (int i = 0; i < image.cols; i++)
	{
		double weights[9] = { 1,1,1,1,1,1,1,1,1 };
		int w_len = 0;

		for (int j = 0; j < image.rows; j++)
		{
			uchar window[9];
			window[0] = p_img[image.cols * (j - 1) + i - 1];
			window[1] = p_img[image.cols * (j - 1) + i];
			window[2] = p_img[image.cols * (j - 1) + i + 1];

			window[3] = p_img[image.cols * j + i - 1];
			window[4] = p_img[image.cols * j + i]; //center
			window[5] = p_img[image.cols * j + i + 1];

			window[6] = p_img[image.cols * (j + 1) + i - 1];
			window[7] = p_img[image.cols * (j + 1) + i];
			window[8] = p_img[image.cols * (j + 1) + i + 1];

			double mean = get_weighted_mean(weights, window, 9);
			int iter = 0;
			while (true)
			{
				iter++;
				//estimate the weights
				for (int l = w_len; l < 9; l++)
				{
					weights[l] = 1 / (abs(window[l] - mean) + 0.1);
				}

				double new_mean = get_weighted_mean(weights, window, 9);
				double diff = abs(new_mean - mean);
				mean = new_mean;
				if (diff < 0.01)
					break;
			}
			total_iterations += iter;

			double deviation = get_deviation(weights, window, mean, 9);

			if (abs(window[4] - mean) > deviation*4.5)
			{
				qsort(window, 9, sizeof(uchar), compare_uchar);
				p_img[image.cols * j + i] = window[4];
			}

			
			weights[0] = weights[3];
			weights[1] = weights[4];
			weights[2] = weights[5];
			weights[3] = weights[6];
			weights[4] = weights[7];
			weights[5] = weights[8];

			weights[6] = 1;
			weights[7] = 1;
			weights[8] = 1;

			w_len = 0;

		}
	}
	uint64 end = GetTimeMs64();

	cout << "aswm mod iterations: \t" << total_iterations << endl;
	cout << "aswm mod time: \t" << end - start << endl;
	return total_iterations;
}

int aswm_mod2_filter(cv::Mat image, cv::Mat prev_frame, cv::Mat prev_frame_res)
{
	uchar *p_img = (uchar *)(image.data);
	int total_iterations = 0;

	static uint64 max_time = 0;
	static uint64 min_time = 0xFFFFFFFF;

	uint64 start = GetTimeMs64();

	cv::Mat diff_frame = prev_frame - image;
	uchar *p_diff_frame = (uchar*)(diff_frame.data);

	for (int i = 0; i < image.cols; i++)
	{
		double weights[9] = { 1,1,1,1,1,1,1,1,1 };
		int w_len = 0;

		for (int j = 0; j < image.rows; j++)
		{
			if (p_diff_frame[image.cols * j + i] == 0)
			{
				p_img[image.cols * j + i] = prev_frame_res.data[image.cols * j + i];
				continue;
			}
			uchar window[9];
			window[0] = p_img[image.cols * (j - 1) + i - 1];
			window[1] = p_img[image.cols * (j - 1) + i];
			window[2] = p_img[image.cols * (j - 1) + i + 1];

			window[3] = p_img[image.cols * j + i - 1];
			window[4] = p_img[image.cols * j + i]; //center
			window[5] = p_img[image.cols * j + i + 1];

			window[6] = p_img[image.cols * (j + 1) + i - 1];
			window[7] = p_img[image.cols * (j + 1) + i];
			window[8] = p_img[image.cols * (j + 1) + i + 1];

			double mean = get_weighted_mean(weights, window, 9);
			int iter = 0;
			while (true)
			{
				iter++;
				//estimate the weights
				for (int l = w_len; l < 9; l++)
				{
					weights[l] = 1 / (abs(window[l] - mean) + 0.1);
				}

				double new_mean = get_weighted_mean(weights, window, 9);
				double diff = abs(new_mean - mean);
				mean = new_mean;
				if (diff < 0.01)
					break;
			}
			total_iterations += iter;

			double deviation = get_deviation(weights, window, mean, 9);

			if (abs(window[4] - mean) > deviation*8.5)
			{
				qsort(window, 9, sizeof(uchar), compare_uchar);
				p_img[image.cols * j + i] = window[4];

				//set 1 for noise
				//aswm_noise.data[image.cols * j + i] = 255;
			}


			weights[0] = weights[3];
			weights[1] = weights[4];
			weights[2] = weights[5];
			weights[3] = weights[6];
			weights[4] = weights[7];
			weights[5] = weights[8];

			weights[6] = 1;
			weights[7] = 1;
			weights[8] = 1;

			w_len = 0;

		}
	}
	uint64 end = GetTimeMs64();

	cout << "aswm mod iterations: \t" << total_iterations << endl;
	cout << "aswm mod time: \t" << end - start << endl;

	if (max_time < end - start)
		max_time = end - start;

	if (min_time > end - start)
		min_time = end - start;

	cout << "min: \t" << min_time << "max: \t" << max_time << endl;
	return total_iterations;
}



/// fixed point part
#include "fix16.h"

#define frac_mul(a, b) (((long long)a * b) >> 16)
#define frac_div(a, b) ((((unsigned long long)a << 15) / b) << 1)

inline fix16_t get_weighted_mean_fix(fix16_t weights[], uchar window[], uchar size)
{
	register fix16_t acc = 0;
	register fix16_t wacc = 0;

	fix16_t *p_weights = weights;
	uchar *p_window = window;

	while(size--)
	{
		fix16_t w = *(p_weights++);

		wacc = wacc + frac_mul(w, *(p_window++) << 16);
		acc =  acc + w;
	}

	return frac_div(wacc, acc);
}

inline fix16_t get_deviation_fix(fix16_t weights[], uchar window[], fix16_t mean, uchar size)
{
	register fix16_t acc = 0;
	register fix16_t wacc = 0;

	fix16_t *p_weights = weights;
	uchar *p_window = window;

	while(size--)
	{
		fix16_t w = *(p_weights++);
		fix16_t diff = (*(p_window++) << 16) - mean;
		fix16_t ddiff = frac_mul(diff, diff);
		wacc = wacc + frac_mul(w, ddiff);
		acc =  acc + w;
	}
	
	return fix16_sqrt(fix16_div(wacc, acc));
}

void aswm_filter_fix(cv::Mat image)
{
	uchar *p_img = (uchar *)(image.data);

	uint64 start = GetTimeMs64();

	for (int i = 0; i < image.cols; i++)
	{
		fix16_t weights[9] = { F16(1.0), F16(1.0), F16(1.0), F16(1.0), F16(1.0), F16(1.0), F16(1.0), F16(1.0), F16(1.0) };
		int w_len = 0;

		for (int j = 0; j < image.rows; j++)
		{
			uchar window[9];
			
			window[0] = p_img[image.cols * (j - 1) + i - 1];
			window[1] = p_img[image.cols * (j - 1) + i];
			window[2] = p_img[image.cols * (j - 1) + i + 1];

			window[3] = p_img[image.cols * j + i - 1];
			window[4] = p_img[image.cols * j + i]; //center
			window[5] = p_img[image.cols * j + i + 1];

			window[6] = p_img[image.cols * (j + 1) + i - 1];
			window[7] = p_img[image.cols * (j + 1) + i];
			window[8] = p_img[image.cols * (j + 1) + i + 1];

			fix16_t mean = get_weighted_mean_fix(weights, window, 9);

			while (true)
			{
				//estimate the weights
				for (int l = 0; l < 9; l++)
				{
#if 1
					fix16_t b = fix16_abs((window[l] << 16) - mean) + F16(0.1);
					weights[l] = (((unsigned long long)F16(1.0) << 15) / b) << 1;
#else
					weights[l] = fix16_div(F16(1.0), (fix16_abs(fix16_sub(window[l] << 16, mean)) + F16(0.1)));
#endif
				}

				fix16_t new_mean = get_weighted_mean_fix(weights, window, 9);
				fix16_t diff = fix16_abs(new_mean - mean);
				mean = new_mean;
				if (diff < F16(0.1))
					break;
			}

			fix16_t deviation = get_deviation_fix(weights, window, mean, 9);

			if (fix16_abs((window[4] << 16) - mean) > frac_mul(deviation, F16(6.5)))
			{
				qsort(window, 9, sizeof(uchar), compare_uchar);
				p_img[image.cols * j + i] = window[4];
			}

			weights[0] = weights[3];
			weights[1] = weights[4];
			weights[2] = weights[5];
			weights[3] = weights[6];
			weights[4] = weights[7];
			weights[5] = weights[8];

			weights[6] = F16(1.0);
			weights[7] = F16(1.0);
			weights[8] = F16(1.0);

			w_len = 0;
		}
	}

	uint64 end = GetTimeMs64();

	cout << "aswm fixed mod time: \t" << end - start << endl;
}