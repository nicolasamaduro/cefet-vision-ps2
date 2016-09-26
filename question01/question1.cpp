#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

Mat histogramEqualization(Mat histogram){
	float Q=0;
	int Gmax=histogram.rows;
	Mat newHistogram(histogram.clone());
	for(int i=1;i<Gmax;i++){
			newHistogram.at<float>(i,0) += newHistogram.at<float>(i-1,0);
	}
	return newHistogram;
}

Mat computeHistogramEqualized(Mat histogram, Mat img,float r){
	Mat newImg(img.clone());
	float sum=0;
	for (unsigned int w=0;w<histogram.rows;w++)
		sum+=histogram.at<float>(w,0);
	
	for(unsigned int i=0;i<img.rows;i++){
		for(unsigned int j=0;j<img.cols;j++){
			newImg.at<uchar>(i,j)=img.at<uchar>(i,j)*pow(histogram.at<float>(img.at<uchar>(i,j),0),r);
		}
	}
	return newImg;
}

Mat getHistogram1C(Mat img){
	int histSize = 256;
  	float range[] = { 0, 256 } ;
  	const float* histRange = { range };
  	bool uniform = true; 
	bool accumulate = false;
	Mat hist;
  	calcHist( &img, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );
  	hist /= (img.cols * img.rows);	
  	return hist;
}

Mat drawHistogram1C(Mat histogram){	
	int histSize=histogram.rows;
	int hist_w = 512; int hist_h = 400;
  	int bin_w = cvRound( (double) hist_w/histSize );
	Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
	normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
	for( int i = 1; i < histSize; i++ ){
      	line( histImage, Point( bin_w*(i-1), hist_h - cvRound(histogram.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(histogram.at<float>(i)) ),
                       Scalar( 255,255, 255), 2, 8, 0  );
	}
	return histImage;
}

Mat sigmaFilter(Mat imgNoise, uchar sigma){
	Mat img(imgNoise.clone());
	
	for(unsigned int i=0;i<imgNoise.rows;i++){
		for(unsigned int j=0;j<imgNoise.cols;j++){
			long int sum=0, cont=0;
			for(int ii=-1;ii<=1;ii++)
				if(i+ii>=0 && i+ii <imgNoise.rows)
					for(int jj=-1;jj<=1;jj++)
						if(j+jj>=0 && j+jj <imgNoise.cols)
							if(abs(imgNoise.at<uchar>(i,j)-imgNoise.at<uchar>(i+ii,j+jj))<sigma){
								sum=sum+imgNoise.at<uchar>(i+ii,j+jj);
								cont++;
							}
			if(cont)
				img.at<uchar>(i,j)=(int)sum/cont;
		}
	}
	return	img;
} 


int main (int argv, char** args){
	Mat img = imread(args[1],CV_LOAD_IMAGE_GRAYSCALE);
	uchar sigma=atoi(args[2]);
	float r =atoi(args[3])/10.0;
	Mat filteredImg=sigmaFilter(img,sigma);
	
	Mat hist = getHistogram1C(filteredImg);
	Mat equaHist = histogramEqualization(hist);
	Mat equaImg = computeHistogramEqualized(equaHist,filteredImg,r);

	namedWindow("Original image", CV_WINDOW_AUTOSIZE );
	imshow("Original image", img );
	namedWindow("Sigma Filter image", CV_WINDOW_AUTOSIZE );
	imshow("Sigma Filter image", filteredImg);
	namedWindow("Imagem equalizada", CV_WINDOW_AUTOSIZE );
	imshow("Imagem equalizada", equaImg);
	
	waitKey(0);

}
