#include "main.h"
#include "camHelper.h"
// Global variables

camHelper cam;
Mat cameraOut, cameraUndistorted, cameraFixPerspective;

int main()
{
    cam.initCamera(0);
    cam.updateCamera(cameraOut);
    cam.loadUndistort();
    cam.initUndistort(cameraOut);
    cam.loadPerspective();
    
    while(true)
    {
        cam.updateCamera(cameraOut);
        imshow("raw camera output", cameraOut);
       
        cam.doUndistort(cameraOut, cameraUndistorted);
        imshow("undistorted camera output", cameraUndistorted);
        
        cam.doPerspective(cameraUndistorted, cameraFixPerspective);
        imshow("fixed perspective camera output", cameraFixPerspective);
        
        char key =  waitKey(100);
        if( key  == 27 ) // 27 == ESC
            break;

    }
    
    return 0;
}

//        createTrackbar("canny threshold", "output", &lowThreshold, 100);