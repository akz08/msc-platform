#include "main.h"
#include "camHelper.h"
#include "footDetect.h"

camHelper cam;
footDetect foot;
Mat cameraOut, cameraUndistorted, cameraFixPerspective;
Mat background, foreground;
Mat foundFeet, threshFeet;

void mousePerspectiveWrap(int event, int x, int y, int, void* param);

VideoWriter testWriter;
VideoCapture labReader("video.avi");
Mat labTest, labThresh;
footDetect labDetector;

int main()
{
        cam.initCamera(0);
    cam.updateCamera(cameraOut);
    cam.loadUndistort();
//    cam.calcUndistort(cameraOut);
    cam.initUndistort(cameraOut);
    cam.loadPerspective();
    
//    namedWindow("ledRectangle", 0);
//    setMouseCallback("ledRectangle", mousePerspectiveWrap, (void*)&cam);
//    cam.calcPerspective(cameraOut, "ledRectangle");
    
    foot.initMOG();
    cam.doUndistort(cameraOut, cameraUndistorted);
    cam.doPerspective(cameraUndistorted, cameraFixPerspective);
    labReader.read(labTest);
    testWriter.open("null.avi",CV_FOURCC('M','J','P','G'),15,labTest.size(),true); // 15 fps
    if (!testWriter.isOpened())
    {
        cout  << "Could not open the output video for write: " <<endl;
    }
    
//    cout << "fps" <<cam.camCapture.get(CV_CAP_PROP_FPS)<<endl;
    while(true)
    {
        if (labReader.read(labTest)) {
//            labReader >> labTest;
            imshow("labTest", labTest);
            
            labDetector.updateMOG(labTest);
            labDetector.setLearningRate(0.0000000001);
            labDetector.grabForeground();
            labDetector.threshForeground(labTest, labThresh);
            imshow("labThresh", labThresh);
            cvtColor(labThresh.clone(), labThresh, CV_GRAY2BGR);
            testWriter.write(labThresh);
        }
        
        cam.updateCamera(cameraOut);
        imshow("raw camera output", cameraOut);
        
        
        cam.doUndistort(cameraOut, cameraUndistorted);
        imshow("undistorted camera output", cameraUndistorted);
        
        cam.doPerspective(cameraUndistorted, cameraFixPerspective);
        imshow("fixed perspective camera output", cameraFixPerspective);
        
        
        
        // preferably do some thresholding first
        foot.updateMOG(cameraFixPerspective);
        foot.getBackground(background);
        imshow("MOG Background", background);
        
//        imshow("Foreground", foot.foreground);

//        foot.getForeground(cameraFixPerspective, foreground);
//        imshow("MOG Foreground", foreground);
         
//        foot.grabForeground();
        
        foot.setLearningRate(0.0000001);
        foot.grabForeground();
        foot.threshForeground(cameraFixPerspective, threshFeet);
        imshow("threshold feet", threshFeet);
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