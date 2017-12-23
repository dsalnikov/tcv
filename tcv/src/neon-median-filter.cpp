#include "neon.h"
#include <opencv2/opencv.hpp>

#define vector_size 16


#define vminmax_u8(a, b) \
    do { \
        uint8x16_t minmax_tmp = (a); \
        (a) = vminq_u8((a), (b)); \
        (b) = vmaxq_u8(minmax_tmp, (b)); \
    } while (0)

void our_filter(cv::Mat image)
{
    int i, j;

    uint8_t *src = (uint8_t *)(image.data);

    for (i = 1; i < image.cols; i += 1)
    {
        for (j = 1; j < image.rows; j++)
        {
            uchar window[9];

            window[0] = src[image.cols * (j - 1) + i - 1];
            window[1] = src[image.cols * (j - 1) + i];
            window[2] = src[image.cols * (j - 1) + i + 1];
            window[3] = src[image.cols * j + i - 1];
            window[4] = src[image.cols * j + i];
            window[5] = src[image.cols * j + i + 1];
            window[6] = src[image.cols * (j + 1) + i - 1];
            window[7] = src[image.cols * (j + 1) + i];
            window[8] = src[image.cols * (j + 1) + i + 1];

            std::sort(window, window + 8);

            if (abs(window[4] - src[image.cols * j + i]) > 30)
                src[image.cols * j + i] = window[4];
        }
    }
}


void neon_our_filter(cv::Mat image)
{
	int i, j;

	uint8_t *src = (uint8_t *)(image.data);

	for (i = 1; i < image.cols; i += vector_size)
	{
		for (j = 1; j < image.rows; j++)
		{
			uint8x16_t q0, q1, q2, q3, q4, q5, q6, q7, q8, q4_prev;

			//load 16 windows
			q0 = vld1q_u8(&src[image.cols * (j - 1) + i - 1]);
			q1 = vld1q_u8(&src[image.cols * (j - 1) + i]);
			q2 = vld1q_u8(&src[image.cols * (j - 1) + i + 1]);

			q3 = vld1q_u8(&src[image.cols * j + i - 1]);
            q4_prev = q4 = vld1q_u8(&src[image.cols * j + i]);
			q5 = vld1q_u8(&src[image.cols * j + i + 1]);

			q6 = vld1q_u8(&src[image.cols * (j + 1) + i - 1]);
			q7 = vld1q_u8(&src[image.cols * (j + 1) + i]);
			q8 = vld1q_u8(&src[image.cols * (j + 1) + i + 1]);

			/* Paeth's 9-element sorting network */
			vminmax_u8(q0, q3);
			vminmax_u8(q1, q4);

			vminmax_u8(q0, q1);
			vminmax_u8(q2, q5);

			vminmax_u8(q0, q2);
			vminmax_u8(q4, q5);

			vminmax_u8(q1, q2);
			vminmax_u8(q3, q5);

			vminmax_u8(q3, q4);

			vminmax_u8(q1, q3);

			vminmax_u8(q1, q6);

			vminmax_u8(q4, q6);

			vminmax_u8(q2, q6);

			vminmax_u8(q2, q3);
			vminmax_u8(q4, q7);

			vminmax_u8(q2, q4);

			vminmax_u8(q3, q7);

			vminmax_u8(q4, q8);

			vminmax_u8(q3, q8);

			vminmax_u8(q3, q4);

            uint8x16_t diff = vabdq_u8(q4, q4_prev);
            const uint8x16_t th = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
            uint8x16_t mask = vcgeq_u8(th, diff);

            q4 = vbslq_u8(mask, q4_prev, q4);

			//q4 now - median values
			vst1q_u8(&src[image.cols * j + i], q4);
		}
	}
}
