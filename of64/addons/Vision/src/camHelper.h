
#ifndef msc_platform_camHelper_h
#define msc_platform_camHelper_h

#define CHESSBOARD 0
#define CLICKRECTANGLE 1
#define LEDRECTANGLE 2

#include "iostream"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

class camHelper 
{
    private:
    Mat perspectiveMat;
    vector<Point2f> perspectivePoints;
    bool enoughPerspectivePoints;
    float ratio_w, ratio_h;
    float unit;
    
    public:
    
    VideoCapture camCapture;
    int camSource;
    
    void initCamera(int camSource);
    void updateCamera(Mat& outputMatrix);
    
    
    Mat cameraMatrix, distortionCoeff, map1, map2;
    
    bool loadUndistort(); 
    void initUndistort(Mat inputMatrix);
    bool calcUndistort(Mat inputMatrix);
    void doUndistort(Mat inputMatrix, Mat& outputMatrix);
    
    Mat undistortedCameraMat;
    Mat homographyMatrix;
    cv::Size destinationSize;
    
    void onMouse(int event, int x, int y, int flags);
    bool loadPerspective();
    void setPerspectiveRatio(float width, float height);
    bool calcPerspective(Mat inputMatrix, string winName, int calcType = CLICKRECTANGLE );
    void doPerspective(Mat inputMatrix, Mat& outputMatrix);
    
    camHelper()
    {
        // default parameters
        initCamera(0);
        ratio_w = 7;
        ratio_h = 14;
    }
    
    ~camHelper()
    {
        
    }
    
    
};


#endif
