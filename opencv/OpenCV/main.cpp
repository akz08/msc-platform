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
    
    // need to first init bgsubtractor
    BackgroundSubtractorMOG2 mog2(100, 16, false);
    
    Mat bgSegmentation, foreground, background;
    double mogLearningRate = 0.001;
    
    int minContourSize = 800;
    int maxContourSize = 2000;
    
    while(true&&undistorted&&persp)
    {
        // INITIAL CAMERA PREPROCESSING
        Mat undistorted;
        Mat preProcessing;
        
        loopCamera(rawCameraMat, capture, false);
        undistort(rawCameraMat, undistorted, camMat, distortMat);
        warpPerspective(undistorted, preProcessing, H, dstSize,  INTER_LINEAR+CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT, Scalar());

        
        imshow("Camera Pre-processing", preProcessing);
        
        // SEGMENTATION
        
        preProcessing.copyTo(bgSegmentation);
        //
        mog2.operator()(bgSegmentation, foreground,mogLearningRate);
        mog2.getBackgroundImage(background);
        imshow("Foreground Mask", foreground);
        imshow("Background", background);
        
        // clean up a bit
//        morphologyEx(foreground, foreground, MORPH_ERODE, Mat(), Point(-1,-1), 5);
        morphologyEx(foreground, foreground, MORPH_OPEN, Mat()); // erode~dilate
        
        // contour stuff
        Mat fgContours;
        foreground.copyTo(fgContours);
        vector<vector<Point> > contours;
        
        findContours(fgContours, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        
        // FILTER OUT "BAD" CONTOURS // EXTRACTING MAXIMUM 2 LIKELY AREAS OF FOOT PRESENCE 
        // how to decide what is too big/ too small? people have different sized feet
        
        
        // also calculate the distance from the two "feet"
        
        vector<vector<Point> >::iterator iterContours = contours.begin();
        while(iterContours != contours.end())
        {
            if (iterContours->size() < minContourSize || iterContours->size() > maxContourSize)
            {
                iterContours = contours.erase(iterContours);
            }
            else
                ++iterContours;
        }
        
//        Mat result(fgContours.size(), CV_8U, Scalar(255));
        
        Mat contourMask = Mat::zeros(preProcessing.size(), CV_8UC1);
        
        drawContours(contourMask,  contours, -1, Scalar(100),CV_FILLED);
        imshow("Contours", contourMask);
        createTrackbar("cmin", "Contours", &minContourSize, 100);
        createTrackbar("cmax", "Contours", &maxContourSize, 6000);
        
        // contours masked... or sth
        Mat toHist(preProcessing.size(), CV_8UC3);
        preProcessing.copyTo(toHist, contourMask);
        
        imshow("test", toHist);
        
        // toHist is now ready for histogram back projection 
        
        
        
        char key =  waitKey(100);
        if( key  == 27 ) // 27 == ESC
            break;

    }
    
    return 0;
}

//        createTrackbar("canny threshold", "output", &lowThreshold, 100);