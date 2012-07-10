#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofBackground(40, 100, 40);
    ofSetFrameRate(60);         // set to 60fps (use as base for serial read)
    
    ofSetLogLevel(OF_LOG_VERBOSE /*OF_LOG_FATAL_ERROR*/);
    
    font.loadFont("DIN.otf",64); // located in /bin/data for now
    
        
    // OSC - open an outgoing connection
    sender.setup(HOST, PORT);
    
    // PHIDGET
       phidget.connect(1000);
    vector<string> bridgeNo;
    for(int i=0; i<4; i++){
        string temp = ofToString(i);
        bridgeNo.push_back(temp);
    }
    
    // SERIAL
    startSerial = false;
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
    
    gui = new ofxUICanvas(100,100,320,600); //ofxUICanvas(float x, float y, float width, float height)	
	gui->setFont("DIN.otf");
    gui->addWidgetDown(new ofxUILabel("OFXUI TUTORIAL", OFX_UI_FONT_LARGE)); 
    gui->addWidgetDown(new ofxUISpacer(300, 2));
    gui->addWidgetDown(new ofxUISlider(304,16,0.0,255.0,100.0,"BACKGROUND VALUE")); 
    gui->addWidgetDown(new ofxUIToggle(32, 32, false, "FULLSCREEN"));
    
 
    current = "Choose Serial Device";
    lbl = new ofxUILabel("HI THERE", OFX_UI_FONT_MEDIUM); // rename this- essential

//    gui->addWidgetDown(new ofxUILabel("TO USE IN LC CALIBRATION (TO DO)", OFX_UI_FONT_MEDIUM));
//    txt = new ofxUITextInput(320, "TEXT INPUT", "Input Text", OFX_UI_FONT_LARGE);
//    txt->setAutoClear(false); // turn off autoclear
//    gui->addWidgetDown(txt);
    
    gui->addWidgetDown(new ofxUILabel("PHIDGETS CALIBRATION", OFX_UI_FONT_LARGE));
    gui->addWidgetDown(new ofxUISpacer(300, 2)); 
    
    gui->addWidgetDown(new ofxUIRadio( 16, 16, "BRIDGE NUMBER", bridgeNo, OFX_UI_ORIENTATION_HORIZONTAL)); 

    gui->addWidgetDown(new ofxUILabel("Calibration Value 1", OFX_UI_FONT_MEDIUM));
    tval1 = new ofxUITextInput(300, "VALUE 1", "Value 1", OFX_UI_FONT_LARGE);
    tval1->setAutoClear(false);
    gui->addWidgetDown(tval1);
    
    gui->addWidgetDown(new ofxUILabel("Calibration Value 2", OFX_UI_FONT_MEDIUM));
    tval2 = new ofxUITextInput(300, "VALUE 2", "Value 2", OFX_UI_FONT_LARGE);
    tval2->setAutoClear(false);
    gui->addWidgetDown(tval2);
    
    gui->addWidgetDown(new ofxUILabel("Hi HERE", OFX_UI_FONT_LARGE));
    
    // THIS MUST ALWAYS BE LAST
    gui->addWidgetDown(lbl);
    ddl = new ofxUIDropDownList(200, current, /*items*/ deviceLine, OFX_UI_FONT_MEDIUM);
    gui->addWidgetDown(ddl);
    
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent); 
    gui->loadSettings("GUI/guiSettings.xml"); 
    
    //

}

//--------------------------------------------------------------
void testApp::update(){
    
    lbl->setLabel(current);

    // PHIDGET READ
    if( phidget.isConnected ){
//        printf("(0): %f \n", phidget.calibValue(0));
//        printf("(1): %f \n", phidget.calibValue(1));
//        printf("(2): %f \n", phidget.calibValue(2));
//        printf("(3): %f \n", phidget.calibValue(3));
    }

    //SERIAL READ ( NEED TO IMPLEMENT DISCONNECT/RECONNECT CASE )
    if(startSerial){
//        string address;
//        address = current;
        serial.setup(current,9600);
        startSerial = false;
        serialConnected = true;
    }
    
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
    
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    
}

void testApp::guiEvent(ofxUIEventArgs &e){
    
    // get the name of the widget toggled
    string name = e.widget->getName(); 
    cout << "got event from: " << name << endl; 
//    current = name;
//    pointer = &current;
//    *pointer = name;
//    cout << "pointer adjusted:" << current << endl;
    // now need to be selective of event name.
    if(find(deviceLine.begin(), deviceLine.end(), name)!= deviceLine.end()){
//        cout << "it's there" <<endl;
        current = "/dev/" + name;   // mac specific
        startSerial = true;
    }
    
    if(e.widget->getName() == "BACKGROUND VALUE")	
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;    
        ofBackground(slider->getScaledValue());
    }   
    else if(e.widget->getName() == "FULLSCREEN")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        ofSetFullscreen(toggle->getValue()); 
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
    
}