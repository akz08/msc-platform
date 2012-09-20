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
VideoWriter threshWriter;
VideoCapture labReader("video.avi");
Mat labTest, labThresh;
footDetect labDetector;

int main()
{
    //Phidgets
//    phidget.connect(1000);
//    bridgeValuesArray[0].slope = 28.1155;
//    bridgeValuesArray[0].yIntercept = 0.918477;
//    bridgeValuesArray[1].slope = 28.2327;
//    bridgeValuesArray[1].yIntercept = 0.0812215;
//    bridgeValuesArray[2].slope = 22.7891;
//    bridgeValuesArray[2].yIntercept = 0.907176;
//    bridgeValuesArray[3].slope = 24.1739;
//    bridgeValuesArray[3].yIntercept = 0.686466;
    
    
//    phidgetFile.open("phidget.csv");
//    phidgetFile << "start" << endl;
    
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
//    labReader.read(labTest);
    testWriter.open("footdetectionsample.avi",CV_FOURCC('M','J','P','G'),15,cameraFixPerspective.size(),true); // 15 fps
    threshWriter.open("threshfootdetectionsample.avi",CV_FOURCC('M','J','P','G'),15,cameraFixPerspective.size(),true);
//    if (!testWriter.isOpened())
//    {
//        cout  << "Could not open the output video for write: " <<endl;
//    }
    
//    cout << "fps" <<cam.camCapture.get(CV_CAP_PROP_FPS)<<endl;
    while(true)
    {
        // Phidget
//        if (phidget.isConnected){
//            
//            for (int i = 0; i < 4; i++){
//                bridgeValuesArray[i].currentValue = phidget.getValues()[i];
//                bridgeValuesArray[i].currentCalibratedValue = ((bridgeValuesArray[i].currentValue*bridgeValuesArray[i].slope) + bridgeValuesArray[i].yIntercept) - zeroIn[i];
//            }
//
//        }
//        else {
//            cout << "Phidget didn't connect..." << endl;
//        }
//        
//        
//        //cout << phidget.getValue(3) << endl;
//        printf("%f", phidget.getValue(3));
        
        
        
//        if (labReader.read(labTest)) {
////            labReader >> labTest;
//            imshow("labTest", labTest);
//            
//            labDetector.updateMOG(labTest);
//            labDetector.setLearningRate(0.0000000001);
//            labDetector.grabForeground();
//            labDetector.threshForeground(labTest, labThresh);
//            imshow("labThresh", labThresh);
//            cvtColor(labThresh.clone(), labThresh, CV_GRAY2BGR);
//            testWriter.write(labThresh);
//        }
        
        cam.updateCamera(cameraOut);
        imshow("raw camera output", cameraOut);
        
        
        cam.doUndistort(cameraOut, cameraUndistorted);
        imshow("undistorted camera output", cameraUndistorted);
        
        cam.doPerspective(cameraUndistorted, cameraFixPerspective);
        
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        
//        temp1 = clock();
//        seconds = time (NULL);
        string msg1 = format("%s", asctime (timeinfo));
        putText(cameraFixPerspective, msg1, Point(15,15), 1, 1,Scalar(255));
        testWriter.write(cameraFixPerspective);
        imshow("fixed perspective camera output", cameraFixPerspective);
        
//        testWriter.write()
        
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
        temp = clock();
        string msg = format("%s", asctime (timeinfo));//"%f", (float)temp);
//        phidgetFile << "start" << endl;
//        phidgetFile << bridgeValuesArray[0].currentCalibratedValue << ",";
//        phidgetFile << bridgeValuesArray[1].currentCalibratedValue << ",";
//        phidgetFile << bridgeValuesArray[2].currentCalibratedValue << ",";
//        phidgetFile << bridgeValuesArray[3].currentCalibratedValue << ",";
//        phidgetFile << temp << endl;
        
        putText(threshFeet, msg, Point(15,15), 1, 1,Scalar(255));
        
        imshow("threshold feet", threshFeet);
        cvtColor(threshFeet.clone(), threshFeet, CV_GRAY2BGR);
        threshWriter.write(threshFeet);
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
        {
//            phidgetFile.close();
            break;

        }
    }
    
    return 0;
}

void mousePerspectiveWrap(int event, int x, int y, int flags, void* pointer)
{
    camHelper* cHPointer = (camHelper*)pointer;
    if(cHPointer != NULL)
        cHPointer->onMouse(event, x, y, flags);
}