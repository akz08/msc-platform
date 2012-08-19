//
//  main.h
//  msc-platform
//


#ifndef msc_platform_main_h
#define msc_platform_main_h

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cvblob.h>
#include <math.h>
#include <time.h>
#include "ioPhidget.h"
#include "opencv2/highgui/highgui.hpp"



using namespace std;
using namespace cv;
using namespace cvb;


/* the code has been restructured to have clearly sectioned out code for both photo trials for testing, and the main foot sensing algorithms */


#pragma mark Camera Management
// INITIALISE CAMERA SOURCE //
// set the default camera source
// initialise VideoCapture
// check if valid camera source
VideoCapture initCamera(int camSource);
void loopCamera(Mat& rawCameraMat, VideoCapture capture, bool scaleDown);

VideoCapture initCamera(int camSource)
{
    // initialise VideoCapture for use
    VideoCapture capture(camSource);
    if( !capture.isOpened() )
    {
        cout << "Invalid source chosen!" << endl;
        return -1;
    }
    return capture;
}

void loopCamera(Mat& rawCameraMat, VideoCapture capture, bool scaleDown)
{
    // loop camera read for main while loop
    if( !capture.isOpened() )
    {
        cout << "Invalid source chosen!" << endl;
        return;
    }
    capture >> rawCameraMat;
    if(scaleDown)
        pyrDown(rawCameraMat, rawCameraMat,Size(rawCameraMat.cols/2,rawCameraMat.rows/2));
}

#pragma mark Undistortion
// UNDISTORTION STEP //
// option to enable step (bool)
// load distortion matrices by default
// if none, initialise undistortion function
// upon completion of undistortion - save/override distortion files

bool initUndistort(bool loadExisting, Mat rawCameraMat, VideoCapture capture, Mat& camMat, Mat& distortMat );

bool initUndistort(bool loadExisting, Mat rawCameraMat, VideoCapture capture, Mat& camMat, Mat& distortMat)
{
    // to be called before the main while loop
    // has internal while loop for the calibration process
    // the while loop breaks on condition that calibration completes successfully 
    // (probably need to code for possible errors)
    if(loadExisting)
    {
        FileStorage fs;
        fs.open("camParameters/distortion.xml", FileStorage::READ);
        if(!fs.isOpened()) // if NOT opened
        {
            cerr << "Failed to open distortion.xml" << endl;
            return false;
        }
        fs["camMat"] >> camMat;
        fs["distortMat"] >> distortMat;
        return true;
    }
    else // we do the calibration. 2-part process: init of vars; while loop
    {
        vector<vector<Point2f> > imagePoints; // captured images from camera
        vector<Point2f> corners; // store corners from findChessboardCorners
        Size boardSize = Size(5, 8); // size of board (width x height)
        float squareSize = 40.0f; // size of individual squares
        int numSamples = 20; // number of samples for calibration
        clock_t prevTimestamp = 0; // used to calculate delay btwn image captures
        int delay = 100; // delay between capturing frames for samples
        
        bool foundChessboard; // chessboard found bool
        bool calibrating = true; // while loop condition
        
        
        // end of init. start the while loop around here
        while(calibrating)
        {
            Mat calibCameraMat;     // an intermediate between capturing & processing
            capture >> rawCameraMat;
            rawCameraMat.copyTo(calibCameraMat);
            cout << "calibrating loop" << endl;
            bool blinkOutput = false;   // 'blink' output to show capture
            
            // keep trying to find the chessboard
            foundChessboard = findChessboardCorners(calibCameraMat, boardSize, corners,
                                                    CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
            // if chessboard found, we get the corners, and "capture" the information every other second into a sample to be used for calibration
            if(foundChessboard)
            {
                Mat greyCameraMat;
                cout << "starting the 'found' loop" <<endl;
                // get subpixel accuracy on corners
                cvtColor(calibCameraMat, greyCameraMat, CV_BGR2GRAY);
                cornerSubPix(greyCameraMat, corners, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
                
                // now we take new samples from the camera after a DELAY TIME...
                if(!capture.isOpened() || clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC)
                {
                    imagePoints.push_back(corners); // add the current frame's corners
                    prevTimestamp = clock();        // ref time for 'last capture'
                    blinkOutput = capture.isOpened();
                    
                }
                
                // draw chessboard corners
                drawChessboardCorners(calibCameraMat, boardSize, Mat(corners), foundChessboard);
                cout << "drew chessboard corners" << endl;
            }
            
            // inverting the image to show image was captured
            if(blinkOutput)
                bitwise_not(calibCameraMat, calibCameraMat);
            string msg = format("%d/%d", (int)imagePoints.size(), numSamples);
            putText(calibCameraMat, msg, Point(5,15), 1, 1,Scalar(0,255,0));
            
            imshow("Distortion", calibCameraMat);
            
            // if we have a sufficient sample of images & corresp. points, we do the calibration
            if( imagePoints.size() >= numSamples )
            {
                cout << "we have enough samples" << endl;
                camMat = Mat(3, 3, CV_64F);
                distortMat = Mat::zeros(8, 1, CV_64F);
                
                vector<vector<Point3f> > objectPoints(1);
                
                for(int i = 0; i < boardSize.height; ++i)
                    for(int j = 0; j < boardSize.width; ++j)
                        objectPoints[0].push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
                
                objectPoints.resize(imagePoints.size(),objectPoints[0]); 
                
                // find camera params
                vector<Mat> rvecs, tvecs; // rotation & translation vectors
                vector<float> reprojErrs; // reprojection erros
                
                double rms = calibrateCamera(objectPoints, imagePoints, calibCameraMat.size(), camMat, distortMat, rvecs, tvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
                
                cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;
                // we COULD compute the total error, but there's no need here
                
                // now we should save the camera parameters...and end the initial loop
                
                // saving...
                FileStorage fs("camParameters/distortion.xml", FileStorage::WRITE);
                fs << "camMat" << camMat;
                fs << "distortMat" << distortMat;
                fs.release();
                
                calibrating = false;    // end the calibration loop
                destroyWindow("Distortion");
                return true;
            }
            
            // stop on ESC key
            char key =  waitKey(100);
            if( key  == 27 ){
                return false;
            }
            
        }
    }
    return false; 
}

#pragma mark Perspective
// PERSPECTIVE CORRECTION STEP //
// option to enable step (bool)
// option to 
// choose between chessboard calibration or user-clicked points

// initialising a public Mat to be used for drawing on for initPerspective
Mat perspectiveMat;
vector<Point2f> perspectivePoints;
bool enoughPerspectivePoints;

// initialising "onMouse" function
void perspMouse(int event, int x, int y, int, void* param);

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

#define CHESSBOARD 0
#define CLICKRECTANGLE 1
#define LEDRECTANGLE 2

bool initPerspective(bool loadExisting, Mat undistortedCameraMat, int calcType, Mat& hmgMat, Size& dstSize, float ratio_w = 7, float ratio_h = 14);

bool initPerspective(bool loadExisting, Mat undistortedCameraMat, int calcType, Mat& hmgMat, Size& dstSize, float ratio_w, float ratio_h)
{
    // note optional parameters ratio_w & ratio_h used as the aspect ratio when choosing calcType CLICKRECTANGLE
    
    
    // want to have to methods of calculating perspective:
    // by chessboard, or by detecting (4) led points to form src points
    if(loadExisting)
    {
        FileStorage fs;
        fs.open("camParameters/perspective.xml", FileStorage::READ);
        if(!fs.isOpened()) // if NOT opened
        {
            cerr << "Failed to open perspective.xml" << endl;
            return false;
        }
        fs["hmgMat"] >> hmgMat;
        
        float dstWidth, dstHeight;
        fs["dstWidth"] >> dstWidth;
        fs["dstHeight"] >> dstHeight;
        dstSize = Size(dstWidth, dstHeight);
        return true;
    }
    else
    {
        // find the chessboard/ some other method to find homography
        switch(calcType) // choose how source points to be determined
        {
            case(CHESSBOARD):
            {
                // finding the chessboard
                vector<Point2f> corners;
                bool foundChessboard;
                // pre-setting board_w & h
                int board_w = 5;
                int board_h = 8;
                Size boardSize = Size(board_w,board_h);
                foundChessboard = findChessboardCorners(undistortedCameraMat, boardSize, corners,
                                                        CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FILTER_QUADS);
                if(foundChessboard)
                {
                    Mat greyCameraMat;
                    cout << "found chessboard for perspective" <<endl;
                    // get subpixel accuracy on corners
                    cvtColor(undistortedCameraMat, greyCameraMat, CV_BGR2GRAY);
                    cornerSubPix(greyCameraMat, corners, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
                    
                    // now extracting the 4 points for homography
                    // [top left, top right, bottom right, bottom left] (Clockwise)
                    // this bit makes sense if you think in squares & the number of corners
                    
                    Point2f srcQuad[4] =
                    {
                        Point2f(corners[0]),
                        Point2f(corners[board_w-1]),  // board_w-1
                        Point2f(corners[(board_h-1)*board_w + board_w-1]),  // (board_h-1)*board_w + board_w-1
                        Point2f(corners[(board_h-1)*board_w])   // (board_h-1)*board_w
                    };
                    
                    // need to do something about the size transform... currently 'soved' by *100
                    Point2f dstQuad[4] =
                    {
                        Point2f(0,0),
                        Point2f(board_w*100-1, 0),
                        Point2f(board_w*100-1, board_h*100-1),
                        Point2f(0, board_h*100-1)
                    };
                    
                    drawChessboardCorners(undistortedCameraMat, boardSize, corners, foundChessboard);
                    imshow( "Checkers", undistortedCameraMat );
                    
                    hmgMat = getPerspectiveTransform(srcQuad, dstQuad);
                    
                    // saving...
                    FileStorage fs("camParameters/perspective.xml", FileStorage::WRITE);
                    fs << "hmgMat" << hmgMat;
                    fs.release();
                    
                    cout << "hmgMat" << hmgMat << endl;
                    destroyWindow("Checkers");
                    return true; // job's done here
                }
                else return false;
            }
            case(CLICKRECTANGLE):
            {
                // USE 4 USER DEFINED POINTS
                undistortedCameraMat.copyTo(perspectiveMat); // copy to a globally accessible variable
                
                namedWindow("ledRectangle", 0);
                setMouseCallback("ledRectangle", perspMouse, 0);
                perspectivePoints.empty();
                enoughPerspectivePoints = false;
                while(!enoughPerspectivePoints)
                {
                    // draw the circles on the image when clicked & also store the points of click
                    string msg = format("Click on four(4) points. Starting from the top left, moving clockwise");
                    putText(perspectiveMat, msg, Point(5,15), 1, 1,Scalar(0,255,0));
                    imshow("ledRectangle", perspectiveMat);
                    
                    char key =  waitKey(100);
                    if( key  == 27 ) // 27 == ESC
                        return false;
                }
                
                // once the points have been collected, destroy the imshow and perform the homography
                destroyWindow("ledRectangle");
                
                // choosing this kind of dstQuad will deform the image 
                
                Point2f srcQuad[4] =
                {
                    Point2f(perspectivePoints[0]),
                    Point2f(perspectivePoints[1]),
                    Point2f(perspectivePoints[2]),
                    Point2f(perspectivePoints[3])
                };
                
                // 'smart' way of determining destination size
                // defining the src image dimensions
                float topWidth, bottomWidth, leftHeight, rightHeight;
                topWidth = perspectivePoints[1].x - perspectivePoints[0].x;
                bottomWidth = perspectivePoints[2].x - perspectivePoints[3].x;
                leftHeight = perspectivePoints[3].y - perspectivePoints[0].y;
                rightHeight = perspectivePoints[2].y - perspectivePoints[1].y;
                
                bool verticalSkew;
                bool horizontalSkew;
                float scaled_w, scaled_h;
                
                if (abs(topWidth-bottomWidth) < abs(leftHeight-rightHeight)) 
                {
                    horizontalSkew = true; // so lH & rH are of interest
                    // determine which is smaller & scale according to aspect ratio
                    float minHeight = min(leftHeight, rightHeight); // scale it down
                    float unit = minHeight/ratio_h; // compare to the height ratio
                    scaled_w = ratio_w * unit; // scale the image properly
                    scaled_h = ratio_h * unit;
                    
                }
                else if(abs(topWidth-bottomWidth) > abs(leftHeight-rightHeight))
                {
                    verticalSkew = true; // so tW & bW are of interest
                    float minWidth = min(topWidth, bottomWidth);
                    float unit = minWidth/ratio_w; // compare to the width ratio
                    scaled_w = ratio_w * unit; // scale the image properly
                    scaled_h = ratio_h * unit;
                }
                
                Point2f dstQuad[4] =
                {
                    Point2f(0, 0),
                    Point2f(scaled_w, 0),
                    Point2f(scaled_w, scaled_h),
                    Point2f(0, scaled_h)
                };
                
                // output a decent display destination size
                // do some checks for scaled_w & h . modify if necessary to fit screen
                if(horizontalSkew)
                {
                    if(scaled_w > 800)
                    {
                        float downScale = 800/scaled_w;
                        scaled_w = 800;
                        scaled_h = scaled_h * downScale;
                    }
                }
                else if (verticalSkew)
                {
                    if(scaled_h > 800)
                    {
                        float downScale = 800/scaled_h;
                        scaled_h = 800;
                        scaled_w = scaled_w * downScale;
                    }
                }
                
                dstSize = Size(scaled_w, scaled_h);
                
                // depreciated
                //                Point2f dstQuad[4] =
                //                {
                //                    Point2f(0,0),
                //                    Point2f(perspectiveMat.cols-1, 0),
                //                    Point2f(perspectiveMat.cols-1, perspectiveMat.rows-1),
                //                    Point2f(0, perspectiveMat.rows-1)
                //                };
                
                hmgMat = getPerspectiveTransform(srcQuad, dstQuad);
                
                if(enoughPerspectivePoints /*&& hmgMat is not empty*/)
                {
                    // saving...
                    FileStorage fs("camParameters/perspective.xml", FileStorage::WRITE);
                    fs << "hmgMat" << hmgMat;
                    fs << "dstWidth" << scaled_w;
                    fs << "dstHeight" << scaled_h;
                    fs.release();
                    
                    cout << "hmgMat" << hmgMat << endl;
                    return true; // job's done here
                }
                else return false;
            }
            case(LEDRECTANGLE):
            {
                // to implement with 4 easily movable LEDs
                // need to get the 4 points of the LEDs
                
                // first prompt to capture background
                // second prompt to capture scene with 4 LEDs
                break;
            }
            default:
                cout << "unidentified source type, try again" << endl;
                return false;
                
        }
        
        // perform perspective undistortion
        
        return true;
    }
    return false;
}

#pragma mark Foot Detection
// FOOT DETECTION STEP // <-- THIS IS THE CORE CONTENT!!!
//void initFootDetection(int mogType = 2);
//
//
//void footDetection(Mat inputImage, int mogType = 2);
//
//void footDetection(Mat inputImage, int mogType)
//{
//    Mat background, foreground;
//    Mat frame; // temporary. sort and remove.
//    int mogLearningRate = 0.001;
//    
//#define USE_MOG_1 = 1
//#define USE_MOG_2 = 2
//    
//    
//    // BACKGROUND-FOREGROUND SEGMENTATION
//    // step out - step in . the history & learning rate helps to define how long this process is.
//    
//    if(mogType == 1)
//    {
//        // MOG implementation
//        BackgroundSubtractorMOG mog1; // MOG takes in a BINARY image
//        
////        cvtColor(processed, processed, CV_RGB2HSV); // sometimes greater contrast in HSV space
//        
//        //
//        inputImage.copyTo(frame);
//        //
//        mog1(frame,foreground,mogLearningRate);
//        mog1.getBackgroundImage(background);
//    
//    }
//    else if(mogType == 2)
//    {
//        // MOG2 implementation (history, varThreshold, shadow)
//        BackgroundSubtractorMOG2 mog2(100, 16, false);
//        //
//        inputImage.copyTo(frame);
//        //
//        mog2.operator()(frame, foreground,mogLearningRate);
//        mog2.getBackgroundImage(background);
//
//    }
//    else // probably unnecessary
//        return;
//    
//    // FOREGROUND CLEANING UP
//    morphologyEx(foreground, foreground, MORPH_OPEN, Mat()); // erode~dilate
////    bitwise_not(foreground, foreground);
//    
//    Mat image;
//    foreground.copyTo(image);
//    
//    // contour stuff
//    
//    vector<vector<Point> > contours;
//    
//    findContours(image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//    
//    // FILTER OUT "BAD" CONTOURS // EXTRACTING MAXIMUM 2 LIKELY AREAS OF FOOT PRESENCE 
//    // how to decide what is too big/ too small? people have different sized feet
//    int minContourSize = 800;
//    int maxContourSize = 2000;
//    
//    // also calculate the distance from the two "feet"
//
//    vector<vector<Point> >::iterator iterContours = contours.begin();
//    while(iterContours != contours.end())
//    {
//        if (iterContours->size() < minContourSize || iterContours->size() > maxContourSize)
//        {
//            iterContours = contours.erase(iterContours);
//        }
//        else
//            ++iterContours;
//    }
//    
//    Mat result(image.size(), CV_8U, Scalar(255));
//    drawContours(result,  contours, -1, Scalar(0),2);
//    imshow("contours", image);
//    createTrackbar("cmin", "contours", &minContourSize, 100);
//    createTrackbar("cmax", "contours", &maxContourSize, 6000);
//    imshow("foreground", foreground);
//    imshow("background", background);
//
//    
//    // we now have two good contours to base the foot on
//    // HISTOGRAM BACK PROJECTION TO HELP BUILD A 'PROFILE' OF WHAT A FOOT IS
//
//    
//    
//    
//    
//    // now to do some processing and get a good mask out of it
//    
//    // one method used
////    Mat mask;
////    mophologyEx(foreground, mask, MORPH_CLOSE, Mat());
////    
////    // another method used
////    Mat mask1(foreground.size((), CV_8U);
////    inputMat.copyTo(foreground,mask1);
//              
//}




// TRACKING STUFF......


#endif
