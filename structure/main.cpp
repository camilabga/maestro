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
    //Trajectory trajectory("../Data_GWR/maestro3D-1.csv");
    Trajectory trajectory("../data/square.csv");
    
    weareable.setIP((char*)"10.6.3.111");
    weareable.start();
    trajectory.unnormalize(Point(FRAME_WIDTH/2, FRAME_HEIGHT/2));
    //trajectory.unnormalize(Point(300, 100));

    trajectory.saveMovement("../Samples_Data/sample02_square_01.csv");
    vision.record("../Samples/sample02_square_01.avi");

    while(1){
        vision.calculateTagCenter();
        vision.drawTrajectory(trajectory, trajectory.getCurrentPointId());
        if (vision.isTargetOn()) {
            trajectory.setNextPoint0(vision.getCenter());
            vision.drawError(vision.getCenter(), trajectory.getCurrentPoint());
            weareable.send(trajectory.getError(vision.getCenter()));

            trajectory.savePoint(vision.getCenter());
            vision.saveVideo();
        }

        vision.show();
        //vision.saveVideo();
    }

    trajectory.endSaving();

    return 0;
}
