#include <iostream>
#include <opencv2/opencv.hpp>
#include <cvblob.h>
#include <Phidget21/Phidget21.h>


using namespace std;
using namespace cv;
using namespace cvb;

//---------------USEFUL INFORMATION---------------//

// Some basics types for OpenCV 2.x
// Point - 2D point with integer values [Point point(x,y);]
// Size - a size [Size sz = Size(100,200)]
// Rect - a rectangle [Rect r1(0, 0, 100, 200)]
// Vec - a vector [Vec3f point = Vec3f(10,10,3.2)]
// Scalar - 4 element vector w/ double precision [Scalar v = Scalar::all(10)]
// Range - continuous range [Range range(0,5)]
// TermCriteria - termination criteria for iterative operations [TermCriteria(int _type, int _maxCount, double _epsilon)]

// Classes
// Mat
//      Copying - shallow (no data copied) [Mat A = B;]
//                deep    [clone()  ,   B.copyTo(A)]




//-----------------------------------------------//

// Declaring public variables //

Mat src, gray, distort;  // Contains image information. "Matlab-like"
int camSource = 1;  // Choose source of camera [0 - default, 1 - external, ...]

// stuff for perspective
int board_w = 7;
int board_h = 9;
Size board_sz = Size(board_w, board_h); 
int Z = 25;
bool found;
Point2f objPts[4], imgPts[4];
Mat H;
int gry;




// Matrices for Undistortion //
Mat camMat = (Mat_<double>(3,3) << \
              7.1125384521484375e+002, 0.0000000000000000e+000, 5.4314685058593750e+002,
              0.0000000000000000e+000, 7.6353259277343750e+002, 3.6925729370117187e+002,
              0.0000000000000000e+000, 0.0000000000000000e+000, 1.0000000000000000e+000);



Mat distortMat =(Mat_<double>(4,1) << \
                 -2.5127691030502319e-001, 3.6654811352491379e-002, -6.8524251691997051e-003,\
                 -4.1806353256106377e-003);



/// Function prototypes //
//double angle(Point, Point, Point);
//void findSquares(Mat&, vector<vector<Point> >&);
//void drawSquares(Mat&, vector<vector<Point> >&);

int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
void thresh_callback(int, void* );
Mat src_gray;


// Main //

int main (int argc, char** argv)
{
    // Phidgets ///
    
    // Declare & Initialise Phidget Object
    CPhidgetInterfaceKitHandle ifKit = 0;
    CPhidgetInterfaceKit_create(&ifKit);
    
    //////////////
    
    
    VideoCapture capture(camSource); // choosing the source of video. (0) being default
    if (!capture.isOpened()) // checking if chosen source is available
        return -1;
    
    // namedWindow("Source", CV_WINDOW_AUTOSIZE);    // highgui function ( this is redundant as imshow creates own window )
    capture >> src;
    
    

    
    
    // downscale the image
    pyrDown(src, src,Size(src.cols/2, src.rows/2) );
    
     Mat hsv,ranger;
    
    
    // TEST PERSPECTIVE
    undistort(src,distort,camMat,distortMat);
    //distort = src;
    
    cvtColor(distort, gray, CV_BGR2GRAY);
    cvtColor(distort, src_gray, CV_BGR2GRAY);
    
    //GET THE CHECKERBOARD ON THE PLANE
    Mat equal;
    cvtColor(distort, equal, CV_BGR2GRAY);
    adaptiveThreshold(equal, equal, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 101, 7);
    //equalizeHist(equal, equal);
    imshow("too", equal);
    vector<Point2f> corners;
    found = findChessboardCorners(equal, board_sz, corners,
                                  CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
    if(found) {
        //Get Subpixel accuracy on those corners
        cornerSubPix(gray, corners,
                     Size(11,11),Size(-1,-1), 
                     TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
        
        //GET THE IMAGE AND OBJECT POINTS:
        //Object points are at (r,c): (0,0), (board_w-1,0), (0,board_h-1), (board_w-1,board_h-1)
        //That means corners are at: corners[r*board_w + c]
        //            Point2f objPts[4], imgPts[4];
        objPts[0].x = 0;         objPts[0].y = 0; 
        objPts[1].x = board_w-1; objPts[1].y = 0; 
        objPts[2].x = 0;         objPts[2].y = board_h-1;
        objPts[3].x = board_w-1; objPts[3].y = board_h-1; 
        imgPts[0] = corners[0];
        imgPts[1] = corners[board_w-1];
        imgPts[2] = corners[(board_h-1)*board_w];
        imgPts[3] = corners[(board_h-1)*board_w + board_w-1];
        
        
        //FIND THE HOMOGRAPHY
        H = getPerspectiveTransform(objPts, imgPts);
        
        cout << H;
    }

    // set a default perspective transform
    
    H = (Mat_<double>(3,3) << \
         41.64548974825176, 30.65534518492649, 204.4431915283202,
         1.377646560422583, 37.35908584238238, 71.58721923828102,
         0.003282731684869353, 0.08694364928835308, 1);
    
    while (true)        // loop until keypress
    {
        // Source Camera
        capture >> src; // offload captured image from camera -> src
            pyrDown(src, src,Size(src.cols/2, src.rows/2) );
        // Undistort
        undistort(src,distort,camMat,distortMat);
        //distort = src;
        
        // Camera Input
        imshow("Source", distort);        // highgui function
    
        // Find Square Viewport
//        vector<vector<Point> > squares;
//        findSquares(distort, squares);
//        drawSquares(distort, squares);
        
//        blur(src, outBlur, Size(10,10)); // Size(...,...) <- template (width, height) class

        cvtColor(src, gray, CV_BGR2GRAY);    // where 3rd arg is 'color space conversion code

        threshold(gray, gray, 200, 100, THRESH_BINARY);
        //imshow("test", gray);
        //createTrackbar("test", "test", &gry, 400,0);
        
//        Mat gray(gray.size(), CV_8UC1) = gray.clone();
//        
//        IplImage grayer = gray(gray.size(), CV_8UC1);
//        cvCvtColor(grayer, grayer, CV_RGB2BGR);
//        CvBlob blobs;
//        IplImage *labelImg = cvCreateImage(cvGetSize(grayer), IPL_DEPTH_LABEL, 1);
    
        
        //cvtColor(src, hsv, CV_BGR2HSV);
        //inRange(hsv, Scalar(0,100,100), Scalar(10, 255,255),ranger); 
       


        createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
        thresh_callback( 0, 0 );
        

        

        
        if(true){ //found
            
//            //DRAW THE POINTS in order: B,G,R,YELLOW
//            circle(distort,imgPts[0],9,Scalar(255,0,0),3);
//            circle(distort,imgPts[1],9,Scalar(0,255,0),3);
//            circle(distort,imgPts[2],9,Scalar(0,0,255),3);
//            circle(distort,imgPts[3],9,Scalar(0,255,255),3);
//            
//            drawChessboardCorners(gray, board_sz, corners, found);
            
//            //FIND THE HOMOGRAPHY
//            Mat H = getPerspectiveTransform(objPts, imgPts);
            
            //LET THE USER ADJUST THE Z HEIGHT OF THE VIEW
           // double Z = 25;
            Mat birds_image;
            
                H.at<double>(2, 2) = Z;
                //USE HOMOGRAPHY TO REMAP THE VIEW
                warpPerspective(distort, birds_image, H, distort.size(), WARP_INVERSE_MAP + INTER_LINEAR,
                                BORDER_CONSTANT, Scalar::all(0));
                imshow("Birds_Eye", birds_image);
            
            createTrackbar("height", "Birds_Eye", &Z, 50,0);

        }

        // redundant 
//        imshow("test", gray);
//        imshow("Source", distort);
        
        
//        // Keypress check
        if (waitKey(10) >= 0)       // highgui function [waitKey(int delay)]
            break;

              
    }
    
    destroyAllWindows();
    return 0;
}


// Function prototypes //


void thresh_callback(int, void* )
{
    Mat src_copy = distort.clone();
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Detect edges using Threshold
    threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
    
    /// Find contours
    findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    /// Find the convex hull object for each contour
    vector<vector<Point> >hull( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    {  convexHull( Mat(contours[i]), hull[i], false ); }
    
    /// Draw contours + hull results
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        drawContours( drawing, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    }
    
    /// Show in a window
    namedWindow( "Hull demo", CV_WINDOW_AUTOSIZE );
    imshow( "Hull demo", drawing );
}
