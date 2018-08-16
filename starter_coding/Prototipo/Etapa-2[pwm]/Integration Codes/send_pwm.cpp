#include <sstream>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <cv.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <curl/curl.h>
#include <ctime>

#include <thread>

using namespace cv;
using namespace std;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 169;
int H_MAX = 193;
int S_MIN = 184;
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

#define CHANGE_POINT_STATUS_X 35
#define CHANGE_POINT_STATUS_Y 15
#define INTERVAL_SEC 0.2

double start = clock()/ static_cast<double>( CLOCKS_PER_SEC );

bool referenced = false;
int x_ref, y_ref;

unsigned cont, estado;
ofstream save;
Mat trajetoria;

vector <float> maestroX, maestroY;
vector <int> goalX, goalY;
int pwm[4];

// char *to_send, *temp0, *temp1, *temp2, *temp3;

// int get(char *url){

//     CURL *curl;
//     CURLcode res;

//     curl = curl_easy_init(); //Inicia uma sessão libcurl no modo fácil
//     if (curl){ //Caso retorne null, não será possivel usar outras funções da libcurl

//         curl_easy_setopt(curl, CURLOPT_URL, url); //Define as opções para o setup fácil

//         res = curl_easy_perform(curl);
//         /* Check for errors */
//         if (res != CURLE_OK){
//             fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
//         }
//         /* always cleanup */
//         curl_easy_cleanup(curl);
//     }

//     return 0;
// }

float map1(float x, float x1, float x2, float y1, float y2){
	return ((x - x1) * (y2 - y1) / (x2 - x1) + y1);
}

// void watch(){
// 	double timeSec = (clock()) / static_cast<double>( CLOCKS_PER_SEC );
// 	if(timeSec-start > INTERVAL_SEC){
// 		start = clock()/ static_cast<double>( CLOCKS_PER_SEC );
// 		cerr<<"Time: "<<timeSec<<endl;
// 		get(to_send);
// 	}
// }

void setNext(int x, int y, Mat &img){
	circle(img, Point(goalX[estado], goalY[estado]), 2, Scalar(0, 0, 255), 8, 8, 0);

	arrowedLine(img, Point(x,y), Point(goalX[estado], goalY[estado]), Scalar(0, 0, 255), 5, 8, 0, 0.5);

	if (((abs(goalX[estado] - x) < CHANGE_POINT_STATUS_X && abs(goalY[estado] - y) < CHANGE_POINT_STATUS_Y) && goalX.size() > estado+1) ||
				((abs(goalX[estado] - x)) >= (abs(goalX[estado+1] - x)))) {
		cout << estado << endl;
		circle(img, Point(goalX[estado], goalY[estado]), 2, Scalar(255, 0, 0), 8, 8, 0);
		estado++;
	} else if (goalX.size() == estado+1){
		estado = 0;
	}

	if (goalX[estado] - x > 0) {
		pwm[3] = 0;
		pwm[2] = abs(goalX[estado] - x);
	} else {
		pwm[2] = 0;
		pwm[3] = abs(goalX[estado] - x);
	}

	if (goalY[estado] - y > 0) {
		pwm[0] = 0;
		pwm[1] = goalY[estado] - y;
	} else {
		pwm[1] = 0;
		pwm[0] = y - goalY[estado];
	}
	
	// cout << "PWM0 = " << pwm[0] << " PWM1 = " << pwm[1] << " PWM2 = " << pwm[2] << " PWM3 = " << pwm[3] << endl;

	// pwm[0] = map1(pwm[0],0,240,800,0);//cima
	// pwm[1] = map1(pwm[1],0,240,800,0);//baixo
	// pwm[2] = map1(pwm[2],0,300,800,0);//esquerda
	// pwm[3] = map1(pwm[3],0,300,800,0);//direita

	
	// sprintf(to_send,"%s","192.168.0.3/");
	// sprintf(temp0,"%d",pwm[1]);
	// strcat(to_send, temp0);
	// strcat(to_send, "-");
	// sprintf(temp1,"%d",pwm[0]);
	// strcat(to_send, temp1);
	// strcat(to_send, "-");
	// sprintf(temp2,"%d",pwm[3]);
	// strcat(to_send, temp2);
	// strcat(to_send, "-");
	// sprintf(temp3,"%d",pwm[2]);
	// strcat(to_send, temp3);
	
    // cout << to_send << endl;

	// watch();
	// thread send(watch);
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

		// rectangle(frame, Point(goalX[i]-CHANGE_POINT_STATUS_X, goalY[i]-CHANGE_POINT_STATUS_Y), 
		// 					Point(goalX[i]+CHANGE_POINT_STATUS_X, goalY[i]+CHANGE_POINT_STATUS_Y), Scalar(0, 255, 0), 1, 8, 0);
		
		ellipse(frame, Point(goalX[i], goalY[i] ), Size( CHANGE_POINT_STATUS_X, CHANGE_POINT_STATUS_Y ), 0, 0, 360, Scalar( 0, 255, 0 ), 1, 8 );

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

	// to_send = (char*) malloc(sizeof(char) * 1024);
	// temp0 = (char*) malloc(sizeof(char) * 1024);
    // temp1 = (char*) malloc(sizeof(char) * 1024); 
    // temp2 = (char*) malloc(sizeof(char) * 1024);
    // temp3 = (char*) malloc(sizeof(char) * 1024);
	
	int x=0, y=0;
	
	cont = 0;

	estado = 0;

	getMaestroPontos();
	
	VideoCapture capture(0);
	//VideoCapture capture("/home/barbosa/Documentos/Pesquisa/Maestro/Videos/IMG_3260.mp4");
	
	if ( !capture.isOpened() ){
      cout << "Cannot open the video file" << endl;
      return -1;
    }

    capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	while(1){
		if (!capture.read(frame)) {
			cout<<"\n Cannot read the video file. \n";
			break;
		}

		while(!referenced){
			if (!capture.read(frame)) {
				cout<<"\n Cannot read the video file. \n";
				break;
			}

			flip(frame, frame, 1);
			imshow(windowName,frame);
			setMouseCallback(windowName, CallBackFunc, NULL);

			waitKey(1);
		}


		showTrajetory(frame);

		cvtColor(frame,HSV,COLOR_BGR2HSV);

		inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);

		if(useMorphOps) morphOps(threshold);

		if(trackObjects)
			trackFilteredObject(x,y,threshold,frame);
			// thread pdi(trackFilteredObject, x,y,threshold,frame);

		//show frames
		flip(frame, frame, 1);
		imshow(windowName2,threshold);
		imshow(windowName,frame);
		waitKey(30);
	}

	return 0;
}