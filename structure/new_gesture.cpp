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
    char *argTest[4];
    argTest[1] = "live"; 
    argTest[2] = "camera.yml";
    argTest[3] = "0.025";
    std::cout<<argc<<"--"<<argv[0]<<"--"<<argv[1]<<"--"<<argv[2]<<"--"<<argv[3];

    Vision vision(argc, argv);
    Trajectory trajectory("../data/square.csv");
    
    trajectory.unnormalize(Point(FRAME_WIDTH/2, FRAME_HEIGHT/2));

    trajectory.saveMovement("../data/new_movement.csv");
    vision.record("../../Videos/random_test.avi");

    while(1){
        vision.calculateTagCenter();
        if (vision.isTargetOn()) {
            trajectory.savePoint(vision.getCenter());
            vision.saveVideo();
        }

        vision.show();
        vision.saveVideo();
    }

    trajectory.endSaving();
    return 0;
}
