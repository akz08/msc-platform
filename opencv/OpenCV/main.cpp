#include "main.h"

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

Mat src, distort;  // Contains image information. "Matlab-like"
Mat gray, hsv,hsv1;
Mat ranger;
int camSource = 0;  // Choose source of camera [0 - default, 1 - external, ...]

// variables for perspective
int board_w = 7;
int board_h = 9;
Size board_sz = Size(board_w, board_h); 
int Z = 25; // "height" from chessboard
bool blPerspective;
Point2f objPts[4], imgPts[4];
Mat H;
int gry;

//var for histogram 
MatND skinHistogram;
int hbins = 30, sbins = 32;
int histSize[] = {hbins, sbins};
float hrange[] = {0, 180};
float srange[] = {0, 256}; // upper boundary is exclusive
const float* histRange[] = {hrange, srange};
int channels[] = {0, 1};
bool unif = true;
bool accu = false;

MatND backHistogram;


Mat image;
 Mat test;


// Matrices for Undistortion //
Mat camMat = (Mat_<double>(3,3) << \
              7.1125384521484375e+002, 0.0000000000000000e+000, 5.4314685058593750e+002,
              0.0000000000000000e+000, 7.6353259277343750e+002, 3.6925729370117187e+002,
              0.0000000000000000e+000, 0.0000000000000000e+000, 1.0000000000000000e+000);



Mat distortMat =(Mat_<double>(4,1) << \
                 -2.5127691030502319e-001, 3.6654811352491379e-002, -6.8524251691997051e-003,\
                 -4.1806353256106377e-003);



/// Function prototypes //


int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
void thresh_callback(const Mat&, const Mat&);
Mat test_gray;


// Main //

int main (int argc, char** argv)
{
    
    FileStorage fs("test/test.yml", FileStorage::WRITE);
    Mat cameraMatrix = (Mat_<double>(3,3) << 1000, 0, 320, 0, 1000, 240, 0, 0, 1);
    fs << "cameraMatrix" << cameraMatrix;
    fs.release();
    
    cout<<"worked!"<<endl;
    
    // INITIATE VIDEO CAPTURE //
    VideoCapture capture(camSource); // choosing the source of video. (0) being default
    if (!capture.isOpened()) // checking if chosen source is available
    { cout << "Invalid source chosen!" ;
        return -1; }
    
    capture >> src;
    
    // downscale the image
    pyrDown(src, src,Size(src.cols/2, src.rows/2) );
    
    //end//
    
    // DISTORTION CORRECTION //
    // undistort source (wide angle lens option)
    undistort(src,distort,camMat,distortMat);
    
    //end//
    
    
    
    // PERSPECTIVE CORRECTION (based on "Learning OpenCV - Gary Bradski") //
    
    cvtColor(distort, gray, CV_BGR2GRAY);
    //cvtColor(distort, src_gray, CV_BGR2GRAY);
    
    
    // get chessboard on plane
    Mat adaptive;
    cvtColor(distort, adaptive, CV_BGR2GRAY);
    adaptiveThreshold(adaptive, adaptive, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 101, 7);
    //equalizeHist(equal, equal);
    //imshow("Adaptive Histogram", adaptive);
    vector<Point2f> corners;
    blPerspective = findChessboardCorners(adaptive, board_sz, corners,
                                  CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
    if(blPerspective) {
        //Get Subpixel accuracy on corners
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

    // else set a default perspective transform
    else{
        H = (Mat_<double>(3,3) << \
             41.64548974825176, 30.65534518492649, 204.4431915283202,
             1.377646560422583, 37.35908584238238, 71.58721923828102,
             0.003282731684869353, 0.08694364928835308, 1);
    }
    
    // end // 
    
    // sample image
    
    image = imread("sample.jpg");
    imshow("sample", image);
    
    
    
    // test image
   
    test = imread("test.jpg");
    imshow("test", test);
    
//    // check 'c' key press to continue 
//    while(true){
//        if(waitKey(0) == 99) break;
//    }

    
//    ioPhidget phidget;
//    phidget.connect(1000);
    
//    if(phidget.isAttached){
//        printf("it is attached!");
//    }
//    else{
//        printf("not attached!");
//    }
    
   


    
    
    while (true)        // loop until keypress
    {

        
        // UPDATE CAMERA FEED //
        capture >> src; // offload captured image from camera -> src
        pyrDown(src, src,Size(src.cols/2, src.rows/2) );
        // Undistort
        undistort(src,distort,camMat,distortMat);
        
        //temporary//
        distort = image;
        
        // Camera Input
        imshow("Source", distort);        // highgui function
        
        // end //
    
        // temporary//
        src = image;
        
        // IMAGE PREPARATION //
        cvtColor(src, gray, CV_BGR2GRAY);    // where 3rd arg is 'color space conversion code
        cvtColor(src, hsv, CV_BGR2HSV);
        
        cvtColor(test, hsv1, CV_BGR2HSV);
 
        
        // obtain histogram 
        
        calcHist(&hsv,1,channels,Mat(),
                 skinHistogram,2,histSize,histRange,
                 true,false);
        
        // normalise 
        calcBackProject(&hsv1,1,channels,
                        skinHistogram,backHistogram,
                        histRange);
        
        normalize( backHistogram, backHistogram,0, 255, NORM_MINMAX);

        GaussianBlur(backHistogram, backHistogram, Size(31,31), 0);
        
        
        
        threshold(backHistogram, backHistogram, 105, 255, THRESH_BINARY);
//adaptiveThreshold(backHistogram, backHistogram, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 101, 7);
        
        //equalizeHist(backHistogram, backHistogram);
        Mat mask;
        morphologyEx(backHistogram, mask, MORPH_CLOSE, Mat());
        
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        findContours(mask, contours,hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        
        vector<RotatedRect> minRect( contours.size() );
        
        for( int i = 0; i <contours.size(); i++)
        { minRect[i] = minAreaRect( Mat(contours[i]) );}
        
        // minAreaRect returns type RotatedRect . contains : center, size(wxh), angle(cw in deg), box(obsolete)
        
        /// Draw contours
        //Mat drawing = Mat::zeros( backHistogram.size(), CV_8UC3 );
        Mat drawing = test.clone();
        
        // going through all contours
        for( int i = 0; i< contours.size(); i++ )
        {
            Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
            drawContours( drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
            
            Point2f rect_points[4]; minRect[i].points(rect_points);
            
            // calculating the midway points
            Point2f half_points[4]; 
            
            for( int j = 0; j< 4; j++){
                line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8);
                
                half_points[j].x = ( rect_points[j].x+rect_points[(j+1)%4].x )/2 ;
                half_points[j].y = ( rect_points[j].y+rect_points[(j+1)%4].y )/2 ;
            }
            
            for( int j = 0; j < 2; j++){
                
                line(drawing, half_points[j], half_points[j+2], color);
            }
        }
        imshow("contours", drawing);
             
        imshow("skin back projection", backHistogram);
                
        
        cvtColor(image, test_gray, CV_BGR2GRAY);
         //thresh_callback( test, test_gray );

        
//        blur(src, outBlur, Size(10,10)); // Size(...,...) <- template (width, height) class
        
        threshold(gray, gray, 200, 100, THRESH_BINARY);
        //imshow("test", gray);
        //createTrackbar("test", "test", &gry, 400,0);c
        
//        Mat gray(gray.size(), CV_8UC1) = gray.clone();
//        
//        IplImage grayer = gray(gray.size(), CV_8UC1);
//        cvCvtColor(grayer, grayer, CV_RGB2BGR);
//        CvBlob blobs;
//        IplImage *labelImg = cvCreateImage(cvGetSize(grayer), IPL_DEPTH_LABEL, 1);
    
        
        //cvtColor(src, hsv, CV_BGR2HSV);
        //inRange(hsv, Scalar(0,100,100), Scalar(10, 255,255),ranger); 
       


        //createTrackbar( " Threshold:", "Source", &thresh, max_thresh );
       
        

        

        
        if(blPerspective){ //found
            
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
        
        
//        // Keypress check
        if (waitKey(10) == 27)       // highgui function [waitKey(int delay)] , 27 corresponds to ESC
            break;

              
    }
    
    destroyAllWindows();
    return 0;
}


// Function prototypes //


void thresh_callback(const Mat& image, const Mat& src_gray )
{
    Mat src_copy = image.clone();
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
