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
    
    // MOG2 implementation
    Mat frame, back, fore;
    BackgroundSubtractorMOG2 bg(100, 16, false);
    vector<vector<Point> > contours;

    // MOG implementation
//    Mat frame, foreground;
//    BackgroundSubtractorMOG mog; // MOG takes in a binary image...
//    vector<vector<Point> > contours;
//    
    int cmin = 800;
    int cmax = 2000;
    
    while(true&&undistorted&&persp)
    {
        Mat undistorted;;
        Mat processed;
        
        loopCamera(rawCameraMat, capture, false);
        undistort(rawCameraMat, undistorted, camMat, distortMat);
        warpPerspective(undistorted, processed, H, dstSize,  INTER_LINEAR+CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT, Scalar());

        // MOG2 implementation
        processed.copyTo(frame);
        bg.operator()(frame, fore,0.001);
        bg.getBackgroundImage(back);
        erode(fore, fore, Mat::ones(Size(4,4), CV_64F));
        dilate(fore, fore, Mat::ones(Size(4,4), CV_64F) ); 
        Mat tempContours;
        fore.copyTo(tempContours);
        findContours(tempContours, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        
                            
        // just calculating the arc length of contour
        double length = arcLength(contours[0], true);
        cout << "contour[0] length" << length << endl;
        
        // some interesting functions that could be of use:
        // cv::contourArea
        // cv::pointPolygonTest test if point in or out of contour
        // cv::matchShapes measure resemblance between two contours
        
        // applying a mask to show foreground
        Mat mask(fore.size(),CV_8U);
        processed.copyTo(fore,mask);
        
        imshow("foreground", fore);
        
        
        
        // MOG implementation
//        cvtColor(processed, processed, CV_RGB2HSV); // sometimes greater contrast in HSV space
//        processed.copyTo(frame);
//        mog(frame,foreground,0.01);
//        erode(foreground, foreground, Mat::ones(Size(4,4),CV_32F));
//        dilate(foreground, foreground, Mat::ones(Size(4,4),CV_32F));
//        bitwise_not(foreground, foreground); // just to flip
//        Mat image;
//        foreground.copyTo(image);
//        
//        findContours(image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//        
//        // FILTER OUT "BAD" CONTOURS
//        vector<vector<Point> >::iterator itc = contours.begin();
//        while(itc != contours.end())
//        {
//            if (itc->size() < cmin || itc->size() > cmax)
//            {
//                itc = contours.erase(itc);
//            }
//            else
//                ++itc;
//        }
//        
//        Mat result(image.size(), CV_8U, Scalar(255));
//        drawContours(result,  contours, -1, Scalar(0),2);
//        imshow("contours", result);
//        createTrackbar("cmin", "contours", &cmin, 100);
//        createTrackbar("cmax", "contours", &cmax, 6000);
//        imshow("gaussian", foreground); 
        
        
        
        
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