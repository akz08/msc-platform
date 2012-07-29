#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofGraphics.h"
#include "ofxUI.h"
#include "ioPhidget.h"
#include "wiicpp.h"
//#include "ofThread.h"
//#include "boost/thread.hpp"  // to help support multithreading for the wiic connection
//#include <boost/date_time.hpp>  
#include "ioThreading.h"

// OSC definitions of host to connect to
#define HOST "localhost"
#define PORT 12000


class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg); 
    
    // duplicating struct structure from iophidget .. need to create a namespace maybe?
    struct calibrationValues {
        double rawValue1;
        double rawValue2;
        double actValue1;
        double actValue2;
        double slope;
        double yIntercept;
        double currentValue;
        double currentCalibratedValue;
        bool calculated;
    };
    
    calibrationValues bridgeValuesArray[4];
    
    // XML LOAD/SAVE VALUES
    void saveValues(string fileName, calibrationValues bvArray[]);
    void loadValues(string fileName, calibrationValues bvArray[]);
    
    // OSC Sender
    ofxOscSender sender;
    
    // Font
    ofTrueTypeFont font;
    
    // Phidget
    ioPhidget phidget;

//    bridgeValues bridgeValuesArray[4];
    
    vector<string> bridgeNo;
    int currentBridge;
    double currentBridgeValue;
    double value;
    double blank;
    
    // Serial Input
    ofSerial serial;
    char    bytesRead[4];           // reading 4 bytes from serial
    char    bytesReadString[5];     // null terminator needed
    int     nBytesRead;
    int     nTimesRead;
    bool    serialConnected;
    
    // Wii Remote/ Balance Board
    CWii wii;
    vector<CWiimote>::iterator i; // should probably review this to be more descriptive 
    int remotesFound;
    bool reloadRemotes;
    bool connectRemotes;
    ioThreading thread;
    struct balanceBoard {//total, topLeft, topRight, bottomLeft, bottomRight
        float total;
        float topLeft;
        float topRight;
        float bottomLeft;
        float bottomRight;
    }bBoard; 
    float total, topLeft, topRight, bottomLeft, bottomRight;
    bool connectedRemotes;
    int exType;
    std::vector<CWiimote> *pointerWiimotes;
    std::vector<CWiimote> referenceWiimotes;
    
    // ofxUI initialisation
    void setGUISetup();
    void setGUIPlatform();
    void eventGUISetup(ofxUIEventArgs &e);
    void eventGUIPlatform(ofxUIEventArgs &e);
    
    ofxUICanvas *guiSetup;
    ofxUICanvas *guiPlatform;
    
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    string currentSerial;
//    string *pointer;
    vector<string> deviceLine;
    ofxUIDropDownList *serialDropdown;
    ofxUILabel *serialValueLabel;
    ofxUILabel *serialLabel;
    ofxUILabel *bridgeValueLabel;
    ofxUILabel *bridgeCalibValueLabel;
    ofxUILabel *val;
    ofxUITextInput *txt;
    ofxUITextInput *calibLabel1;
    ofxUITextInput *calibLabel2;
    
    bool enableSaving;
    
    double xPlatform;
    double yPlatform;
    
    double platformBaseWidth;
    double platformBaseHeight;
    
    bool simulateBalance;
    ofxOscReceiver receiver;
    float simulateBalanceX;
    float simulateBalanceY;
    
    double testPlatformX;
    double testPlatformY;
};

