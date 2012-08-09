# include "main.h"


/* the code has been restructured to have clearly sectioned out code for both photo trials for testing, and the main foot sensing algorithms */

// DECLARE VARIABLES HERE //
// chosen image source
//Mat rawCameraMat;
//int camSource = 0;    


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
            putText(calibCameraMat, msg, Point(50,50), 1, 1,Scalar(0,255,0));

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

// initialising onMouse function
void onMouse(int event, int x, int y, int, void* param);

void onMouse(int event, int x, int y, int, void* param)
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

bool initPerspective(bool loadExisting, Mat undistortedCameraMat, int sourceType, Mat& hmgMat);

bool initPerspective(bool loadExisting, Mat undistortedCameraMat, int sourceType, Mat& hmgMat)
{
    
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
        
        return true;
    }
    else
    {
        // find the chessboard/ some other method to find homography
        switch(sourceType) // choose how source points to be determined
        {
            case(CHESSBOARD):
            {
                // finding the chessboard
                vector<Point2f> corners;
                bool foundChessboard;
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
                    
                    // need to do something about the size transform... currently 'soved' by *100
                    
                    Point2f srcQuad[4] =
                    {
                        Point2f(0,0),
                        Point2f(board_w*100-1, 0),
                        Point2f(board_w*100-1, board_h*100-1),
                        Point2f(0, board_h*100-1)
                    };
                    
                    Point2f dstQuad[4] =
                    {
                        Point2f(corners[0]),
                        Point2f(corners[board_w-1]),  // board_w-1
                        Point2f(corners[(board_h-1)*board_w + board_w-1]),  // (board_h-1)*board_w + board_w-1
                        Point2f(corners[(board_h-1)*board_w])   // (board_h-1)*board_w
                    };
                    
                    drawChessboardCorners(undistortedCameraMat, boardSize, corners, foundChessboard);
                    imshow( "Checkers", undistortedCameraMat );
                    
                    hmgMat = getPerspectiveTransform(dstQuad, srcQuad);

                    // saving...
                    FileStorage fs("camParameters/perspective.xml", FileStorage::WRITE);
                    fs << "hmgMat" << hmgMat;
                    fs.release();
                    
                    cout << "hmgMat" << hmgMat << endl;
                    return true; // job's done here
                }
                else return false;
            }
            case(CLICKRECTANGLE):
            {
                // USE 4 USER DEFINED POINTS
                undistortedCameraMat.copyTo(perspectiveMat); // copy to a globally accessible variable
                
                namedWindow("ledRectangle", 0);
                setMouseCallback("ledRectangle", onMouse, 0);
                perspectivePoints.empty();
                enoughPerspectivePoints = false;
                while(!enoughPerspectivePoints)
                {
                    // draw the circles on the image when clicked & also store the points of click
                    string msg = format("Click on four(4) points. Starting from the top left, moving clockwise");
                    putText(perspectiveMat, msg, Point(50,50), 1, 1,Scalar(0,255,0));
                    imshow("ledRectangle", perspectiveMat);
                    
                    char key =  waitKey(100);
                    if( key  == 27 ) // 27 == ESC
                        break;
                }
                
                // once the points have been collected, destroy the imshow and perform the homography
                destroyWindow("ledRectangle");
                
                Point2f srcQuad[4] =
                {
                    Point2f(0,0),
                    Point2f(perspectiveMat.cols-1, 0),
                    Point2f(perspectiveMat.cols-1, perspectiveMat.rows-1),
                    Point2f(0, perspectiveMat.rows-1)
                };
                
                Point2f dstQuad[4] =
                {
                    Point2f(perspectivePoints[0]),
                    Point2f(perspectivePoints[1]),
                    Point2f(perspectivePoints[2]),
                    Point2f(perspectivePoints[3])
                };
                
                hmgMat = getPerspectiveTransform(dstQuad, srcQuad);
                
                if(enoughPerspectivePoints /*&& hmgMat is not empty*/)
                {
                    // saving...
                    FileStorage fs("camParameters/perspective.xml", FileStorage::WRITE);
                    fs << "hmgMat" << hmgMat;
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


// FOOT DETECTION STEP // <-- THIS IS THE CORE CONTENT!!!
