#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv){
    VideoCapture video;

    Mat frame, frameBlur, imageHSV, mask, circles;

    video.open(0);
    if (!video.isOpened()) return -1;

    while (true){
        video >> frame;
        GaussianBlur(frame, frameBlur, Size(7,7), 0, 0);
        cvtColor(frameBlur, imageHSV, COLOR_BGR2HSV);

        //Retorna uma imagem binary do resultado do filtro
        inRange(imageHSV, Scalar(0, 0, 230), Scalar(9, 15, 255), mask);
        

        imshow("frameBlur", mask);
        char key = (char)waitKey(30);
        if (key == 27) break;
    }

    return 0;
}
