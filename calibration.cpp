#include "calibration.h"

const int imageWidth = 640;                             //����ͷ�ķֱ���  
const int imageHeight = 480;
const int boardWidth = 7;                               //����Ľǵ���Ŀ  
const int boardHeight = 5;                              //����Ľǵ�����  
const int boardCorner = boardWidth * boardHeight;       //�ܵĽǵ�����  
const int frameNumber = 13;                             //����궨ʱ��Ҫ���õ�ͼ��֡��  
const int squareSize = 30;                              //�궨��ڰ׸��ӵĴ�С ��λmm  
const Size boardSize = Size(boardWidth, boardHeight);   //  

Mat intrinsic;                                          //����ڲ���  
Mat distortion_coeff;                                   //����������  
vector<Mat> rvecs;                                        //��ת����  
vector<Mat> tvecs;                                        //ƽ������  
vector<vector<Point2f>> corners;                        //����ͼ���ҵ��Ľǵ�ļ��� ��objRealPoint һһ��Ӧ  
vector<vector<Point3f>> objRealPoint;                   //����ͼ��Ľǵ��ʵ���������꼯��  


vector<Point2f> corner;                                   //ĳһ��ͼ���ҵ��Ľǵ�  

Mat rgbImage, grayImage;

/*����궨����ģ���ʵ����������*/
void calRealPoint(vector<vector<Point3f>>& obj, int boardwidth, int boardheight, int imgNumber, int squaresize)
{
	//  Mat imgpoint(boardheight, boardwidth, CV_32FC3,Scalar(0,0,0));  
	vector<Point3f> imgpoint;
	for (int rowIndex = 0; rowIndex < boardheight; rowIndex++)
	{
		for (int colIndex = 0; colIndex < boardwidth; colIndex++)
		{
			//  imgpoint.at<Vec3f>(rowIndex, colIndex) = Vec3f(rowIndex * squaresize, colIndex*squaresize, 0);  
			imgpoint.push_back(Point3f(rowIndex * squaresize, colIndex * squaresize, 0));
		}
	}
	for (int imgIndex = 0; imgIndex < imgNumber; imgIndex++)
	{
		obj.push_back(imgpoint);
	}
}

/*��������ĳ�ʼ���� Ҳ���Բ�����*/
void guessCameraParam(void)
{
	/*�����ڴ�*/
	intrinsic.create(3, 3, CV_64FC1);
	distortion_coeff.create(5, 1, CV_64FC1);

	/*
	fx 0 cx
	0 fy cy
	0 0  1
	*/
	intrinsic.at<double>(0, 0) = 256.8093262;   //fx         
	intrinsic.at<double>(0, 2) = 160.2826538;   //cx  
	intrinsic.at<double>(1, 1) = 254.7511139;   //fy  
	intrinsic.at<double>(1, 2) = 127.6264572;   //cy  

	intrinsic.at<double>(0, 1) = 0;
	intrinsic.at<double>(1, 0) = 0;
	intrinsic.at<double>(2, 0) = 0;
	intrinsic.at<double>(2, 1) = 0;
	intrinsic.at<double>(2, 2) = 1;

	/*
	k1 k2 p1 p2 p3
	*/
	distortion_coeff.at<double>(0, 0) = -0.193740;  //k1  
	distortion_coeff.at<double>(1, 0) = -0.378588;  //k2  
	distortion_coeff.at<double>(2, 0) = 0.028980;   //p1  
	distortion_coeff.at<double>(3, 0) = 0.008136;   //p2  
	distortion_coeff.at<double>(4, 0) = 0;          //p3  
}

void outputCameraParam(void)
{
	/*��������*/
	CvMat cv_mat_intrinsic = intrinsic;
	CvMat cv_mat_distortion_coeff = distortion_coeff;

	cvSave("cameraMatrix.xml", &cv_mat_intrinsic);
	cvSave("cameraDistoration.xml", &cv_mat_distortion_coeff);
	//cvSave("rotatoVector.xml", &rvecs);  
	//cvSave("translationVector.xml", &tvecs);  
	/*�������*/
	cout << "fx :" << intrinsic.at<double>(0, 0) << endl << "fy :" << intrinsic.at<double>(1, 1) << endl;
	cout << "cx :" << intrinsic.at<double>(0, 2) << endl << "cy :" << intrinsic.at<double>(1, 2) << endl;

	cout << "k1 :" << distortion_coeff.at<double>(0, 0) << endl;
	cout << "k2 :" << distortion_coeff.at<double>(1, 0) << endl;
	cout << "p1 :" << distortion_coeff.at<double>(2, 0) << endl;
	cout << "p2 :" << distortion_coeff.at<double>(3, 0) << endl;
	cout << "p3 :" << distortion_coeff.at<double>(4, 0) << endl;


}


void calibrate()
{
	Mat img;
	int goodFrameCount = 0;
	namedWindow("chessboard");
	cout << "��Q�˳� ..." << endl;
	while (goodFrameCount < frameNumber)
	{
		char filename[100];
		sprintf_s(filename, "chessboard\\chess%d.jpg", goodFrameCount++);
		//  cout << filename << endl;  
		rgbImage = imread(filename, CV_LOAD_IMAGE_COLOR);
		cvtColor(rgbImage, grayImage, CV_BGR2GRAY);
		imshow("Camera", grayImage);

		bool isFind = findChessboardCorners(rgbImage, boardSize, corner, 0);
		if (isFind == true) //���нǵ㶼���ҵ� ˵�����ͼ���ǿ��е�  
		{
			/*
			Size(5,5) �������ڵ�һ���С
			Size(-1,-1) ������һ��ߴ�
			TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1)������ֹ����
			*/
			cornerSubPix(grayImage, corner, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 20, 0.1));
			drawChessboardCorners(rgbImage, boardSize, corner, isFind);
			imshow("chessboard", rgbImage);
			corners.push_back(corner);
			//string filename = "res\\image\\calibration";  
			//filename += goodFrameCount + ".jpg";  
			//cvSaveImage(filename.c_str(), &IplImage(rgbImage));       //�Ѻϸ��ͼƬ��������  
			cout << "The image is good" << endl;
		}
		else
		{
			cout << "The image is bad please try again" << endl;
		}
		//  cout << "Press any key to continue..." << endl;  
		//  waitKey(0);  

		if (waitKey(10) == 'q')
		{
			break;
		}
		  imshow("chessboard", rgbImage);  
		  //waitKey(0);
	}

	/*
	ͼ��ɼ���� ��������ʼ����ͷ��У��
	calibrateCamera()
	������� objectPoints  �ǵ��ʵ����������
	imagePoints   �ǵ��ͼ������
	imageSize     ͼ��Ĵ�С
	�������
	cameraMatrix  ������ڲξ���
	distCoeffs    ����Ļ������
	rvecs         ��תʸ��(�����)
	tvecs         ƽ��ʸ��(�������
	*/

	/*����ʵ�ʳ�ʼ���� ����calibrateCamera�� ���flag = 0 Ҳ���Բ���������*/
	guessCameraParam();
	cout << "guess successful" << endl;
	/*����ʵ�ʵ�У�������ά����*/
	calRealPoint(objRealPoint, boardWidth, boardHeight, frameNumber, squareSize);
	cout << "cal real successful" << endl;
	/*�궨����ͷ*/
	calibrateCamera(objRealPoint, corners, Size(imageWidth, imageHeight), intrinsic, distortion_coeff, rvecs, tvecs, 0);
	cout << "calibration successful" << endl;
	/*���沢�������*/
	outputCameraParam();
	cout << "out successful" << endl;

	/*��ʾ����У��Ч��*/
	//Mat cImage;
	//undistort(rgbImage, cImage, intrinsic, distortion_coeff);
	//imshow("Corret Image", cImage);
	//cout << "Correct Image" << endl;
	//cout << "Wait for Key" << endl;
	//waitKey(0);
	//system("pause");
}