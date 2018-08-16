// click in a point and get its coordinate

#include <sstream>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


using namespace cv;
using namespace std;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 168;
int H_MAX = 179;
int S_MIN = 162;
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

#define CHANGE_POINT_STATUS_X 15
#define CHANGE_POINT_STATUS_Y 5

bool referenced = false;
int x_ref, y_ref;

unsigned cont, estado;
ofstream save;
Mat trajetoria;

vector <float> maestroX, maestroY;
vector <int> goalX, goalY;
int pwm[4];

float map1(float x, float x1, float x2, float y1, float y2){
	if (x < x1) {
		return 0;
	} else if (x > x2) {
		return y2;
	}

	return ((x - x1) * (y2 - y1) / (x2 - x1) + y1);
}

void setNext(int x, int y, Mat &img){
	circle(img, Point(goalX[estado], goalY[estado]), 2, Scalar(0, 0, 255), 8, 8, 0);

	arrowedLine(img, Point(x,y), Point(goalX[estado], goalY[estado]), Scalar(0, 0, 255), 5, 8, 0, 0.5);

	if ((abs(goalX[estado] - x)) >= (abs(goalX[estado+1] - x))) {
		estado++;
		circle(img, Point(goalX[estado-1], goalY[estado-1]), 2, Scalar(255, 0, 0), 8, 8, 0);
	}

	if ((abs(goalX[estado] - x) < CHANGE_POINT_STATUS_X && abs(goalY[estado] - y) < CHANGE_POINT_STATUS_Y) && goalX.size() > estado+1) {
		cout << estado << endl;
		circle(img, Point(goalX[estado], goalY[estado]), 2, Scalar(255, 0, 0), 8, 8, 0);
		estado++;
	} else if (goalX.size() == estado+1){
		estado = 0;
	}
}

void getMaestroPontos(){
	ifstream myfile("/home/barbosa/Documentos/Pesquisa/Maestro/getDataFiles/NORM_4020.txt");
	float temp1, temp2;
	int g;
	maestroX.clear();
	maestroY.clear();
	if (myfile.is_open()){
		while (myfile >> g >> temp1 >> temp2) {
			maestroX.push_back(temp1);
			maestroY.push_back(temp2);
		}

		myfile.close();
	}
}

void showTrajetory(Mat &frame){
	goalX.clear();
	goalY.clear();
	/*for(vector<float>::iterator it = maestroX.begin(); it != maestroX.end(); ++it) {
		goalX.push_back( (int)(x_ref + x_ref* (*it) ) );
	}

	for(vector<float>::iterator it = maestroY.begin(); it != maestroY.end(); ++it) {
		goalY.push_back( (int)(FRAME_HEIGHT-(y_ref* (*it) )) );
	}*/

	for (vector<float>::size_type i = 0; i < maestroX.size(); i++){
		goalX.push_back( (int)(x_ref + x_ref* maestroX[i] ) );
		goalY.push_back( (int)(FRAME_HEIGHT-(maestroY[i] * (FRAME_HEIGHT-y_ref)) ));

		if (estado == i) {
			circle(frame, Point(goalX[i], goalY[i]), 2, Scalar(0, 0, 255), 8, 8, 0);
		} else if (estado > i) {
			circle(frame, Point(goalX[i], goalY[i]), 2, Scalar(255, 0, 0), 8, 8, 0);
		} else {
			circle(frame, Point(goalX[i], goalY[i]), 2, Scalar(255, 255, 255), 8, 8, 0);
		}
			
	}
	
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata){
     if  ( event == EVENT_LBUTTONDOWN && !referenced){
		  x_ref = x;
		  y_ref = y;
		  cout << x_ref << " " << y_ref << endl;
		  referenced = true;
     }
}

void seePoints(int x, int y){
	circle(trajetoria, Point(x,y), 2, Scalar(255, 255, 255), 1, 8, 0);
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
				//draw object location on screen
				
				
				// call the function to store the points into .txt

				setNext(x, y, frame);

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

	estado = 0;

	getMaestroPontos();

	// VideoCapture capture(0);
	VideoCapture capture("/home/barbosa/Documentos/Pesquisa/Maestro/Videos/Compasso2x4.mp4");
	
	if ( !capture.isOpened() ){
      cout << "Cannot open the video file" << endl;
      return -1;
    }

    capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	while(1){
		capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
		if (!capture.read(frame)) {
			cout<<"\n Cannot read the video file. \n";
			break;
		}

		while(!referenced){
			if (!capture.read(frame)) {
				cout<<"\n Cannot read the video file. \n";
				break;
			}

			setMouseCallback(windowName, CallBackFunc, NULL);
			flip(frame, frame, 1);
			imshow(windowName,frame);
			waitKey(1);
		}


		showTrajetory(frame);

		cvtColor(frame,HSV,COLOR_BGR2HSV);

		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);

		if(useMorphOps) morphOps(threshold);

		if(trackObjects)
			trackFilteredObject(x,y,threshold,frame);

		//show frames
		flip(frame, frame, 1);
		imshow(windowName2,threshold);
		imshow(windowName,frame);
		waitKey(30);
	}

	return 0;
}