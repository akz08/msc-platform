#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofGraphics.h"
#include "ofxUI.h"
#include "ioPhidget.h"


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
    
    // ofxUI initialisation
    void setGUISetup();
    ofxUICanvas *guiSetup;
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    string current;
//    string *pointer;
    vector<string> deviceLine;
    ofxUIDropDownList *ddl;
    ofxUILabel *serialValueLabel;
    ofxUILabel *bridgeValueLabel;
    ofxUILabel *bridgeCalibValueLabel;
    ofxUILabel *val;
    ofxUITextInput *txt;
    ofxUITextInput *calibLabel1;
    ofxUITextInput *calibLabel2;
    
    bool enableSaving;
    
    double x;
    double y;
};
