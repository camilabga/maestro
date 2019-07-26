#ifndef Vision_h
#define Vision_h

#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
#include <fstream>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Trajectory.h"

using namespace std;
using namespace cv;
using namespace aruco;

class CmdLineParser {
  int argc;
  char **argv;
 public:
  CmdLineParser(int _argc, char **_argv) : argc(_argc), argv(_argv) {}
  bool operator[](string param) {
    int idx = -1;
    for (int i = 0; i < argc && idx == -1; i++)
      if (string(argv[i]) == param) idx = i;
    return (idx != -1);
  }
  string operator()(string param, string defvalue = "-1") {
    int idx = -1;
    for (int i = 0; i < argc && idx == -1; i++)
      if (string(argv[i]) == param) idx = i;
    if (idx == -1)
      return defvalue;
    else
      return (argv[idx + 1]);
  }
};
struct TimerAvrg {
  std::vector<double> times;
  size_t curr = 0, n;
  std::chrono::high_resolution_clock::time_point begin, end;
  TimerAvrg(int _n = 30) {
    n = _n;
    times.reserve(n);
  }
  inline void start() { begin = std::chrono::high_resolution_clock::now(); }
  inline void stop() {
    end = std::chrono::high_resolution_clock::now();
    double duration =
        double(
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                .count()) *
        1e-6;
    if (times.size() < n)
      times.push_back(duration);
    else {
      times[curr] = duration;
      curr++;
      if (curr >= times.size()) curr = 0;
    }
  }
  double getAvrg() {
    double sum = 0;
    for (auto t : times) sum += t;
    return sum / double(times.size());
  }
};

class Vision {
 private:
  MarkerDetector MDetector;
  VideoCapture TheVideoCapturer;
  vector<Marker> TheMarkers;
  Mat TheInputImage, TheInputImageGrey, TheInputImageCopy;
  CameraParameters TheCameraParameters;
  string dictionaryString;
  int iDetectMode = 0, iMinMarkerSize = 0, iCorrectionRate = 0,
      iShowAllCandidates = 0, iEnclosed = 0, iThreshold, iCornerMode,
      iDictionaryIndex = 0;
  int waitTime = 0;
  bool showMennu = false, bPrintHelp = false, isVideo = false;
  char key = 0;
  int index = 0, indexSave = 0;
  TimerAvrg Fps;
  float TheMarkerSize;
  float resizeFactor;

  Point center;
  bool target_on;
  VideoWriter video;

  void setParamsFromGlobalVariables();
  Mat resize(const cv::Mat &in, int width);
  void createMenu();
  void putText(cv::Mat &im, string text, cv::Point p, float size);
  void printHelp(cv::Mat &im);
  void printInfo(cv::Mat &im);
  void printMenuInfo();
  Mat resizeImage(cv::Mat &in, float resizeFactor);
  void cvTackBarEvents(int pos, void *);

 public:
  Vision();
  // Vision(int argc, char **argv);
  ~Vision();
  // bool start();
  inline const Point &getCenter() const { return center; }
  void calculateTagCenter();
  inline void saveVideo() { video.write(TheInputImage); }
  inline bool isTargetOn() { return target_on; }

  void drawTrajectory(Trajectory &T, int next);
  void drawError(Point pos, Point error);
  inline void show() {
    flip(TheInputImageCopy, TheInputImageCopy,
         1); /*imshow("", TheInputImageCopy);*/
  }
  inline Mat getInputImage() {
    flip(TheInputImageCopy, TheInputImageCopy, 1);
    return TheInputImageCopy;
  }
  inline void release() { TheVideoCapturer.release(); }
  void record(string filename);
};

#endif
