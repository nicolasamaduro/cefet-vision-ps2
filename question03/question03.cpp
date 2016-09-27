#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void computeDft(Mat img, Mat *phaImg, Mat *magImg) {
    Mat paddedImg;
    int m = getOptimalDFTSize( img.rows );
    int n = getOptimalDFTSize( img.cols );
    copyMakeBorder(img, paddedImg, 0, m - img.rows, 0, n - img.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat planesImg[] = {Mat_<float>(paddedImg), Mat::zeros(paddedImg.size(), CV_32F)};
    Mat complexImg;
    merge(planesImg, 2, complexImg);

    dft(complexImg, complexImg);
    split(complexImg, planesImg);

    magnitude(planesImg[0], planesImg[1], *magImg);
    phase(planesImg[0], planesImg[1], *phaImg);
}

Mat CannyThreshold(Mat img, int thr1, int thr2){
    Mat edgesImg;
    Canny( img, edgesImg, thr1, thr2, 3 );
    return edgesImg;
}

Mat computeInverseDft(Mat phaImg,Mat magImg){
    Mat plane[2];
    polarToCart(magImg, phaImg, plane[0], plane[1]);

    Mat inverseComplexImg, inverseImg;

    merge(plane, 2, inverseComplexImg);
    dft(inverseComplexImg, inverseImg , DFT_INVERSE | DFT_REAL_OUTPUT);
    normalize(inverseImg, inverseImg, 0, 1, CV_MINMAX);

    return inverseImg;
}

void CallBackFunc(int event, int x, int y, int flags, void* param){
    Mat* img = (Mat*) param;
    int windowsSize = 250;

    namedWindow("Window",	WINDOW_AUTOSIZE);
    namedWindow("Window - Edges Image",	WINDOW_AUTOSIZE);
    namedWindow("Window - Phase Edges Image",	WINDOW_AUTOSIZE);
    namedWindow("Window - Magnitude Edges Image",	WINDOW_AUTOSIZE);
    namedWindow("Window - Phase Image",	WINDOW_AUTOSIZE);
    namedWindow("Window - Magnitude Image",	WINDOW_AUTOSIZE);
    namedWindow("Window - Inverse Edges Image",	WINDOW_AUTOSIZE);

    if ( event == EVENT_MOUSEMOVE )
    {
        unsigned int k, l;
        if (x > (windowsSize) / 2 &&	y > (windowsSize) / 2 && x < (*img).cols - (windowsSize) / 2 && y < (*img).rows - (windowsSize) / 2) {
            Mat zoomNoiseImg(windowsSize, windowsSize, CV_8U, Scalar(0));
            for (unsigned int i = x - (windowsSize) / 2, k = 0; i < x + (windowsSize) / 2; k++, i++) {
                for (unsigned int j = y - (windowsSize) / 2, l = 0; j < y + (windowsSize) / 2; l++, j++) {
                    zoomNoiseImg.at<uchar>(l, k) = (*img).at<uchar>(j, i);
                }
            }
            Mat zoomImg;
            blur( zoomNoiseImg, zoomImg, Size(3, 3));

            Mat phaImg, magImg, absPhaImg, absMagImg;
            computeDft(zoomImg, &phaImg, &magImg);

            convertScaleAbs(phaImg, absPhaImg);
            convertScaleAbs(magImg, absMagImg);

            Mat edgesPhaImg = CannyThreshold(absPhaImg, 10, 30);
            Mat edgesMagImg = CannyThreshold(absMagImg, 10, 30);

            edgesPhaImg.convertTo(absPhaImg,CV_32F,1,0);
            edgesMagImg.convertTo(absMagImg,CV_32F,1,0);

            Mat inverseEdgeImg=computeInverseDft(absPhaImg,absMagImg);
            Mat edgesImg = CannyThreshold(zoomImg,40,120);

            imshow("Window", zoomImg);
            imshow("Window - Edges Image", edgesImg);
            imshow("Window - Phase Edges Image", edgesPhaImg);
            imshow("Window - Magnitude Edges Image", edgesMagImg);
            imshow("Window - Phase Image", phaImg);
            imshow("Window - Magnitude Image", magImg);
            imshow("Window - Inverse Edges Image", inverseEdgeImg);
        }
    }
}

int main (int argv, char** args) {
    Mat img = imread(args[1], CV_LOAD_IMAGE_GRAYSCALE);
    namedWindow("Auto",	WINDOW_AUTOSIZE);
    setMouseCallback("Auto", CallBackFunc, &img);
    imshow("Auto", img);
    waitKey(0);
    return 0;
}
