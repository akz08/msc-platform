#include "main.h"
#include "cleaned.h"

// Global variables
Mat rawCameraMat;
int cameraSource = 0; // 0 being default
Mat camMat, distortMat;
Mat nextImage(VideoCapture capture);

Mat nextImage(VideoCapture capture){
    Mat result;
    Mat view0;
    capture >> view0;
    view0.copyTo(result);
    return result;
}


int main()
{
    VideoCapture capture;
    capture = initCamera(cameraSource);
    bool undistorted = initUndistort(rawCameraMat, capture, true, camMat, distortMat);
    
    while(true&&undistorted)
    {
        Mat temp;
        Mat temps;
        temps = nextImage(capture).clone();
        undistort(temps, temp, camMat, distortMat);
        imshow("undistorted", temp);

        capture >> rawCameraMat;
        imshow("raw", rawCameraMat);
        
        char key =  waitKey(100);
        
        if( key  == 27 )
            break;

    }
    
    return 0;
}