#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    // SETUP WINDOW & LOAD DEPENDENCIES
    ofBackground(190, 190, 190);
    ofSetFrameRate(60);         // set to 60fps (use as base for serial read)
    ofSetLogLevel(OF_LOG_VERBOSE /*OF_LOG_FATAL_ERROR*/);
    font.loadFont("DIN.otf",64); // located in /bin/data for now
    
    // OSC - open an outgoing connection
    sender.setup(HOST, PORT);
    
    // PHIDGET
//    phidget.connect(1000);
    
    vector<string> bridgeNo;
    for(int i=0; i<4; i++){
        string temp = "Brdg " + ofToString(i);
        bridgeNo.push_back(temp);
    }
    
    // SERIAL
    serialConnected = false;
    
//    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    
//    vector<string> deviceLine;
    string tempDeviceLine;
    for(int i=0; i<deviceList.size();i++){
    tempDeviceLine = deviceList[i].getDeviceName();
        deviceLine.push_back(tempDeviceLine);
    }
    
//    serial.setup("/dev/tty.usbserial-A700eTJJ",9600); // mac osx // commenting out temporarily
    
    nTimesRead = 0;
    nBytesRead = 0;
    memset(bytesReadString, 0, 5);
    
    // ofxUI - setting up UI
    
    guiSetup = new ofxUICanvas(100,100,320,700); //ofxUICanvas(float x, float y, float width, float height)	
	guiSetup->setFont("DIN.otf");

    
 


//    gui->addWidgetDown(new ofxUILabel("TO USE IN LC CALIBRATION (TO DO)", OFX_UI_FONT_MEDIUM));
//    txt = new ofxUITextInput(320, "TEXT INPUT", "Input Text", OFX_UI_FONT_LARGE);
//    txt->setAutoClear(false); // turn off autoclear
//    gui->addWidgetDown(txt);
    
    // PHIDGET SECTION
    guiSetup->addWidgetDown(new ofxUILabel("PHIDGETS CALIBRATION", OFX_UI_FONT_LARGE));
    guiSetup->addWidgetDown(new ofxUISpacer(300, 2)); 
    guiSetup->addWidgetDown(new ofxUIToggle( 16, 16, false, "enable phidget")); 
    
    guiSetup->addWidgetDown(new ofxUIRadio( 16, 16, "BRIDGE NUMBER", bridgeNo, OFX_UI_ORIENTATION_HORIZONTAL)); 
    guiSetup->addWidgetDown(new ofxUILabel("Bridge Value", OFX_UI_FONT_MEDIUM));
    guiSetup->addWidgetDown(new ofxUILabel("Raw Bridge Value", OFX_UI_FONT_SMALL));

    guiSetup->addWidgetDown(new ofxUILabel("Calibration Value 1", OFX_UI_FONT_MEDIUM));
    calibLabel1 = new ofxUITextInput(300, "VALUE 1", "Value 1", OFX_UI_FONT_LARGE);
    calibLabel1->setAutoClear(false);
    guiSetup->addWidgetDown(calibLabel1);
    
    guiSetup->addWidgetDown(new ofxUILabel("Calibration Value 2", OFX_UI_FONT_MEDIUM));
    calibLabel2 = new ofxUITextInput(300, "VALUE 2", "Value 2", OFX_UI_FONT_LARGE);
    calibLabel2->setAutoClear(false);
    guiSetup->addWidgetDown(calibLabel2);
 
    guiSetup->addWidgetDown(new ofxUILabelButton(false, "Calibrate", OFX_UI_FONT_MEDIUM));
    guiSetup->addWidgetDown(new ofxUILabel("Calibrated Value", OFX_UI_FONT_MEDIUM));
    guiSetup->addWidgetDown(new ofxUILabel("Calibrated Bridge Value", OFX_UI_FONT_SMALL));

    
    
    // SERIAL SECTION
    guiSetup->addWidgetDown(new ofxUILabel("SERIAL CONNECTION", OFX_UI_FONT_LARGE));
    guiSetup->addWidgetDown(new ofxUISpacer(300, 2)); 
    guiSetup->addWidgetDown(new ofxUIToggle( 16, 16, false, "enable serial")); 
    
    current = "Choose Serial Device";
    serialValueLabel = new ofxUILabel("HI THERE", OFX_UI_FONT_MEDIUM); // rename this- essential
    guiSetup->addWidgetDown(serialValueLabel);
    ddl = new ofxUIDropDownList(200, current, /*items*/ deviceLine, OFX_UI_FONT_MEDIUM);
    guiSetup->addWidgetDown(ddl);
    
    ofAddListener(guiSetup->newGUIEvent, this, &testApp::guiEvent); 
//    guiSetup->loadSettings("GUI/guiSettings.xml"); // temporary commenting out
    
    //

}

//--------------------------------------------------------------
void testApp::update(){
    
    value = value++;
    
    serialValueLabel->setLabel(current);
    if (phidget.isConnected)
    val->setLabel(ofToString(*currentBridgeValue));
    
    // PHIDGET READ
    if( phidget.isConnected ){
//        printf("(0): %f \n", phidget.calibValue(0));
//        printf("(1): %f \n", phidget.calibValue(1));
//        printf("(2): %f \n", phidget.calibValue(2));
//        printf("(3): %f \n", phidget.calibValue(3));
    }

    //SERIAL READ ( NEED TO IMPLEMENT DISCONNECT/RECONNECT CASE )

    
    if(ofGetFrameNum() % 10 == 0 && serialConnected){
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
    inputSerial.setAddress("/input/serial");
    inputSerial.addFloatArg(rawSerial);
    sender.sendMessage(inputSerial);
}

//--------------------------------------------------------------
void testApp::draw(){

    ofSetColor(220);
    string msg;
    msg = "Serial:" + ofToString(bytesReadString);
    
    font.drawString(msg,50,100);
    
    // drawing a test 'handle'
    int width =ofGetWidth()/2;
    int height = ofGetHeight()/2;
    glPushMatrix();
    glTranslatef(width, height, 0);
    glRotatef(ofToFloat(bytesReadString),0,0,1);
    ofRectangle rect;
    rect.x = -150;
    rect.y = -25;
    rect.width = 300;
    rect.height = 50;
    ofRect(rect);
    glPopMatrix();
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    
    // get the name of the widget toggled
    string name = e.widget->getName(); 
    
//    cout << "got event from: " << name << endl; // debugging
    //    current = name;
    //    pointer = &current;
    //    *pointer = name;
    //    cout << "pointer adjusted:" << current << endl;
    // now need to be selective of event name.
    
    // ENABLE/DISABLE DEVICES
    if(name == "enable phidget"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;

        if(toggle->getValue()){
            cout << "trying to connect to phidget" << endl;
            phidget.connect(1000);
            
        }
        else {
            cout << "disable phidget -> implement this" << endl;
        }
    }

    else if(name == "enable serial"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        
        if(toggle->getValue()){
            cout << "serial enabled" << endl;
            if(current != "Choose Serial Device"){
                serial.setup(current,9600);
                serialConnected = true;
            }
        }
        else {
            cout << "serial disabled" << endl;
            serialConnected = false;
            serial.close();
        }
    }
    
    /////////////////////////////////////////
    
    if(find(deviceLine.begin(), deviceLine.end(), name)!= deviceLine.end()){
        //        cout << "it's there" <<endl;
        current = "/dev/" + name;   // mac specific
    }

    // values of phidget calibration goes here
    else if(name == "VALUE 1")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget; 
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: ";
        }       
        string output = textinput->getTextString(); 
        double test = 2;
        
        phidget.cBridge0.actValue1 = ofToFloat(output); // set actualvalue
        phidget.cBridge0.rawValue1 = phidget.getValue(0); // get value at point in time
        
        cout << output << endl; 
    }
    else if(name == "VALUE 2")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget; 
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: "; 
        }       
        string output = textinput->getTextString(); 
        cout << output << endl; 
    }
    if(name == "Brdg 0"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(phidget.cBridge0.actValue1));
        calibLabel2->setTextString(ofToString(phidget.cBridge0.actValue2));
        
        double ho[2] = {1,2};
        if(phidget.isConnected){
            currentBridgeValue = &ho[0];
            double *temp;
            temp = phidget.getValues();
            currentBridgeValue = &temp[0];
        }
        else {
            
        } 
        
    }
    else if(name == "Brdg 1"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(phidget.cBridge1.actValue1));
        calibLabel2->setTextString(ofToString(phidget.cBridge1.actValue2));
        if(phidget.isConnected){
            double *temp;
            temp = phidget.getValues();
            currentBridgeValue = &temp[1];
        }
        else {
            
        }
    }
    else if(name == "Brdg 2"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(phidget.cBridge2.actValue1));
        calibLabel2->setTextString(ofToString(phidget.cBridge2.actValue2));
        if(phidget.isConnected){
            double *temp;
            temp = phidget.getValues();
            currentBridgeValue = &temp[2];
        }
        else {
            
        }
    }
    else if(name == "Brdg 3"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(phidget.cBridge3.actValue1));
        calibLabel2->setTextString(ofToString(phidget.cBridge3.actValue2));
        if(phidget.isConnected){
            double *temp;
            temp = phidget.getValues();
            currentBridgeValue = &temp[3];
        }
        else {
            
        }
    }
    
    if (name == "calibrate"){
        // update actual values
        phidget.cBridge0.actValue1 = ofToFloat(calibLabel1->getTextString());
        phidget.cBridge0.actValue2 = ofToFloat(calibLabel2->getTextString());
    }
    
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

//------------------ofxUI stuff here----------------------------
void testApp::exit(){
    
    guiSetup->saveSettings("GUI/guiSettings.xml");
    delete guiSetup;
    
}
