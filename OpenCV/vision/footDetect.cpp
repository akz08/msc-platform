  #include "footDetect.h"
#include "math.h"

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
    char key = waitKey(50);
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
    morphologyEx(grabbedFrame, grabbedFrame, MORPH_OPEN, Mat::ones(6,6,CV_32F)); // large kernel to ensure separation
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
        // compute moments
        Moments mom = moments(*itc);
        // calculate angle of principal axis
//        double alpha = 0.5 * atan( (2*mom.mu11)/(mom.mu20 - mom.mu02) );
//        alpha = alpha * 180 / 3.14159265359;
//        cout << "calculated" << alpha << endl;
//        circle(footFrame, Point(mom.m10/mom.m00, mom.m01/mom.m00),2,Scalar(0),2);
        // calculate semi-major axis a
//        double a1 = 2*(mom.nu02 + mom.nu20 + sqrt(pow((mom.nu20-mom.nu02), 2) + 4*pow(mom.nu11, 2)));
//        a1 = sqrt(a1);
        
//        double a2 = 2*(mom.nu02 + mom.nu20 + sqrt(pow((mom.nu20-mom.nu02), 2) + 4*pow(mom.nu11, 2)));
//        a2 = sqrt(a2);
        
//        double a1 = pow(((16.0*pow(mom.mu20, 3))/(3.12*mom.mu02)),0.125);
//        double a2 = pow(((16.0*pow(mom.mu02, 3))/(3.12*mom.mu20)),0.125);

        double lbd1 = 0.5*(mom.mu20 + mom.mu02) - 0.5*sqrt(pow(mom.mu20, 2) + pow(mom.mu02, 2) - 2*mom.mu02*mom.mu20 + 4*pow(mom.mu11, 2));
        
        double lbd2 = 0.5*(mom.mu20 + mom.mu02) + 0.5*sqrt(pow(mom.mu20, 2) + pow(mom.mu02, 2) - 2*mom.mu02*mom.mu20 + 4*pow(mom.mu11, 2));
        
        double a1 = sqrt(lbd1/mom.m00);
        double a2 = sqrt(lbd2/mom.m00);
        
        double theta = atan( (lbd2 - mom.mu20) / mom.mu11);
        theta = theta * (3.1415 / 180);
        
        cout << "semi-major axis length: " << a1 << "," << a2 << endl;
        
        // filter out by rough size
//        if(itc->size() < minFootSize || itc->size() > maxFootSize)
//        {
//            itc = contours.erase(itc);
//        }
        // filter out by principal angle
//        else if(abs(alpha) < 1.57079632679)
//        {
//            itc = contours.erase(itc);
//            cout<< "didn't make it" << alpha << endl;
//        }
        // filter out by axis length
        if(max(a1,a2) < 80)
        {
            itc = contours.erase(itc);
//            cout << max(a1,a2) << " didn't make it" << endl;
        }
        else {
            cout << max(a1,a2) << " made it! with angle: " << theta << endl;
            ++itc;
        }
    }
    
    footFrame.operator=(Scalar(255)); // wipe before drawing feet
     drawContours(footFrame, contours, -1, Scalar(0),-1);
    imshow("Feet Found", footFrame);


    
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
