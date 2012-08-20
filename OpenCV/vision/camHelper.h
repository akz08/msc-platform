
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
    void perspMouse(int event, int x, int y, int, void* param)
    {
        switch (event) {
            case CV_EVENT_LBUTTONDOWN:
            {
                cout << "detected a left mouse click at: " << x << ", " << y  << endl;
                
                if(perspectivePoints.size() < 4)
                {
                    circle(perspectiveMat, Point(x,y), 10, Scalar(0,0,0), 3, 8);
                    perspectivePoints.push_back(Point(x,y));
                    cout << "saved the point: " << x << ", " << y << endl;
                }
                else {
                    enoughPerspectivePoints = true;
                }
                break;
            }
                
            default:
                break;
        }
    }
     
    
    public:
    
    VideoCapture camCapture;
    int camSource;
    
    void initCamera(int camSource);
    void updateCamera(Mat& outputMatrix);
    
    
    Mat cameraMatrix, distortionCoeff, map1, map2;
    
    bool loadUndistort(); 
    void initUndistort(Mat inputMatrix);
    bool calcUndistort(Mat inputMatrix, Mat& outputMatrix);
    void doUndistort(Mat inputMatrix, Mat& outputMatrix);
    
    Mat undistortedCameraMat;
    Mat homographyMatrix;
    Size destinationSize;
    
    bool loadPerspective();
    bool calcPerspective(int calcType);
    void doPerspective(Mat inputMatrix, Mat& outputMatrix);
    
    float ratio_w, ratio_h;
    
    camHelper()
    {
        initCamera(0);

    }
    
    ~camHelper()
    {
        
    }
    
    
};


#endif
