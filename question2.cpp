#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<cmath>

using namespace cv;
using namespace std;

Mat sobelOperator(Mat img,int order){
	int scale = 1,delta = 0,ddepth = CV_16S;
	Mat grayImg;

	GaussianBlur( img, img, Size(3,3), 0, 0, BORDER_DEFAULT );

	cvtColor( img, grayImg, CV_BGR2GRAY );
	Mat imgDer,imgDerX, imgDerY,gradImgDerX, gradImgDerY;

	Sobel( grayImg, imgDerX, ddepth, 2, 0, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( imgDerX, gradImgDerX );

	Sobel( grayImg, imgDerY, ddepth, 0, 2, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( imgDerY, gradImgDerY);
	
	addWeighted( gradImgDerX, 0.5, gradImgDerY, 0.5, 0, imgDer);
	return imgDer;	
}

int main (int argv, char** args){
	Mat img = imread(args[1]);
	if( !img.data )
		return -1;
	int order=2;
	Mat imgDer=sobelOperator(img,order);
	Mat binary;
	
	threshold( imgDer,binary,20, 255,0);
	
	namedWindow("Original Image", CV_WINDOW_AUTOSIZE );
	imshow("Original Image", img );
	namedWindow("Sobel Operator second order", CV_WINDOW_AUTOSIZE );
	imshow("Sobel Operator second order", imgDer );
	namedWindow("Binary edge", CV_WINDOW_AUTOSIZE );
	imshow("Binary edge", binary);
	
	waitKey(0);
}
