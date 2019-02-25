#include "main.h"

double distortion_coeff_para[5] = { -3.8105117843095559e-01, 2.4164351661242375e-01,
9.3293570553525465e-04, -1.9526088751923355e-03,-1.1826323216543390e-01 };
double intrinsic_para[3][3] = { 4.0995413361138139e+02, 0, 3.0697437782314427e+02, 0,
4.1130032965650275e+02, 2.3498032087999141e+02, 0., 0., 1. };


int main()
{
	//串口初始化
	HANDLE Com4 = initialSerialPort("COM3");
	//calibrate();
	Mat distortion_coeff(1,5, CV_32FC1), intrinsic(3, 3, CV_32FC1);
	for (int i = 0; i < 5; i++)
		distortion_coeff.at<float>(0, i) = distortion_coeff_para[i];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			intrinsic.at<float>(i, j) = intrinsic_para[i][j];
	int order = 0, order_last = 0;
	int counter = 0;

	Mat R = Mat::eye(3, 3, CV_32F);
	Mat mapx = Mat(Size(640, 480), CV_32FC1);   //X 坐标重映射参数
	Mat mapy = Mat(Size(640, 480), CV_32FC1);   //Y 坐标重映射参数
	//cout << distortion_coeff << endl;
	//cout << intrinsic << endl;
	initUndistortRectifyMap(intrinsic, distortion_coeff, R, Mat(), Size(640, 480), CV_32FC1, mapx, mapy);

	VideoCapture cap(0);//("C:\\Users\\Yiran Chen\\PycharmProjects\\whacAMole\\地鼠.mp4");
	if (!cap.isOpened())
	{
		cout << "Failed to open video/camera! " << endl;
		system("pause");
		return 0;
	}
	Mat src_frame, frame;
	vector<Mat> template_images;
	for (int i = 0; i < 8; i++)
	{
		string path = "template4." + to_string(i + 1) + ".jpg";
		Mat temp = imread(path, IMREAD_GRAYSCALE);
		template_images.push_back(temp);
	}
	Mat templateGray;
	///cvtColor(templateImg, templateGray, COLOR_BGR2GRAY);
	namedWindow("template", CV_WINDOW_NORMAL);
	
	while (cap.isOpened())
	{
		cap >> src_frame;
		if (src_frame.empty())
		{
			cout << "frame is empty! " << endl;
			break;
		}
		remap(src_frame, src_frame, mapx, mapy, INTER_LINEAR);
		frame = src_frame(Rect(Point(150,150),Point(550,380)));
		imshow("frame",frame);
		//frame = imread("pic1.jpg");
		Mat transformed_frame = preprocess(frame);
		if (transformed_frame.size() == Size(0, 0))
		{
			waitKey(1);
			continue;
		}

		Point targetPoint = templateMatching(transformed_frame, template_images);
		if (targetPoint == Point(0, 0))
		{
			waitKey(1);
			continue;
		}
		//cout << "x: " << targetPoint.x << endl;
		//cout << "y: " << targetPoint.y << endl;
		
		//确定地鼠的序号
		Point coord;
		if (targetPoint.x < 70)
			coord.x = 1;
		else if (targetPoint.x < 266)//266
			coord.x = 2;
		else coord.x = 3;
		if (targetPoint.y < 100)//100
			coord.y = 1;
		else if (targetPoint.y < 170)//150
			coord.y = 2;
		else coord.y = 3;
		order = coord.x + coord.y * 3 - 3;  //1~9

		//串口发送
		string result;
		counter++;
		if (counter >= 1)
		{
			cout << "oder_last: " << order_last << endl;
			cout << "order: " << order << endl;

			result = to_string(order);
			order_last = order;

			counter = 0;
			serialSend(Com4, result);
			cout << "Order number of the mole: " << result << endl;
		}
				

		if (cvWaitKey(10) == 27) //Esc 退出
			break;
	}
	cap.release();
	destroyAllWindows();
	system("pause");
	return 0;
}
