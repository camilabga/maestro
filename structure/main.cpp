#include "Vision.h"
#include "Weareable.h"
#include "Trajectory.h"

using namespace std; 
using namespace cv;
using namespace aruco;

/* 
00 sem pulseira
01 amostragem
02 GWR
*/ 

int main (int argc, char **argv) {
    
    Vision vision(argc, argv);
    Weareable weareable;
    Trajectory trajectory("../data/square.csv");

    trajectory.unnormalize(Point(FRAME_WIDTH/2, FRAME_HEIGHT/2));
    trajectory.saveMovement("../data/random_test.csv");

    weareable.setIP((char*)"192.168.43.236");
    weareable.start();

    while(1){
        vision.calculateTagCenter();
        //vision.circleTracker();
        
        vision.drawTrajectory(trajectory, trajectory.getCurrentPointId());
        if (vision.isTargetOn()) {
            trajectory.setNextPoint0(vision.getCenter());
            vision.drawError(vision.getCenter(), trajectory.getCurrentPoint());

            weareable.send(trajectory.getError(vision.getCenter()));

            trajectory.savePoint(vision.getCenter());
        }

        vision.show();
    }

    trajectory.endSaving();
}
