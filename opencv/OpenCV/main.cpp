#include "main.h"
#include "cleaned.h"

// Global variables
VideoCapture capture;
Mat rawCameraMat, undistortedCameraMat;

int cameraSource = 0; // 0 being default
Mat camMat, distortMat;

int main()
{
    bool undistorted;
    bool persp;
    Mat H; // homography matrix
    
    capture = initCamera(cameraSource);
    capture >> rawCameraMat;
    undistorted = initUndistort(false, rawCameraMat, capture, camMat, distortMat);
    
    if(undistorted)
    {
        undistort(rawCameraMat,undistortedCameraMat, camMat, distortMat);
        persp = initPerspective(false, undistortedCameraMat, CLICKRECTANGLE, H);
    }    
    
    while(true&&undistorted&&persp)
    {
        Mat undistorted;;
        Mat processed;
        
        loopCamera(rawCameraMat, capture, false);
        undistort(rawCameraMat, undistorted, camMat, distortMat);
        warpPerspective(undistorted, processed, H, undistorted.size(),  INTER_LINEAR, BORDER_CONSTANT, Scalar());

        imshow("output", processed);

        
        char key =  waitKey(100);
        if( key  == 27 ) // 27 == ESC
            break;

    }
    
    return 0;
}