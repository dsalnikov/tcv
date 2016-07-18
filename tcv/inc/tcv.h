#ifndef __TCV_H__
#define __TCV_H__

Mat add_gaussian_noise(Mat src, int mean, int deviation);
Mat add_salt_pepper(Mat src, int bot, int top);

void average_filter(Mat image);
void median_filter(Mat image);

double get_mean(uchar window[], uchar size);
double get_variance(uchar window[], uchar size);
double getPSNR(const Mat& I1, const Mat& I2);
double getMAE(const Mat& I1, const Mat& I2);

#endif // !__TCV_H__