#ifndef Weareable_h
#define Weareable_h

#include <sstream>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include /*<curl/curl.h>*/ "/usr/include/curl/curl.h"
#include <ctime>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdexcept>

#include <thread>


#define BUFLEN 2048
#define SERVICE_PORT 8080

using namespace std;

class Weareable{
    private:
        char *ip_esp;
        int fd, i, slen;
        char buf[BUFLEN];

    public:
        Weareable();
        ~Weareable();

        void start();
        void send(char *to_send);
        inline void setIP(char *ip){ip_esp = ip;}

};

#endif
