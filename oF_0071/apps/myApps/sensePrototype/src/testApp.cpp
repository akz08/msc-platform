#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofBackground(40, 100, 40);
    ofSetFrameRate(60);         // set to 60fps (use as base for serial read)
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    font.loadFont("DIN.otf",64); // located in /bin/data for now
    
    // OSC - open an outgoing connection
    sender.setup(HOST, PORT);
    
    
    // SERIAL
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    
    //serial.setup(0,9600); // open the first device
    serial.setup("/dev/tty.usbserial-A700eTJJ",9600); // mac osx
    
    nTimesRead = 0;
    nBytesRead = 0;
    memset(bytesReadString, 0, 5);
}

//--------------------------------------------------------------
void testApp::update(){

    //SERIAL READ
    if(ofGetFrameNum() % 10 == 0){
        int nRead = 0; // temp var to keep count per read
        
        serial.writeByte('a');
        
        unsigned char bytesReturned[4];
        
        memset(bytesReadString, 0, 5);
        memset(bytesReturned, 0, 4);
        
        while( (nRead = serial.readBytes(bytesReturned,4)) > 0){
            nTimesRead++;
            nBytesRead = nRead;
        };
        
        memcpy(bytesReadString, bytesReturned, 4);
    }
 
    //SEND OSC
    float rawSerial;
    rawSerial = ofToFloat(bytesReadString);
    
    ofxOscMessage inputSerial;
    inputSerial.setAddress("/input/raw/serial");
    inputSerial.addFloatArg(rawSerial);
    sender.sendMessage(inputSerial);
}

//--------------------------------------------------------------
void testApp::draw(){

    ofSetColor(220);
    string msg;
    msg = "Serial:" + ofToString(bytesReadString);
    
    font.drawString(msg,50,100);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}