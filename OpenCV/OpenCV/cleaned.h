# include "main.h"


/* the code has been restructured to have clearly sectioned out code for both photo trials for testing, and the main foot sensing algorithms */

// DECLARE VARIABLES HERE //
#pragma mark global variables
// chosen image source
//Mat rawCameraMat;
//int camSource = 0;    



// INITIALISE CAMERA SOURCE //
// set the default camera source
// initialise VideoCapture
// check if valid camera source
VideoCapture initCamera(int camSource);
void loopCamera(Mat rawCameraMat, VideoCapture capture, bool scaleDown);
bool initUndistort(Mat rawCameraMat, VideoCapture capture, bool loadExisting, Mat& camMat, Mat& distortMat );

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

void loopCamera(Mat rawCameraMat, VideoCapture capture, bool scaleDown)
{
    // loop camera read for main while loop
    capture >> rawCameraMat;
    if(scaleDown)
        pyrDown(rawCameraMat, rawCameraMat,Size(rawCameraMat.cols/2,rawCameraMat.rows/2));
}

// UNDISTORTION STEP //
// option to enable step (bool)
// load distortion matrices by default
// if none, initialise undistortion function
// upon completion of undistortion - save/override distortion files

bool initUndistort(Mat rawCameraMat, VideoCapture capture, bool loadExisting, Mat& camMat, Mat& distortMat)
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
            return false;
        }
        fs["camMat"] >> camMat;
        fs["distortMat"] >> distortMat;
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


// PERSPECTIVE CORRECTION STEP //
// option to enable step (bool)
// option to 
// choose between chessboard calibration or ...


// FOOT DETECTION STEP // <-- THIS IS THE CORE CONTENT!!!
