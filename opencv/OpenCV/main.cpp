#include "main.h"
#include "camHelper.h"
#include "footDetect.h"

camHelper cam;
footDetect foot;
Mat cameraOut, cameraUndistorted, cameraFixPerspective;
Mat background, foreground;
Mat foundFeet;

void mousePerspectiveWrap(int event, int x, int y, int, void* param);

int main()
{
    cam.initCamera(0);
    cam.updateCamera(cameraOut);
//    cam.loadUndistort();
    cam.calcUndistort(cameraOut);
    cam.initUndistort(cameraOut);
//    cam.loadPerspective();
    
    namedWindow("ledRectangle", 0);
    setMouseCallback("ledRectangle", mousePerspectiveWrap, (void*)&cam);
    cam.calcPerspective(cameraOut, "ledRectangle");
    
    foot.initMOG();
    
    while(true)
    {
        cam.updateCamera(cameraOut);
//        imshow("raw camera output", cameraOut);
       
        cam.doUndistort(cameraOut, cameraUndistorted);
//        imshow("undistorted camera output", cameraUndistorted);
        
        cam.doPerspective(cameraUndistorted, cameraFixPerspective);
        imshow("fixed perspective camera output", cameraFixPerspective);
        
        // preferably do some thresholding first
        foot.updateMOG(cameraFixPerspective);
        foot.getBackground(background);
        imshow("MOG Background", background);
        
        imshow("Foreground", foot.foreground);

//        foot.getForeground(cameraFixPerspective, foreground);
//        imshow("MOG Foreground", foreground);
         
//        foot.grabForeground();
        
        foot.setLearningRate(0.0000001);
        foot.grabForeground();
        foot.findFeet(cameraFixPerspective,foundFeet);
        imshow("found feet", foundFeet);
        foot.getHueHistogram(foundFeet, cameraFixPerspective);
        
        char key =  waitKey(50);
        if(key == 32)
        {
            cout << "decreased learning rate to 0.0000001" << endl;
            foot.setLearningRate(0.0000001);
        }
        if (key == 99)
        {
            foot.grabForeground();
            foot.findFeet(cameraFixPerspective,foundFeet);
            imshow("found feet", foundFeet);
            foot.getHueHistogram(foundFeet, cameraFixPerspective);
        }
        if(key  == 27) // 27 == ESC
            break;

    }
    
    return 0;
}

void mousePerspectiveWrap(int event, int x, int y, int flags, void* pointer)
{
    camHelper* cHPointer = (camHelper*)pointer;
    if(cHPointer != NULL)
        cHPointer->onMouse(event, x, y, flags);
}