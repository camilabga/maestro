#ifndef Trajectory_h
#define Trajectory_h

#include <sstream>
#include <string>
#include <iostream>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

#define CHANGE_POINT_STATUS_X 40
#define CHANGE_POINT_STATUS_Y 20

#define ELIPSE_X (2*CHANGE_POINT_STATUS_X*CHANGE_POINT_STATUS_X)
#define ELIPSE_Y (CHANGE_POINT_STATUS_Y*CHANGE_POINT_STATUS_Y)

using namespace std;
using namespace cv;

class Trajectory{
    private:
        vector<float> x,y;
        vector<Point>points,vel;
        vector<Point>movement;

        vector<int> min,delta;  // contem informação sobre a desnormalização, deve utilizar unnormalize2 e getPointsFromCSV2
        vector<float>velx,vely; // Vetor de velocidade relativo

        unsigned int current_point;

        char *to_send, *temp0, *temp1;
        ofstream save_points;


        unsigned index_s;

    public:
        Trajectory();
        ~Trajectory();
        Trajectory(string file);

        void getPointsFromCSV(string file);
        inline const Point getPoint(int index){return points[index];}
        inline const int getSize(){return points.size();}

        inline const Point getVel(int index){return vel[index];}

        void saveMovement(string file);
        void savePoint(Point pos);
        void endSaving();

        void unnormalize(Point norma);
        void unnormalize();

        void unnormalize2();
        void getPointsFromCSV2(string file);

        void normalize(Point norma);
        void normalize();

        char* getError(Point pos);

        void setNextPoint0(Point pos);
        void setNextPoint1(Point pos);
        inline unsigned int getCurrentPointId(){return current_point;}
        inline Point getCurrentPoint(){return points[current_point];}
};

#endif
