#include "preprocess.h"

void on_mouse(int event, int x, int y, int flags, void *ustc)
//event����¼����ţ�x,y������꣬flags��ק�ͼ��̲����Ĵ���    
{
	bool �Ƿ񻭾��� = false;//���ɱ���ػ���Ҫ���弸��ȫ�ֱ���������
	Point ������� = Point(-1, -1);
	Point ���λ�� = Point(-1, -1);
	Mat& image = *(cv::Mat*) ustc;//�����Ϳ��Դ���Mat��Ϣ�ˣ��ܻ���
	char temp[16];
	switch (event) {
	case CV_EVENT_LBUTTONDOWN://�������
	{
		sprintf(temp, "(%d,%d)", x, y);
		putText(image, temp, Point(x, y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		�Ƿ񻭾��� = true;
		������� = Point(x, y);
	}	break;
	case CV_EVENT_MOUSEMOVE://�ƶ����
	{
		���λ�� = Point(x, y);
		if (�Ƿ񻭾���)
		{
		}
	}break;
	case EVENT_LBUTTONUP:
	{
		�Ƿ񻭾��� = false;
		sprintf(temp, "(%d,%d)", x, y);
		putText(image, temp, Point(x, y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));

		//���ú������л���
		cv::rectangle(image, �������, ���λ��, cv::Scalar(0, 0, 255));//�����ɫ  
		Mat roi = image(Rect(�������, ���λ��));
		imwrite("template3.jpg", roi);
	}break;
	}
}


Mat preprocess(Mat frame)
{
	//VideoCapture cap(0);
	//while (cap.isOpened())
	//{
		//cap >> frame;
		//Mat frame;
		Mat gray_frame, binary_frame, hsv_frame;
		Mat frame_to_show = frame.clone();
		//frame = imread("pic1.jpg");

		cvtColor(frame, gray_frame, CV_BGR2GRAY);
		cvtColor(frame, hsv_frame, CV_BGR2HSV);
		//imshow("gray", gray_frame);
		/*Mat channels[3];
		split(frame, channels);
		imshow("h", channels[0]);
		imshow("s", channels[1]);
		imshow("v", channels[2]);*/

		//��ֵ��
		//threshold(channels[2], binary_frame, 220, 255, CV_THRESH_BINARY);
		inRange(frame, Scalar(150, 150, 150), Scalar(255, 255, 255), binary_frame);
		//adaptiveThreshold(channels[2],binary_frame,255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY,3,5);
		imshow("binary", binary_frame);
		Mat dilate_frame;
		Mat dilate_element = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(binary_frame, dilate_frame, dilate_element);
		/*Mat erode_frame;
		Mat erode_element = getStructuringElement(MORPH_RECT, Size(3, 3));
		erode(binary_frame, erode_frame, erode_element);


		dilate_element = getStructuringElement(MORPH_RECT, Size(7,7));
		dilate(dilate_frame, dilate_frame, dilate_element);
		erode_element = getStructuringElement(MORPH_RECT, Size(3, 3));
		erode(dilate_frame, dilate_frame, erode_element);*/

		imshow("dilate", dilate_frame);
		/*Mat canny_frame;
		Canny(dilate_frame, canny_frame, 128, 255);
		imshow("canny", canny_frame);*/

		//Ѱ�ҡ�ɸѡ����
		vector< vector<cv::Point> > contours_list;
		vector< vector<cv::Point> > contours_sublist;
		findContours(binary_frame, contours_list, RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		vector<Point> corner;
		for (int i = 0; i < contours_list.size(); i++)
		{
			if (contourArea(contours_list[i]) > 20000)
				contours_sublist.push_back(contours_list[i]);
		}
		//ɸѡ�������20000����С��
		int min_area = 640*480;
		for (int i = 0; i < contours_sublist.size(); i++)
		{
			if (contourArea(contours_sublist[i]) < min_area)
			{
				min_area = contourArea(contours_sublist[i]);
				//cout <<"min_area: "<< min_area << endl;
				corner = contours_sublist[i];
			}
			//drawContours(frame_to_show, contours_sublist, i, Scalar(255), 8);
		}
		//�����ο�
		Rect rect = boundingRect(corner);
		Mat roi = gray_frame(rect);
		rectangle(frame_to_show, rect, Scalar(255), 5);
		imshow("gray", gray_frame);
		Mat roi_image;
		roi.copyTo(roi_image);
		//�ж϶���η���
		
		Point2f corner_dst[4] = {Point2f(0, 0), 
								 Point2f(400, 0),
								 Point2f(400, 300), 
								 Point2f(0, 300)};
		vector < Point2f > poly;
		if (!corner.size())//�ж������Ƿ�Ϊ��
		{
			waitKey(1);
			//continue;
			return Mat(0, 0, CV_8UC1);
		}
		approxPolyDP(corner, poly, 15, true); //��϶����
		if (poly.size() != 4)
			return Mat(0, 0, CV_8UC1);
		//cout << "poly:" << endl;
		//cout << poly << endl;
		int min = 640+480, index=0;
		for (int i = 0; i < 4; i++) {
			if (poly[i].x + poly[i].y < min) {
				min = poly[i].x + poly[i].y;
				index = i;
			}
			line(frame_to_show, poly[i], poly[(i + 1) % 4], Scalar(0, 255, 0), 2, LINE_AA);
		}

		Point2f rect_point = Point2f(rect.x, rect.y);
		Point2f corner_src[4] = { poly[(index + 0) % 4] - rect_point,
								  poly[(index + 1) % 4] - rect_point,
								  poly[(index + 2) % 4] - rect_point,
								  poly[(index + 3) % 4] - rect_point };
		//cout << "corner_src:" << endl;
		//for(auto a:corner_src) cout << a << endl;

		//������Ӱ�任����
		Mat transfer_mat = getPerspectiveTransform(corner_dst, corner_src);
		//cout << transfer_mat;
		
		Mat dst_image;
		warpPerspective(roi_image, dst_image, transfer_mat, Size(400, 300), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP, cv::BORDER_REPLICATE);
		
		equalizeHist(dst_image, dst_image);
		imshow("roi", roi_image);
		imshow("dst", dst_image); 
		imshow("frame_to_show", frame_to_show);

		//setMouseCallback("dst", on_mouse, (void*)&dst_image);

		if(waitKey(100)=='p')
			imwrite("template3.jpg",dst_image);
//	}
	return dst_image;
}