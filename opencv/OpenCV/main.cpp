#include "main.h"
#include "cleaned.h"

// Global variables
VideoCapture capture;
Mat rawCameraMat, undistortedCameraMat;

int cameraSource = 0; // 0 being default
Mat camMat, distortMat;
Mat nextImage(VideoCapture capture);
// temporary function to get things off the ground
Mat nextImage(VideoCapture capture){
    Mat result;
    Mat view0;
    capture >> view0;
    view0.copyTo(result);
    return result;
}

int h_lo = 0, h_up = 20, s_lo = 100, s_up = 255, v_lo = 0, v_up = 255;


int main()
{
    
    capture = initCamera(cameraSource);
    bool undistorted = initUndistort(rawCameraMat, capture, true, camMat, distortMat);
    capture >> rawCameraMat;
    undistort(rawCameraMat,undistortedCameraMat, camMat, distortMat);
    Mat H;
    bool persp = false;
    persp = initPerspective(undistortedCameraMat, false, 0, H);
    
    // we now have a 'undistortedMat' with no LED lit
    
    while(true&&undistorted)
    {
        Mat processed;

        capture>>rawCameraMat;
        undistort(rawCameraMat, processed, camMat, distortMat);

        Mat tester = processed - undistortedCameraMat;
        
//        Mat output;
//        warpPerspective(processed, output, H, processed.size(),  INTER_LINEAR, BORDER_CONSTANT, Scalar());
        Mat hsv;
        cvtColor(processed, hsv, CV_BGR2RGB);
        
        inRange(hsv, Scalar((double)h_lo,(double)s_lo,(double)h_lo), Scalar((double)h_up,(double)s_up,(double)v_up), processed);

        imshow("undistorted", processed);
        
        createTrackbar("H Low thresh", "undistorted", &h_lo, 360,0);
        createTrackbar("H Hi thresh", "undistorted", &h_up, 360,0);
        
        createTrackbar("S Low thresh", "undistorted", &s_lo, 255,0);
        createTrackbar("S Hi thresh", "undistorted", &s_up, 255,0);
//        
        createTrackbar("V Low thresh", "undistorted", &v_lo, 255,0);
        createTrackbar("V Hi thresh", "undistorted", &v_up, 255,0);
        

        imshow("tester",tester);
        
        char key =  waitKey(100);
        if( key  == 27 ) // 27 == ESC
            break;

    }
    
//    capture = initCamera(cameraSource);
//    
//    while(true)
//    {
//        Mat view;
//        loopCamera(view, capture, false);
//        imshow("test", view);
//    }
    
    return 0;
}