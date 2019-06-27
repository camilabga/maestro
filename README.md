# Repository made to support the project Maestro

## Introduction
The project consists in a system designed to teach and correct the movement of a blind student that aims to learn a movement. With a OpenCV algorithm and a wearable, we made it possible to these students have a better perception to wheather they are making the gesture correctly or not.

The correction of the movement is passed to the user with an haptic feedback in real time while he/she is executing the gesture.

This work proposes a human gesture evaluation with visual detection and haptic feedback as an additional tool. The purpose is to utilize gesture monitoring with a visual detection alongside markers to execute a gesture following and then, send a haptic feedback to the user.

The project evolved to a different direction. To filter the gesture, we use a machine learning algorithm. At first, a SOM neural network was tested, but then the problem fitted better with a network called [GWR](https://www.sciencedirect.com/science/article/pii/S0893608002000783).

## Installing the requisites
* OpenCV 3.4.x [Installation Guide](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html)
* ArUco 3.0.10 [Installation Guide](http://maztories.blogspot.com/2013/07/installing-aruco-augmented-reality.html)

This project was initially executed on a Linux environment with the distribution Ubuntu 16.04. The OpenCV version was 3.4.1 and the ArUco version was 3.0.10. The machine had an Intel Core i5 7th generation and 8GB of RAM Memory.

## Configuring the Wearable

1. Regular motors activation
2. Alternated motors activation

### Regular motors activation
to do

### Alternated motors activation
to do 

## Running the code
The main code is in the ../structure directory. To execute it you must, first choose the mode of execution by editing the **main.cpp** file and the project that contains the NN algorithm (GWR).

It has 4 modes:
1. Recording a new Gesture
2. Running the data through the GWR
3. Running the correction algorithm without the Wearable
4. Running the correction algorithm with the Wearable

### Recording a new Gesture
Open the file **main.cpp** and edit it to look like this:

```c++
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
```

### Running the data through the GWR
to do

### Running the correction algorithm without the Wearable
Open the file **main.cpp** and edit it to look like this:

```c++
    Vision vision(argc, argv);
    Trajectory trajectory("../data/square.csv");
    trajectory.unnormalize(Point(FRAME_WIDTH/2, FRAME_HEIGHT/2));
    trajectory.saveMovement("../data/random_test.csv");

    while(1){
        vision.calculateTagCenter();
        vision.drawTrajectory(trajectory, trajectory.getCurrentPointId());
        if (vision.isTargetOn()) {
            trajectory.setNextPoint0(vision.getCenter());
            vision.drawError(vision.getCenter(), trajectory.getCurrentPoint());
            trajectory.savePoint(vision.getCenter());
        }

        vision.show();
    }

    trajectory.endSaving();
```

### Running the correction algorithm with the Wearable
Open the file **main.cpp** and edit it to look like this:

```c++
    Vision vision(argc, argv);
    Weareable weareable;
    Trajectory trajectory("../data/square.csv");

    trajectory.unnormalize(Point(FRAME_WIDTH/2, FRAME_HEIGHT/2));
    trajectory.saveMovement("../data/random_test.csv");

    weareable.setIP((char*)"10.6.4.107");
    weareable.start();

    while(1){
        vision.calculateTagCenter();
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
```

## Article(s) Published
### Human Gesture Evaluation with Visual Detection and Haptic Feedback
**Abstract:** Learning a gesture pertains to learning a expression of motion by a human, involving the hands, arms, face, head, and/or body. In this work, we propose to employ haptic feedback as an additional tool in the gesture following/evaluation loop. Accordingly, the user wears a haptic wearable device in the form of a bracelet which vibrates according to the trajectory error. Our research hypothesis is then to investigate whether such haptic device aids the user in correcting his movement in relation to the prerecorded trajectory.

**Acess in:** https://dl.acm.org/citation.cfm?id=3243104
