#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv){
    VideoCapture video;

    Mat frame, frameBlur, imageHSV, mask;

    video.open(0);
    if (!video.isOpened()) return -1;

    while (true){
        vector<Vec3f> circles;
        
        video >> frame;
        GaussianBlur(frame, frameBlur, Size(7,7), 0, 0);  
        cvtColor(frameBlur, imageHSV, COLOR_BGR2HSV);

        //Retorna uma imagem binary do resultado do filtro
        //inRange(imageHSV, Scalar(55, 144, 17), Scalar(140, 255, 255), mask);
        inRange(imageHSV, Scalar(150, 0, 230), Scalar(255, 160, 255), mask);

        Mat element = getStructuringElement( MORPH_ELLIPSE, Size(10, 10), Point(-1,-1));
        erode(mask, mask, element);
        dilate(mask, mask, element);

        //HoughCircles(mask, circles, CV_HOUGH_GRADIENT, 1, 150, 100, 20, 0, 0 );
        HoughCircles(mask, circles, CV_HOUGH_GRADIENT, 1, 150, 50, 10, 0, 0);

        if (!circles.empty())
            cout << circles[0][2] << endl;

        for( size_t i = 0; i < circles.size(); i++ ){
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle(frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle(frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }

        flip(frame, frame, 1);
        imshow("Img", frame);
        imshow("mask", mask);
        char key = (char)waitKey(30);
        if (key == 27) break;
    }

    return 0;
}
