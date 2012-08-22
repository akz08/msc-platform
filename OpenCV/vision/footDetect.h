#ifndef msc_platform_footDetect_h
#define msc_platform_footDetect_h


#include "iostream"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class footDetect
{
    
private:
    Mat grabbedImage;
    
    
    
public:
    
    double _mogLearningRate;
    Mat foreground;
    BackgroundSubtractorMOG2 mog;
    
    void initMOG(int history = 1000, float varThreshold = 128, double defaultLearningRate = 0.01);
    void updateMOG(Mat inputMatrix);
    void setLearningRate(double inputLearningRate);
    void getBackground(Mat& outputMatrix);
    void getForeground(Mat inputMatrix, Mat& outputMatrix);

    bool grabForeground();
    void findFeet();
    
    
    
    
    
    
    void initGetFeet();
    void getFeet();
    
    footDetect()
    {
        
    }
    
    ~footDetect()
    {
        
    } 
    
};

#endif
