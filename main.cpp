#include "rc_stereoVersion.h"
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
Mat Left, Right;
Point StereoVersion::find_circle(Mat &A_hsv, int camera, int flag) {
  // find contours
  vector<vector<Point>> contours;
  Mat img_contours;
  A_hsv.copyTo(img_contours);
  imshow("img_co", img_contours);
  findContours(img_contours, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE,
               Point(0, 0));
  if (contours.size() > 0)

  {
    // find biggest one contour
    sort(contours.begin(), contours.end(),
         [](const vector<Point> &c1, const vector<Point> &c2) {
           return contourArea(c1, false) > contourArea(c2, false);
         });

    Mat ROI;
    vector<Mat> channels;
    Mat bit_mask;
    // camera is a flag of left and right ,  param flag is to distinguish the
    // red and blue
    if (camera == 1) {
      drawContours(Left, contours, 0, Scalar(0, 255, 0), 3, LINE_8);

      Left.copyTo(ROI);
      if (flag == 0) {
        split(ROI, channels);
        bitwise_and(channels[0], img_contours, bit_mask);
        // return bit_mask;
      }

      else // red
      {
        split(ROI, channels);
        bitwise_and(channels[2], img_contours, bit_mask);
        // imshow("bit mask", bit_mask);
        // return bit_mask;
      }
    }
    // right
    else {
      drawContours(Right, contours, 0, Scalar(0, 255, 0), 3, LINE_8);
      Right.copyTo(ROI);
      if (flag == 0) {
        split(ROI, channels);
        bitwise_and(channels[0], img_contours, bit_mask);
        // return bit_mask;
      } else // red
      {
        split(ROI, channels);
        bitwise_and(channels[2], img_contours, bit_mask);
        // return bit_mask;
        // imshow("bit mask", bit_mask);
      }
    }

    Moments M = moments(contours[0]);
    Point centre_point = Point(M.m10 / M.m00, M.m01 / M.m00);

    return centre_point;
  } else
    return Point(0, 0);
}

int main() {
  StereoVersion StereoCamera(36, 40);
  int flag = 1; // flag is a parma to distinguish blue and red ,flag=1 means red
                // ,flag=0 means blue
  VideoCapture camera1(2);
  VideoCapture camera2(4);

  Mat left_gray, left_hsv;
  Mat right_gray, right_hsv;
  Mat red_l_bit, red_r_bit; // imgs with the suffix "bit" means done bitwise_and
                            // to delete useless imformation
  Mat blue_l_bit, blue_r_bit;
  cout << "flag is" << flag << endl;
  while (1) {

    camera1 >> Right;
    camera2 >> Left;
    // color is blue
    if (flag == 0) {
      left_hsv = StereoCamera.find_blue(Left, "left");
      Point blue_l_centre = StereoCamera.find_circle(left_hsv, 1, 0);

      right_hsv = StereoCamera.find_blue(Right, "right");

      Point blue_r_centre = StereoCamera.find_circle(right_hsv, 1, 0);
      double blue_dis = StereoCamera.find_depth(blue_l_centre, blue_r_centre,
                                                left_hsv, right_hsv);
      cout << blue_dis << endl;

      Mat BG = StereoCamera.show2ps(right_hsv, left_hsv);
      imshow("BLUE HSV", BG);
    }
    // color is red
    else {
      left_hsv = StereoCamera.find_red(Left, "left");
      Point red_l_centre = StereoCamera.find_circle(left_hsv, 1, 1);
      right_hsv = StereoCamera.find_red(Right, "right");
      Point red_r_centre = StereoCamera.find_circle(right_hsv, 0, 1);

      double red_dis = StereoCamera.find_depth(red_l_centre, red_r_centre,
                                               left_hsv, right_hsv);
      cout << "dis is" << red_dis << endl;
    }

    Mat H = StereoCamera.show2ps(Right, Left);
    imshow("h", H);
    waitKey(1);
  }

  return 0;
}
