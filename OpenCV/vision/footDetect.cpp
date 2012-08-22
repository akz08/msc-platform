#include "footDetect.h"

void footDetect::initMOG(int history, float varThreshold, double defaultLearningRate)
{
    BackgroundSubtractorMOG2 mog(history, varThreshold, false);
    _mogLearningRate = defaultLearningRate;
    
}

void footDetect::updateMOG(Mat inputMatrix)
{
    //    mog.set("nmixtures", 5);

    mog.operator()(inputMatrix, foreground, _mogLearningRate);
    morphologyEx(foreground, foreground, MORPH_OPEN, Mat());
}

void footDetect::setLearningRate(double inputLearningRate)
{
    _mogLearningRate = inputLearningRate;
}

void footDetect::getBackground(Mat& outputMatrix)
{
    mog.getBackgroundImage(outputMatrix);
}

void footDetect::getForeground(Mat inputMatrix, Mat& outputMatrix)
{
    foreground.copyTo(outputMatrix);
    Mat mask(inputMatrix.size(), CV_8UC1);
    inputMatrix.copyTo(outputMatrix, mask);
}

bool footDetect::grabForeground()
{
    char key = waitKey(10);
    if (key == 99)
    {
        foreground.copyTo(grabbedFrame);
        findFeet(); // temporary
        imshow("snapshot", grabbedFrame);
        return  true;
    }
    
    return false;
}

void footDetect::findFeet()
{
    // find from grabbedFrame (temporary?)
    morphologyEx(grabbedFrame, grabbedFrame, MORPH_OPEN, Mat::ones(4,4,CV_32F)); // large kernel to ensure separation
    threshold(grabbedFrame, grabbedFrame, 254, 255, THRESH_BINARY);
    
    
//    footFrame = grabbedFrame(Rect(
//                             (grabbedFrame.cols - grabbedFrame.cols*activeAreaPc)*0.5,
//                             (grabbedFrame.rows - grabbedFrame.rows*activeAreaPc)*0.5,
//                                  grabbedFrame.cols*activeAreaPc,
//                                  grabbedFrame.rows*activeAreaPc
//                             ));
    footFrame = grabbedFrame.clone();
    
    findContours(footFrame.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    vector<vector<Point> >::iterator itc = contours.begin();
    while(itc != contours.end())
    {
        if(itc->size() < minFootSize || itc->size() > maxFootSize)
        {
            itc = contours.erase(itc);
        }
        else {
            ++itc;
        }
    }
    
//    footFrame.convertTo(footFrame, CV_32F);
    cvtColor(footFrame, footFrame, CV_GRAY2BGR);
     drawContours(footFrame, contours, -1, Scalar(255,0,0),5);
    imshow("hisf", footFrame);
   
    
    // now rid of most noise, narrowing down the selection

    
}

void footDetect::initGetFeet()
{
    setLearningRate(0.0001);
    // use masked foreground to extract two feet
}

void footDetect::getFeet()
{
    // use histogram back projection to extract two feet areas
}
