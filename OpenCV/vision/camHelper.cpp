#include "camHelper.h"

void camHelper::initCamera(int camSource)
{
    camCapture.open(camSource);
    
    if( !camCapture.isOpened() )
    {
        cout << "Invalid source chosen! Setting to default." << endl;
        camCapture.open(0);
    }
}

void camHelper::updateCamera(Mat& outputMatrix)
{
    camCapture >> outputMatrix;
}

bool camHelper::loadUndistort()
{
    FileStorage fs;
    fs.open("camParameters/distortion.xml", FileStorage::READ);
    if(!fs.isOpened())
    {
        cerr << "Failed to open distortion.xml" << endl;
        return false;
    }
    fs["camMat"] >> cameraMatrix;
    fs["distortMat"] >> distortionCoeff;
    return true;
}

void camHelper::initUndistort(Mat inputMatrix)
{
    initUndistortRectifyMap(cameraMatrix, distortionCoeff, Mat(), getOptimalNewCameraMatrix(cameraMatrix, distortionCoeff, inputMatrix.size(), 1), inputMatrix.size(), CV_16SC2, map1, map2);
}

bool camHelper::calcUndistort(Mat inputMatrix)
{
    Mat rawCameraMat;
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
        updateCamera(rawCameraMat);
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
            if(!camCapture.isOpened() || clock() - prevTimestamp > delay*1e-3*CLOCKS_PER_SEC)
            {
                imagePoints.push_back(corners); // add the current frame's corners
                prevTimestamp = clock();        // ref time for 'last capture'
                blinkOutput = camCapture.isOpened();
                
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
            cameraMatrix = Mat(3, 3, CV_64F);
            distortionCoeff = Mat::zeros(8, 1, CV_64F);
            
            vector<vector<Point3f> > objectPoints(1);
            
            for(int i = 0; i < boardSize.height; ++i)
                for(int j = 0; j < boardSize.width; ++j)
                    objectPoints[0].push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
            
            objectPoints.resize(imagePoints.size(),objectPoints[0]); 
            
            // find camera params
            vector<Mat> rvecs, tvecs; // rotation & translation vectors
            vector<float> reprojErrs; // reprojection erros
            
            double rms = calibrateCamera(objectPoints, imagePoints, calibCameraMat.size(), cameraMatrix, distortionCoeff, rvecs, tvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
            
            cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;
            // we COULD compute the total error, but there's no need here
            
            // now we should save the camera parameters...and end the initial loop
            
            // saving...
            FileStorage fs("camParameters/distortion.xml", FileStorage::WRITE);
            fs << "camMat" << cameraMatrix;
            fs << "distortMat" << distortionCoeff;
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
    
    return false;
}

void camHelper::doUndistort(Mat inputMatrix, Mat &outputMatrix)
{
    remap(inputMatrix, outputMatrix, map1, map2, INTER_LINEAR);
}

bool camHelper::loadPerspective()
{
    FileStorage fs;
    fs.open("camParameters/perspective.xml", FileStorage::READ);
    if(!fs.isOpened())
    {
        cerr << "Failed to open perspective.xml" << endl;
        return false;
    }
    fs["hmgMat"] >> homographyMatrix;
    
    float dstWidth, dstHeight;
    fs["dstWidth"] >> dstWidth;
    fs["dstHeight"] >> dstHeight;
    destinationSize = Size(dstWidth, dstHeight);
    return true;
}

void camHelper::onMouse(int event, int x, int y, int flags)
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

bool camHelper::calcPerspective(Mat inputMatrix, string winName, int calcType)
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
            foundChessboard = findChessboardCorners(inputMatrix, boardSize, corners,
                                                    CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FILTER_QUADS);
            if(foundChessboard)
            {
                Mat greyCameraMat;
                cout << "found chessboard for perspective" <<endl;
                // get subpixel accuracy on corners
                cvtColor(inputMatrix, greyCameraMat, CV_BGR2GRAY);
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
                
                drawChessboardCorners(inputMatrix, boardSize, corners, foundChessboard);
                imshow( "Checkers", inputMatrix );
                
                homographyMatrix = getPerspectiveTransform(srcQuad, dstQuad);
                
                // saving...
                FileStorage fs("camParameters/perspective.xml", FileStorage::WRITE);
                fs << "hmgMat" << homographyMatrix;
                fs.release();
                
                cout << "hmgMat" << homographyMatrix << endl;
                destroyWindow("Checkers");
                return true; // job's done here
            }
            else return false;
        }
        case(CLICKRECTANGLE):
        {
            // USE 4 USER DEFINED POINTS
            inputMatrix.copyTo(perspectiveMat); // copy to a globally accessible variable

            perspectivePoints.empty();
            enoughPerspectivePoints = false;
            while(!enoughPerspectivePoints)
            {
                // draw the circles on the image when clicked & also store the points of click
                string msg = format("Click on four(4) points. Starting from the top left, moving clockwise");
                putText(perspectiveMat, msg, Point(5,15), 1, 1,Scalar(0,255,0));
                imshow(winName, perspectiveMat);
                
                char key =  waitKey(100);
                if( key  == 27 ) // 27 == ESC
                    return false;
            }
            
            
            // once the points have been collected, destroy the imshow and perform the homography
            destroyWindow(winName);
            
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
            
            destinationSize = Size(scaled_w, scaled_h);
            
            homographyMatrix = getPerspectiveTransform(srcQuad, dstQuad);
            
            if(enoughPerspectivePoints /*&& hmgMat is not empty*/)
            {
                // saving...
                FileStorage fs("camParameters/perspective.xml", FileStorage::WRITE);
                fs << "hmgMat" << homographyMatrix;
                fs << "dstWidth" << scaled_w;
                fs << "dstHeight" << scaled_h;
                fs.release();
                
                cout << "hmgMat" << homographyMatrix << endl;
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
    
    return true;
}

void camHelper::doPerspective(Mat inputMatrix, Mat &outputMatrix)
{
    warpPerspective(inputMatrix, outputMatrix, homographyMatrix, destinationSize,  INTER_LINEAR+CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT, Scalar());
    
}








