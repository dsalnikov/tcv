#include <opencv2/opencv.hpp>

using namespace cv;

Mat add_gaussian_noise(Mat src, int mean, int deviation)
{
    Mat gaussian_noise = src.clone();
    randn(gaussian_noise, mean, deviation);

    return gaussian_noise + src;
}

Mat add_salt_pepper(Mat src, int bot, int top)
{
    Mat saltpepper_noise = Mat::zeros(src.rows, src.cols, CV_8U);
    randu(saltpepper_noise, 0, 255);

    Mat black = saltpepper_noise < bot;
    Mat white = saltpepper_noise > top;

    Mat saltpepper_img = src.clone();
    saltpepper_img.setTo(255, white);
    saltpepper_img.setTo(0, black);
    return saltpepper_img;
}


void average_filter(Mat image)
{
    uchar *p_img = (uchar *)(image.data);

    for (int i = 1; i < image.cols - 1; i++)
    {
        for (int j = 1; j < image.rows - 1; j++)
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

            uchar center = 0;
            for (int l = 0; l < 9; l++)
            {
                center += window[l] / 9;
            }
            p_img[image.cols * j + i] = center;
        }
    }
}

int compare_uchar(const void * a, const void * b)
{
	return (*(uchar*)a - *(uchar*)b);
}

void median_filter(Mat image)
{
    uchar *p_img = (uchar *)(image.data);

    for (int i = 0; i < image.cols; i++)
    {
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

            
            qsort(window, 9, sizeof(uchar), compare_uchar);
            p_img[image.cols * j + i] = window[4];
        }
    }

}

double get_mean(uchar window[], uchar size)
{
    double acc = 0;
    for (int i = 0; i < size; i++)
    {
        acc += window[i] / (double)size;
    }
    return acc;
}

double get_variance(uchar window[], uchar size)
{
    double acc = 0;
    double mean = get_mean(window, size);
    for (int i = 0; i < size; i++)
    {
        acc += pow((double)window[i] - mean, 2) / size;
    }
    return acc;
}

double getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);         // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if (sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0*log10((255 * 255) / mse);
        return psnr;
    }
}

double getMAE(const Mat& I1, const Mat& I2)
{
    Mat error = abs(I1 - I2);
    Scalar s = sum(error);
    double se = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if (se <= 1e-10) // for small values return zero
        return 0;
    else
    {
        return se / (double)(I1.channels() * I1.total());
    }
}