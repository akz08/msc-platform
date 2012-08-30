#ifndef msc_platform_footDetect_h
#define msc_platform_footDetect_h


#include "iostream"
#include "opencv2/opencv.hpp"
#include "opencv2/legacy/legacy.hpp"

using namespace std;
using namespace cv;

class footDetect
{
    
private:
    Mat grabbedFrame;
    vector<vector<cv::Point> > contours;
    
    
public:
    
    double _mogLearningRate;
    Mat foreground;
    BackgroundSubtractorMOG2 mog;
    
    void initMOG(int history = 1000, float varThreshold = 128, double defaultLearningRate = 0.01);
    void updateMOG(Mat inputMatrix);
    void setLearningRate(double inputLearningRate);
    void getBackground(Mat& outputMatrix);
    void getForeground(Mat inputMatrix, Mat& outputMatrix);

    void grabForeground();
    float activeAreaPc;
    Mat footFrame;
    void findFeet(Mat inputMatrix, Mat& outputMatrix);
    
    int minFootSize; // be generous with these
    int maxFootSize;
    
    void kalmanFilter(Mat inputMatrix, VideoCapture capture);
    
    
    MatND getHueHistogram(Mat inputMatrix, Mat checkMat);
    void initGetFeet();
    void getFeet();
    
    footDetect()
    {
        activeAreaPc = 0.7f;
        minFootSize = 100;
        maxFootSize = 3000;
    }
    
    ~footDetect()
    {
        
    } 
    
};

#endif
