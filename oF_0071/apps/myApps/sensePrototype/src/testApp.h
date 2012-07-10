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
    
    // OSC Sender
    ofxOscSender sender;
    
    // Font
    ofTrueTypeFont font;
    
    // Phidget
    ioPhidget phidget;
    struct bridgeCalib{
        double Value1;
        double Value2;
        double gradient;
        double Yintr;
    }b0,b1,b2,b3;
    
    // Serial Input
    ofSerial serial;
    char    bytesRead[4];           // reading 4 bytes from serial
    char    bytesReadString[5];     // null terminator needed
    int     nBytesRead;
    int     nTimesRead;
    bool    startSerial;
    bool    serialConnected;
    
    // ofxUI initialisation
    ofxUICanvas *gui;
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    string current;
//    string *pointer;
    vector<string> deviceLine;
    ofxUIDropDownList *ddl;
    ofxUILabel *lbl;
    ofxUITextInput *txt;
    ofxUITextInput *tval1;
    ofxUITextInput *tval2;
    
};
