#ifndef RC_R2_H
#define RC_R2_H
#include "Debug.h"
#include <iostream>
#include <map>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#define test 0

class Initcolor
{
public:
    vector<int> red{ 165, 255, 43, 255, 46, 255 };
    vector<int> blue{ 100, 124, 43, 255, 46, 255 };
};

class HsV
{
    friend class Initcolor;
    friend class Block;

private:
    int       h_max;
    int       h_min;
    int       s_max, s_min;
    int       v_max, v_min;
    Initcolor c;

public:
    HsV(int h_max, int h_min, int s_max, int s_min, int v_max, int v_min) : h_max(h_max), h_min(h_min), s_max(s_max), s_min(s_min), v_max(v_max), v_min(v_min){};
    ~HsV() {}
    Mat range(Mat& img_rgb, int color_flag)
    {
        Mat img_hsv, img_range;
        cvtColor(img_rgb, img_hsv, COLOR_BGR2HSV);
        vector<Mat> hsv_channels;
        split(img_hsv, hsv_channels);
        equalizeHist(hsv_channels[2], hsv_channels[2]);
        hsv_channels[2] += 100;
        merge(hsv_channels, img_hsv);
#if test
        imshow("img_hsv", img_hsv);
#endif
        if (color_flag == 0)  // red
        {
            inRange(img_hsv, Scalar(c.red[0], c.red[2], c.red[4]), Scalar(c.red[1], c.red[3], c.red[5]), img_range);
            return img_range;
        }
        else  // blue
        {
            inRange(img_hsv, Scalar(c.blue[0], c.blue[2], c.blue[4]), Scalar(c.blue[1], c.blue[3], c.blue[5]), img_range);
            return img_range;
        }
    }
};
class Block
{
    friend class tools;

private:
    float radius;
    int   color_flag;     // 0 is equal to blue,1 is equal to red
    int   color_capture;  // Current color which is captured

public:
    // template match
    Mat match(Mat& img_rgb, Mat& tmp, Mat& result)
    {

        matchTemplate(img_rgb, tmp, result, TM_SQDIFF);

        double maxvel, minvel;
        Point  Maxloc, Minloc;
        minMaxLoc(result, &minvel, &maxvel, &Maxloc, &Minloc);
        Rect R(Maxloc.x, Maxloc.y, tmp.cols, tmp.rows);

        result = img_rgb(R);
#if test
        imshow("img_rgb", result);
#endif
        return result;
    }
    Mat get_min_rect(Mat& img_rgb, Mat& red_range, Mat& blue_range, Mat& min_rect_result)
    {
        if (red_range.empty() | blue_range.empty())
        {
            cout << "picture values err" << endl;
            exit(0);
        }

        Mat bit_and_copy;

        bitwise_or(red_range, blue_range, bit_and_copy);

        medianBlur(bit_and_copy, bit_and_copy, 9);
        vector<vector<Point>> c1;
        Point2f               rotate_points[4];
        vector<Point2f>       Points;
        Point2f               center;

        Mat kernel = getStructuringElement(0, Size(7, 7));

        morphologyEx(bit_and_copy, bit_and_copy, MORPH_CLOSE, kernel);
        morphologyEx(bit_and_copy, bit_and_copy, MORPH_OPEN, kernel);

        morphologyEx(bit_and_copy, bit_and_copy, MORPH_CLOSE, kernel);

        threshold(bit_and_copy, bit_and_copy, 254, 255, THRESH_OTSU | THRESH_BINARY);
#if test
        imshow("bit_wise_and", bit_and_copy);
#endif
        bitwise_not(bit_and_copy, bit_and_copy);
#if test
        imshow("bit_not", bit_and_copy);
#endif
        findContours(bit_and_copy, c1, 1, 2);

        try
        {

            return bit_and_copy;
        }
        catch (Exception e)
        {
            cout << "here is wrong" << endl;
            cout << e.what() << endl;
        }
    }
    Mat get_contours(Mat& img_binary, Mat& img_rgb, int& count)
    {
        cout << "--------------------------------" << count << "-------------------------------------" << endl;
        try
        {
            vector<long double> dis_set;
            count++;
            int                   axis_right_refer = 550, axis_left_refer = 150;
            float                 refer = 0;
            float                 distance;
            long double           dis;
            vector<vector<Point>> c;
            findContours(img_binary, c, 1, 1);
#if test
            imshow("img_binary", img_binary);
#endif

            for (size_t i = 0; i < c.size(); i++)
            {

                Rect tmp_rect = boundingRect(c[i]);
                if (tmp_rect.width > tmp_rect.height)
                {
                    if (tmp_rect.area() < 500)
                    {
                        if (tmp_rect.x > axis_left_refer && tmp_rect.x < axis_right_refer)
                        {
                            dis      = ((tmp_rect.width * tmp_rect.width) + (tmp_rect.height * tmp_rect.height));
                            distance = sqrt(dis);
                            cout << "rect:" << tmp_rect.x << " " << tmp_rect.y << " "
                                 << "distance is " << distance << "!!!!!!!" << endl;
                            dis_set.push_back(distance);
                            rectangle(img_rgb, tmp_rect, Scalar(255, 255, 0), 2, 8, 0);
                        }
                    }
                }
            }

            if (dis_set.size() > 0)
            {
                sort(dis_set.begin(), dis_set.end(), [](const long double& c1, const long double& c2) { return c1 < c2; });
                ostringstream os1, os2;
                os1 << "longgest distance:" << dis_set[dis_set.size() - 1];
                cout << "here is ok555" << endl;
                String text1 = os1.str();
                os2 << "shortest distance:" << dis_set[0];
                String text2 = os2.str();

                putText(img_rgb, text1, Point(20, 20), 0, 1, Scalar(255, 0, 255), 1, 8);
                putText(img_rgb, text2, Point(20, 80), 0, 1, Scalar(255, 0, 255), 1, 8);
            }
            return img_rgb;
        }
        catch (Exception e)
        {
            cout << e.what() << endl;
        }
    }
    Mat find_red(Mat& img_rgb, Mat& tmp, Mat& mid, Mat& result)
    {
        Initcolor colors;
        HsV       Red(colors.red[0], colors.red[1], colors.red[2], colors.red[3], colors.red[4], colors.red[5]);
        //        result = match(img_rgb, tmp, mid);
        result = Red.range(img_rgb, 0);
        return result;
    }
    Mat find_blue(Mat& img_rgb, Mat& tmp, Mat& mid, Mat& result)
    {
        Initcolor colors;
        HsV       Blue(colors.blue[0], colors.blue[1], colors.blue[2], colors.blue[3], colors.blue[4], colors.blue[5]);
        //        result = match(img_rgb, tmp, mid);
        result = Blue.range(img_rgb, 1);
        return result;
    }
};
class tools
{
    friend class Block;

public:
    static Mat show2ps(Mat& A, Mat& B)
    {

        Size imgsize    = A.size();
        Mat  BackGround = Mat::zeros(Size(imgsize.width * 2, imgsize.height), A.type());

        Rect R_A(0, 0, A.cols, A.rows);
        Mat  ROI_A = BackGround(R_A);
        A.copyTo(ROI_A);
        Rect R_B(imgsize.width, 0, B.cols, B.rows);
        Mat  ROI_B = BackGround(R_B);
        B.copyTo(ROI_B);

        return BackGround;
    }
};

#endif  // RC_R2_H
