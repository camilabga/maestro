#include <sstream>
#include <string>
#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;
using namespace std;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 157;
int H_MAX = 256;
int S_MIN = 146;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

float cont;
ofstream save;
Mat trajetoria;

float previous_x = 0, previous_y = 0;
float difX, difY;

void seePoints(int x, int y){
	circle(trajetoria, Point(x,y), 2, Scalar(255, 255, 255), 1, 8, 0);
}

void saveData(float x, float y, float i){
	//if(i%10 == 1) {
		x = (x-323)/(FRAME_WIDTH-323);
		y = (FRAME_HEIGHT-y)/(FRAME_HEIGHT-69);
		difX = x - previous_x;
		difY = y - previous_y;
		previous_x = x;
		previous_y = y;
		save << i/873 << "," << x <<  "," << y << "," << difX << "," << difY << "\n";
	//}
}

string intToString(int number){
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void drawObject(int x, int y,Mat &frame){

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

    //UPDATE:JUNE 18TH, 2013
    //added 'if' and 'else' statements to prevent
    //memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);

	putText(frame,intToString(x)+","+intToString(y),Point(x,y+30),1,1,Scalar(0,255,0),2);

}
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);


	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);



}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &frame){
	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
					refArea = area;
				} else objectFound = false;
			}
			//let user know you found an object
			if(objectFound ==true){
				cont++;
				putText(frame,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
				//draw object location on screen
				drawObject(x,y,frame);
				
				// call the function to store the points into .txt
				saveData(x,y,cont);
				seePoints(x, y);

				// set the detected points into a white matrix

			}

		} else putText(frame,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
	}
}

int main(int argc, char* argv[]){
    bool trackObjects = true;
    bool useMorphOps = true;
	Mat frame;
	Mat HSV;
	Mat threshold;
	Mat feedback;

	int x=0, y=0;
	
	cont = 0;

	VideoCapture capture("/home/barbosa/Documentos/Pesquisa/Maestro/Videos/Compasso2x4.mp4");
    if ( !capture.isOpened() ){
        cout << "Cannot open the video file. \n";
        return -1;
    }

    capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	save.open ("Compasso2x4-5d.csv");
	trajetoria = Mat::zeros( FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3 );
	
	/*for(int i=0; i<FRAME_WIDTH;i++){
		for(int j=0;j<FRAME_HEIGHT;j++){
		  trajetoria.at<uchar>(i,j)=255;
		}
	}*/


	while(1){
        if (!capture.read(frame)) {
            cout<<"\n Cannot read the video file. \n";
            break;
        }

		cvtColor(frame,HSV,COLOR_BGR2HSV);

		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);

		if(useMorphOps) morphOps(threshold);

		if(trackObjects)
			trackFilteredObject(x,y,threshold,frame);

		//show frames
		imshow(windowName2,threshold);
		imshow(windowName,frame);
		imshow("Trajetoria", trajetoria);
		waitKey(30);
	}
	save.close();

	return 0;
}