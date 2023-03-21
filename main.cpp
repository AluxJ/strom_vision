#include "rc_r2.h"
#include <iostream>
#include <map>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>
using namespace std;
using namespace cv;

int color_flag = 0;  // 0 is red, 1 is blue
int count_flag = 1;
int main()
{
    VideoCapture cam1;
    Mat          tmp;
    try
    {
        //        cam1.open("/home/nuc/code/RC2022/R2/rc_r2/camera6/videos/4.mp4");
        tmp = imread("/home/nuc/code/RC2022/R2/rc_r2/template/tmp1.jpg");
        VideoCapture cam1("/dev/video0");
    }
    catch (Exception e)
    {
        cout << "wrong is here" << endl;
        cout << e.what() << endl;
    }
    Mat   img_rgb, img_copy;
    Mat   ROI_min_rect, ROI_contours;
    Mat   mid, result, background;
    Block block;
    Mat   red_range, blue_range;
    while (1)
    {

        cam1 >> img_rgb;
        if (!img_rgb.empty())
        {
            imshow("1", img_rgb);
            block.find_red(img_rgb, tmp, mid, red_range);
            block.find_blue(img_rgb, tmp, mid, blue_range);
            ROI_min_rect = block.get_min_rect(img_rgb, red_range, blue_range, mid);
            imshow("min_rect", ROI_min_rect);
            img_copy = block.get_contours(ROI_min_rect, img_rgb, count_flag);
            imshow("rgb", img_copy);
            waitKey(30);
        }
    }

    return 0;
}
