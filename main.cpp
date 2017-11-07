#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#include <chrono>

#include "tcv.h"
#include "aswm.h"
#include "sdm.h"

#include "fix16.h"

void neon_our_filter(cv::Mat image);
void our_filter(cv::Mat image);


long long measure(void(*f)(cv::Mat), cv::Mat image)
{
    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

    f(image);

    chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();

    return chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
}



int main(int argc, char** argv )
{
    Mat filt, filt2, original;

    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], IMREAD_GRAYSCALE);

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    image.copyTo(original);

    //imwrite("lena.png", original);

    image = add_salt_pepper(image, 5, 245);

    long long dur = measure(our_filter, filt);
    long long dur_neon = measure(neon_our_filter, filt2);

    for (int i = 0; i < 100000; i++)
    {
        image.copyTo(filt);
        image.copyTo(filt2);

        long long t0 = measure(our_filter, filt);
        long long t1 = measure(neon_our_filter, filt2);

        dur = (dur + t0)/2;
        dur_neon = (dur_neon + t1)/2;

    }
    //std::cout << "SDM PSNR: " << getPSNR(original, filt) << std::endl;
    //std::cout << "PSNR: " << getPSNR(original, filt2) << std::endl;

    cout << "duration: " << dur << endl;
    cout << "duration neon: " << dur_neon << endl;
    cout << "speedup: " << (double)dur / dur_neon << endl;

	//imshow("original", image);
    //imshow("aswm", filt2);
    //imshow("sdm", filt);

    //imwrite("aswm.png", filt);
    //imwrite("aswm_mod.png", filt2);

    cvWaitKey(0);

    return 0;
}