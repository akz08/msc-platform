#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

#include "opencv2/opencv.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <iostream>
//#include <math.h>
//#include <time.h>
#include "camHelper.h"
#include "footDetect.h"

void mousePerspectiveWrap(int event, int x, int y, int, void* param);

void mousePerspectiveWrap(int event, int x, int y, int flags, void* pointer)
{
    camHelper* cHPointer = (camHelper*)pointer;
    if(cHPointer != NULL)
        cHPointer->onMouse(event, x, y, flags);
}

//========================================================================
int main( ){

//    camHelper cam;
//    footDetect foot;
//    Mat cameraOut, cameraUndistorted, cameraFixPerspective;
//    Mat background, foreground;
//    Mat foundFeet;
//    
//    cam.initCamera(0);
//    cam.updateCamera(cameraOut);
//    cam.loadUndistort();
////    cam.calcUndistort(cameraOut);
////    cam.initUndistort(cameraOut);
//    cam.loadPerspective();
//    
////    namedWindow("ledRectangle", 0);
////    setMouseCallback("ledRectangle", mousePerspectiveWrap, (void*)&cam);
////    cam.calcPerspective(cameraOut, "ledRectangle");
    
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1300,768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
