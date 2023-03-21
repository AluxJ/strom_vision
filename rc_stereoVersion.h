#ifndef RC_STEREOVERSION_H
#define RC_STEREOVERSION_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;
class StereoVersion {
public:
  StereoVersion(double f_lngth, double baseline)
      : f_length(f_length), baseline(baseline){};

public:
  void sgbm(Mat &L, Mat &R, Mat &OutPut);

  void budget(Mat &M_l, Mat &dis_l, Mat &M_r, Mat &dis_r, Mat &inputArray_l,
              Mat &inputArray_r, Mat &outputArry_l, Mat &outputArry_r,
              Size img_size, Mat &R_T, Mat &T_T);

  Mat show2ps(Mat &A, Mat &B);
  Mat find_red(Mat &A, const String &camear);
  Mat find_blue(Mat &A, const String &camear);
  Point find_circle(Mat &A_hsv, int camera, int flag);
  double find_depth(Point &l_c_p, Point &r_c_p, Mat &l_img, Mat &r_img);

private:
  double f_length = 0;
  double baseline = 0;
};
Mat StereoVersion::show2ps(Mat &A, Mat &B) {

  Size imgsize = A.size();
  Mat BackGround =
      Mat::zeros(Size(imgsize.width * 2, imgsize.height), A.type());

  Rect R_A(0, 0, A.cols, A.rows);
  Mat ROI_A = BackGround(R_A);
  A.copyTo(ROI_A);
  Rect R_B(imgsize.width, 0, B.cols, B.rows);
  Mat ROI_B = BackGround(R_B);
  B.copyTo(ROI_B);

  return BackGround;
}

Mat StereoVersion::find_red(Mat &A, const String &camear) {
  // hsv
  Mat img_hsv;
  cvtColor(A, img_hsv, COLOR_BGR2HSV);

  vector<Mat> channels;
  split(img_hsv, channels);
  equalizeHist(channels[2], channels[2]);
  merge(channels, img_hsv);
  Scalar red_min = {165, 43, 46};
  Scalar red_max = {255, 255, 255};
  Mat img_range;
  inRange(img_hsv, red_min, red_max, img_range);

  Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
  morphologyEx(img_range, img_range, MORPH_OPEN, element);
  morphologyEx(img_range, img_range, MORPH_CLOSE, element);
  Mat element2 = getStructuringElement(MORPH_RECT, Size(1, 1));
  dilate(img_range, img_range, element2);
  return img_range;
}
// blue's hsv range is not sure
Mat StereoVersion::find_blue(Mat &A, const String &camear) {
  // hsv
  Mat img_hsv;
  cvtColor(A, img_hsv, COLOR_BGR2HSV);

  vector<Mat> channels;
  split(img_hsv, channels);
  equalizeHist(channels[0], channels[0]);
  merge(channels, img_hsv);
  Scalar blue_min = {175, 43, 46};
  Scalar blue_max = {255, 255, 255};
  Mat img_range;
  inRange(img_hsv, blue_min, blue_max, img_range);
  Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
  morphologyEx(img_range, img_range, MORPH_OPEN, element);
  morphologyEx(img_range, img_range, MORPH_CLOSE, element);
  Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
  dilate(img_range, img_range, element2);
  return img_range;
}

double StereoVersion::find_depth(Point &l_c_p, Point &r_c_p, Mat &l_img,
                                 Mat &r_img) {
  if (l_img.cols == r_img.cols) {
    f_length = 36; // mm
                   // f = (r_img.cols * 0.5) / tan(alpha * 0.5 * CV_PI / 180.0);
  } else {
    cout << "right is not same to left" << endl;
  }

  int x_left = l_c_p.x;
  int x_right = r_c_p.x;
  int disparity = abs(x_left - x_right);
  double dist = (baseline * f_length) / double(disparity);

  return abs(dist);
}

#endif // RC_STEREOVERSION_H
