#include "main.h"
#include "camHelper.h"

camHelper cam;
Mat cameraOut, cameraUndistorted, cameraFixPerspective;

void mousePerspectiveWrap(int event, int x, int y, int, void* param);

int main()
{
    cam.initCamera(0);
    cam.updateCamera(cameraOut);
    cam.loadUndistort();
//    cam.calcUndistort(cameraOut);
    cam.initUndistort(cameraOut);
    cam.loadPerspective();
    
    namedWindow("ledRectangle", 0);
    setMouseCallback("ledRectangle", mousePerspectiveWrap, (void*)&cam);
    cam.calcPerspective(cameraOut, "ledRectangle");
    
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

void mousePerspectiveWrap(int event, int x, int y, int flags, void* pointer)
{
    camHelper* cHPointer = (camHelper*)pointer;
    if(cHPointer != NULL)
        cHPointer->onMouse(event, x, y, flags);
}