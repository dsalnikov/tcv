#include <opencv2/opencv.hpp>
#include "GetTimeMs64.h"

#include <iostream>
using namespace std;

int sdm_filter(cv::Mat image, uchar threshold) {
    uchar *p_img = (uchar *)(image.data);

    static uint64 max_time = 0;
    static uint64 min_time = 0xFFFFFFFF;
    static uint64 mean_time = 0;
    static uint64 mean_num = 1;

    uint64 start = GetTimeMs64();

    for (int i = 1; i < image.cols; i++)
    {
        for (int j = 1; j < image.rows; j++)
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

            uchar v_diff = ((ushort)window[1] + window[7]) >> 1;
            uchar h_diff = ((ushort)window[3] + window[5]) >> 1;

            uchar ld_diff = ((ushort)window[0] + window[8]) >> 1;
            uchar rd_diff = ((ushort)window[2] + window[6]) >> 1;

            if ((abs(v_diff - window[4]) > threshold) && (abs(h_diff - window[4]) > threshold) &&
                (abs(ld_diff - window[4]) > threshold) && (abs(rd_diff - window[4]) > threshold))
            {

                //qsort(window, 9, sizeof(uchar), compare_uchar);
                for (char k = 0; k < 9; k++) {
                    for (char m = 0; m < 9-1-k; m++) {
                        if (window[m] > window[m+1]) {
                            char tmp = window[m];
                            window[m] = window[m+1];
                            window[m+1] = tmp;
                        }
                    }
                }
                p_img[image.cols * j + i] = window[4];
            }

        }
    }

    uint64 end = GetTimeMs64();

    cout << "me time: \t" << end - start << endl;

    mean_time += end - start;

    if (max_time < end - start)
        max_time = end - start;

    if (min_time > end - start)
        min_time = end - start;

    cout << "min: \t" << min_time << "max: \t" << max_time << "mean: \t" << mean_time / mean_num++ << endl;

    return 0;
}
