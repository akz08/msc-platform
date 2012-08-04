# include "main.h"

/* the code has been restructured to have clearly sectioned out code for both photo trials for testing, and the main foot sensing algorithms */

// DECLARE VARIABLES HERE //
#pragma mark global variables
// chosen image source
Mat rawCameraMat;
//int camSource = 0;    



// INITIALISE CAMERA SOURCE //
// set the default camera source
// initialise VideoCapture
// check if valid camera source
VideoCapture initCamera(Mat rawCameraMat, int camSource, bool scaleDown);
void loopCamera(Mat rawCameraMat, VideoCapture capture, bool scaleDown);

VideoCapture initCamera(Mat rawCameraMat, int camSource, bool scaleDown)
{
    // initialise VideoCapture for use + initial 'snapshot' for testing purposes
    VideoCapture capture(camSource);
    if( !capture.isOpened() )
    {
        cout << "Invalid source chosen!" << endl;
        return -1;
    }
    capture >> rawCameraMat;
    if(scaleDown)
        pyrDown(rawCameraMat, rawCameraMat,Size(rawCameraMat.cols/2,rawCameraMat.rows/2));
    return capture;
}

void loopCamera(Mat rawCameraMat, VideoCapture capture, bool scaleDown)
{
    // loop camera read for main while loop
    capture >> rawCameraMat;
    if(scaleDown)
        pyrDown(rawCameraMat, rawCameraMat,Size(rawCameraMat.cols/2,rawCameraMat.rows/2));
}

void initUndistort(Mat rawCameraMat, VideoCapture capture, bool loadExisting, Mat& camMat, Mat& distortMat );

void initUndistort(Mat rawCameraMat, VideoCapture capture, bool loadExisting, Mat& camMat, Mat& distortMat)
{
    // to be called before the main while loop
    // has internal while loop for the calibration process
    // the while loop breaks on condition that calibration completes successfully 
    // (probably need to code for possible errors)
    if(loadExisting)
    {
        FileStorage fs;
        fs.open("camParameters/distortion.xml", FileStorage::READ);
        if(fs.isOpened())
        {
            cerr << "Failed to open distortion.xml" << endl;
            return;
        }
        fs["camMat"] >> camMat;
        fs["distortMat"] >> distortMat;
    }
    else 
    {
        // calculating the board corner positions... ie find chessboard ?
        vector<vector<Point2f> > imagePoints;
        vector<Point2f> corners;
        Size boardSize;
        bool found;
        clock_t prevTimestamp = 0;
        bool blinkOutput = false;
        found = findChessboardCorners(rawCameraMat, boardSize, corners,
                                      CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
        if(found)
        {
             // get subpixel accuracy on corners
            Mat viewGrey;
            cvtColor(rawCameraMat, viewGrey, CV_BGR2GRAY);
            cornerSubPix(viewGrey, rawCameraMat, Size(11,11), Size(-1,-1), TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
         
            // now we take new samples from the camera after a delay time...
            if(!capture.isOpened() || clock() - prevTimestamp > 1000*1e-3*CLOCKS_PER_SEC)
            {
                imagePoints.push_back(corners);
                prevTimestamp = clock();
                blinkOutput = capture.isOpened();
                
            }
            
            // draw chessboard corners
            drawChessboardCorners(rawCameraMat, boardSize, Mat(corners), found);
        }
        
        if(blinkOutput)
            bitwise_not(rawCameraMat, rawCameraMat);
        imshow("Distortion", rawCameraMat);
        
        // if we have a sufficient sample of images & points, we do the calibration
        int numFrames = 20; // take 20 samples 
        if( imagePoints.size() >= numFrames )
        {
            // run calibration function used here...
            camMat = Mat::eye(3, 3, CV_64F);
            distortMat = Mat::zeros(8, 1, CV_64F);
            vector<vector<Point3f> > objectPoints(1);
            objectPoints.resize(imagePoints.size(),objectPoints[0]); // necessary?
            
            // find camera params
            vector<Mat> rvecs, tvecs; // rotation & translation
            vector<float> reprojErrs;
            double rms = calibrateCamera(objectPoints, imagePoints, rawCameraMat.size(), camMat, distortMat, rvecs, tvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
            cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;
            // we COULD compute the total average error
            
            // now we should save the camera parameters...and end the initial loop
        }
    }
}

// option/ability to change source


// UNDISTORTION STEP //
// option to enable step (bool)
// load distortion matrices by default
// if none, initialise undistortion function
// upon completion of undistortion - save/override distortion files


// PERSPECTIVE CORRECTION STEP //
// option to enable step (bool)
// option to 
// choose between chessboard calibration or ...


// FOOT DETECTION STEP // <-- THIS IS THE CORE CONTENT!!!
