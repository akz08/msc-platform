#include "main.h"

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
    Size dstSize;
    
    capture = initCamera(cameraSource);
    capture >> rawCameraMat;
    undistorted = initUndistort(true, rawCameraMat, capture, camMat, distortMat);
    
    if(undistorted)
    {
        undistort(rawCameraMat,undistortedCameraMat, camMat, distortMat);
        persp = initPerspective(true, undistortedCameraMat, CLICKRECTANGLE, H, dstSize);
    }    
    
//    int i = 0;
    
    // trying to perform a gaussian bg-fg segmentation
//    Mat frame, back, fore;
//    BackgroundSubtractorMOG2 bg(100, 6, false);
//    vector<vector<Point> > contours;

    Mat frame, foreground;
    BackgroundSubtractorMOG mog;
    
    while(true&&undistorted&&persp)
    {
        Mat undistorted;;
        Mat processed;
        
        loopCamera(rawCameraMat, capture, false);
        undistort(rawCameraMat, undistorted, camMat, distortMat);
        warpPerspective(undistorted, processed, H, dstSize,  INTER_LINEAR+CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT, Scalar());

//        processed.copyTo(frame);
//        bg.operator()(frame, fore);
//        bg.getBackgroundImage(back);
//        erode(fore, fore, Mat::ones(Size(4,4), CV_64F));
//        dilate(fore, fore, Mat::ones(Size(4,4), CV_64F) );   
//        findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//        drawContours(processed, contours, -1, Scalar(255,0,0));
//        imshow("background", back);
//        imshow("foreground", fore);
        
        cvtColor(processed, processed, CV_RGB2HSV);
        processed.copyTo(frame);
        mog(frame,foreground,0.001);
        erode(foreground, foreground, Mat::ones(Size(4,4),CV_32F));
        dilate(foreground, foreground, Mat::ones(Size(4,4),CV_32F));
//        threshold(foreground, foreground, 155, 255, THRESH_BINARY_INV); // just to complement the image... can just do a NOT
        bitwise_not(foreground, foreground);
        imshow("gaussian", foreground);
        
        
        
//        cvtColor(processed, processed, CV_BGR2GRAY);

        
        
//        if(i==0){
//        processed.copyTo(bg); 
//            i++;
//        }

        
        
        
        imshow("output", processed);
        
//        createTrackbar("canny threshold", "output", &lowThreshold, 100);
        
        char key =  waitKey(100);
        if( key  == 27 ) // 27 == ESC
            break;

    }
    
    return 0;
}