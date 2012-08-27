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

void footDetect::grabForeground()
{
    foreground.copyTo(grabbedFrame);
//    findFeet(); // temporary
    imshow("snapshot", grabbedFrame);
}

void footDetect::findFeet(Mat inputMatrix, Mat& outputMatrix)
{
    // find from grabbedFrame (temporary?)
    morphologyEx(grabbedFrame, grabbedFrame, MORPH_ERODE, Mat::ones(10,10,CV_32F)); // large kernel to ensure separation
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
        
        double eccentricity = sqrt(lbd2/lbd1);
        
        Point centroidPt(mom.m10/mom.m00, mom.m01/mom.m00);
    
        
//        Vec3f centroidData = footFrame.at<Vec3f>(mom.m10/mom.m00, mom.m01/mom.m00);
        
//        double b,g,r;
//        b = centroidData[0];
//        g = centroidData[1];
//        r = centroidData[2];
        
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
        // filter out by principal axis length
        if(max(a1,a2) < 50)
        {
            itc = contours.erase(itc);
//            cout << max(a1,a2) << " didn't make it" << endl;
        }
        // filter out by eccentricity
        else if(eccentricity < 2)
        {
            itc = contours.erase(itc);
        }
    
        
        // ensure that centroid exists within the object's bounds
        
        
        else {
            cout << max(a1,a2) << " made it! with angle: " << theta << endl;
            cout << "eccentricity: " << eccentricity << endl;
//            cout << "b:" << b << "g:" << g << "r:" << r << endl;
            // save the centroid point for checking later ?
            ++itc;
        }
    }
    
    footFrame.operator=(Scalar(0)); // wipe before drawing feet
     drawContours(footFrame, contours, -1, Scalar(255),-1);
//    imshow("Feet Found", footFrame);

//    foreground.copyTo(outputMatrix);
//    Mat mask(inputMatrix.size(), CV_8UC1);
    inputMatrix.copyTo(outputMatrix, footFrame);

    
}

MatND footDetect::getHueHistogram(Mat& inputMatrix)
{
    // get hue 1D histogram
    MatND histogram;
    Mat hsv, hue;
    cvtColor(inputMatrix, hsv, CV_BGR2HSV);

    hue.create( hsv.size(), hsv.depth() );
    int ch[] = { 0, 0 };
    mixChannels( &hsv, 1, &hue, 1, ch, 1 );
    
    float thresh; // for backprojection
    int minSaturation;
    int histSize;
    histSize = 256; // the number of bins
    
    float hue_range[] = { 0, 180 };
    const float* ranges = { hue_range };

    Mat mask;
    if (minSaturation > 0) 
    {
        vector<Mat> v;
        split(hsv, v);
        threshold(v[1], mask, minSaturation, 255, THRESH_BINARY);
    }
    
    calcHist(&hue, 1, 0, Mat(), histogram, 1, &histSize, &ranges);
    normalize(histogram, histogram);
    
    // calculate the back projection
    Mat backProjection;
    calcBackProject(&hue, 1, 0, histogram, backProjection, &ranges);
    
    if(thresh > 0.0)
        threshold(backProjection, backProjection, thresh, 255, THRESH_BINARY);
    
    return histogram;
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
