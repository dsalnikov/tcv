#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

#include "tcv.h"
#include "aswm.h"

#include "fix16.h"

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

    image = add_salt_pepper(image, 15, 235);
    image.copyTo(filt);
    image.copyTo(filt2);

    aswm_filter_fix(filt);
    aswm_filter(filt2);

    std::cout << "Fix PSNR: " << getPSNR(original, filt) << std::endl;
    std::cout << "PSNR: " << getPSNR(original, filt2) << std::endl;


    imshow("img", image);
    imshow("filtered", filt);

    cvWaitKey(0);

    return 0;
}