#ifndef _RECOGNITION_H
#define _RECOGNITION_H
#include <highgui.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
Point templateMatching(Mat src, vector<Mat> templ);
#endif // !_RECOGNITION_H

