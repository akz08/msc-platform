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
    Mat mask(inputMatrix.size(), CV_8U);
    inputMatrix.copyTo(outputMatrix, mask);
}

bool footDetect::grabForeground()
{
    char key = waitKey(10);
    if (key == 99)
    {
        foreground.copyTo(grabbedImage);
        imshow("snapshot", grabbedImage);
        findFeet();
        return  true;
    }
    
    return false;
}

void footDetect::findFeet()
{
    // find from grabbedImage (temporary?)
    morphologyEx(grabbedImage, grabbedImage, MORPH_OPEN, Mat());
    threshold(grabbedImage, grabbedImage, 254, 255, THRESH_BINARY);
    cout << "did find feet"<<endl;
    // get all contours
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
