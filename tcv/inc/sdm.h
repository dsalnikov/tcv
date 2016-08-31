#ifndef __SDM_H__
#define __SDM_H__

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/imgproc/imgproc.hpp"

int sdm_filter(cv::Mat image, uchar threshold);

#endif // !__SDM_H__