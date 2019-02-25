#include "recognition.h"
Point templateMatching(Mat src, vector<Mat> templ)
{

	Mat image_matched;
	Mat sim_mat;
	double minVal, maxVal;
	Point minLoc, maxLoc;
	double targetVal = 0;
	Point targetLoc;
	//Ä£°åÆ¥Åä
	for (int i =0;i<templ.size();i++)
	{
		matchTemplate(src, templ[i], image_matched, TM_CCOEFF_NORMED);
		//Ñ°ÕÒ×î¼ÑÆ¥ÅäÎ»ÖÃ
		minMaxLoc(image_matched, &minVal, &maxVal, &minLoc, &maxLoc);
		if (targetVal < maxVal)
		{
			targetVal = maxVal;
			targetLoc = maxLoc;
			sim_mat = templ[i];
		}
	}
	Mat image_color;
	//cvtColor(src, image_color, CV_GRAY2BGR);
	cout << targetVal << endl;
	//cout << maxLoc << endl;
	if (maxVal < 0.75)
	{
		return Point(0, 0);
	}
	
	rectangle(src, Point(targetLoc.x, targetLoc.y), Point(targetLoc.x + sim_mat.cols, targetLoc.y + sim_mat.rows), Scalar(255));
	//imshow("source image", src);
	imshow("match result", image_matched);
	imshow("target", src);
	//cv::waitKey(0);

	return Point(maxLoc.x, maxLoc.y);
}

