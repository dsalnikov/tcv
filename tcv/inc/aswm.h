#ifndef __ASWM_H__
#define __ASWM_H__

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/imgproc/imgproc.hpp"

double get_weighted_mean(double weights[], uchar window[], uchar size);
double get_deviation(double weights[], uchar window[], double mean, uchar size);
int aswm_filter(cv::Mat image);
int aswm_mod_filter(cv::Mat image);
int aswm_mod2_filter(cv::Mat image, cv::Mat prev_frame, cv::Mat prev_frame_res);
int compare_uchar(const void * a, const void * b);


void aswm_filter_fix(cv::Mat image);
void aswm_filter_mod2_fix(cv::Mat image, cv::Mat prev_frame, cv::Mat prev_frame_res);

#endif // !__ASWM_H__