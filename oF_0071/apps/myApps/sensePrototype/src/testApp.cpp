#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    // SETUP WINDOW & LOAD DEPENDENCIES
    ofBackground(113, 198, 113);
    ofSetFrameRate(60);                                             // set to 60fps (use as base for serial read)
    ofSetLogLevel(OF_LOG_VERBOSE /*OF_LOG_FATAL_ERROR*/);
    font.loadFont("DIN.otf",64);                                    // located in /bin/data for now
    
    // OSC 
    // open an outgoing connection
    sender.setup(HOST, PORT);
    // open incoming connection for testing
    receiver.setup(PORT);
    
    // PHIDGET
    for(int i=0; i<4; i++){                                         // creating a list for number of bridges (4)
        string temp = "Brdg " + ofToString(i);
        bridgeNo.push_back(temp);
    }
    currentBridge = 0;                                              // initialise bridge number
    
    // SERIAL
    serialConnected = false;
//    serial.listDevices();                                           // prints device list to console
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    string tempDeviceLine;
    for(int i=0; i<deviceList.size();i++){
    tempDeviceLine = deviceList[i].getDeviceName();
        deviceLine.push_back(tempDeviceLine);
    }
    nTimesRead = 0;
    nBytesRead = 0;
    memset(bytesReadString, 0, 5);
    
    // INITIALISE GUI
    setGUISetup();
    setGUIPlatform();
    
    // SAVE/LOAD
    enableSaving = false;
    loadValues("Variables/loadCells.xml", bridgeValuesArray);
    
    // DRAWING
    xPlatform = 960;
    yPlatform = 100;
    
    platformBaseWidth = 230;
    platformBaseHeight = 250;
    
    simulateBalance = true;
    simulateBalanceX = 0;
    simulateBalanceY = 0;

}

//--------------------------------------------------------------
void testApp::update(){

    // TESTING "BALANCE BOARD" POINT
//    yPlatform = (bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[2].currentCalibratedValue)/(bridgeValuesArray[1].currentCalibratedValue + bridgeValuesArray[3].currentCalibratedValue+bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[2].currentCalibratedValue);
//    xPlatform = (bridgeValuesArray[1].currentCalibratedValue + bridgeValuesArray[3].currentCalibratedValue)/(bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[2].currentCalibratedValue+bridgeValuesArray[1].currentCalibratedValue + bridgeValuesArray[3].currentCalibratedValue);
//    
//    cout << "x = " << xPlatform << "y = " << yPlatform << endl;
    
    // SIMULATE BALANCE
    if(simulateBalance){
        
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            // check for mouse moved message
            if(m.getAddress() == "/3/xy"){
                simulateBalanceX = m.getArgAsFloat(0);
                simulateBalanceY = m.getArgAsFloat(1);
            }
            // check for mouse button message
            else if(m.getAddress() == "/mouse/button"){
           
            }
        }
        
    }
    
    // PHIDGET READ
    if (phidget.isConnected){
    
        currentBridgeValue = phidget.getValues()[currentBridge];
        bridgeValueLabel->setLabel(ofToString(currentBridgeValue));

        for (int i = 0; i < 4; i++){
            bridgeValuesArray[i].currentValue = phidget.getValues()[i];
            bridgeValuesArray[i].currentCalibratedValue = bridgeValuesArray[i].currentValue*bridgeValuesArray[i].slope + bridgeValuesArray[i].yIntercept;
        }
        if (bridgeValuesArray[currentBridge].calculated){
            bridgeCalibValueLabel->setLabel(ofToString(bridgeValuesArray[currentBridge].currentCalibratedValue));
        }
    }

    //SERIAL READ
    serialLabel->setLabel(currentSerial);
    if(serialConnected){
        serialValueLabel->setLabel(ofToString(bytesReadString));
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

    ofSetColor(200);

    ofRectangle scooterHandle;
    scooterHandle.x = -150; 
    scooterHandle.y = -12.5;
    scooterHandle.width = 300;
    scooterHandle.height = 25;
    
    ofRectangle platformBase;
    platformBase.x = -platformBaseWidth/2;
    platformBase.y = scooterHandle.height + 10; // 10 is a 'buffer'
    platformBase.width = platformBaseWidth;
    platformBase.height = platformBaseHeight;
    
    
    
    glPushMatrix();
        glTranslatef(xPlatform, yPlatform, 0); // the 'origin' is now the centre

        ofRect(platformBase);
        ofSetColor(255,0,0);
        ofCircle(simulateBalanceX*platformBaseWidth -platformBaseWidth/2, scooterHandle.height + 10 + simulateBalanceY*platformBaseHeight, 5);
    
        glPushMatrix();
            ofSetColor(220);
            glRotatef(ofToFloat(bytesReadString),0,0,1);
            ofRect(scooterHandle);
        glPopMatrix();
    glPopMatrix();
    
}

//--------------------------------------------------------------
void testApp::saveValues(string fileName, calibrationValues bvArray[]){
    ofxXmlSettings *XML = new ofxXmlSettings();
    // saving the 4 data structures
    for(int i = 0; i<4; i++)
    {
        int index = XML->addTag("Bridge");
        if(XML->pushTag("Bridge", index))
        {
            XML->setValue("number", index);
            XML->setValue("rawValue1", bvArray[i].rawValue1);
            XML->setValue("rawValue2", bvArray[i].rawValue2);
            XML->setValue("actValue1", bvArray[i].actValue1);
            XML->setValue("actValue2", bvArray[i].actValue2);
            XML->setValue("slope", bvArray[i].slope);
            XML->setValue("yIntercept", bvArray[i].yIntercept);
            XML->setValue("currentValue", bvArray[i].currentValue);
            XML->setValue("currentCalibratedValue", bvArray[i].currentCalibratedValue);
            XML->setValue("calculated", bvArray[i].calculated);
        }
        XML->popTag();
    }
    XML->saveFile(fileName);
    delete XML;
}
void testApp::loadValues(string fileName, calibrationValues bvArray[]){
    ofxXmlSettings *XML = new ofxXmlSettings();
    XML->loadFile(fileName);
    int index = XML->getNumTags("Bridge");
    for(int i = 0; i < index; i++)
    {
        XML->pushTag("Bridge", i);
        bvArray[i].rawValue1 = XML->getValue("rawValue1",0.0);
        bvArray[i].rawValue2 = XML->getValue("rawValue2",0.0);
        bvArray[i].actValue1 = XML->getValue("actValue1",0.0);
        bvArray[i].actValue2 = XML->getValue("actValue2",0.0);
        bvArray[i].slope = XML->getValue("slope",0.0);
        bvArray[i].yIntercept = XML->getValue("yIntercept",0.0);
        bvArray[i].currentValue = XML->getValue("currentValue",0.0);
        bvArray[i].currentCalibratedValue = XML->getValue("currentCalibratedValue",0.0);
        bvArray[i].calculated = XML->getValue("rawValue1",false);
        XML->popTag();
    }
    delete XML;
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
    eventGUISetup(e);
    eventGUIPlatform(e);
}

void testApp::eventGUISetup(ofxUIEventArgs &e){
    
    // get the name of the widget toggled (debugging)
    string name = e.widget->getName(); 
    
//    cout << "got event from: " << name << endl; // debugging
    //    current = name;
    //    pointer = &current;
    //    *pointer = name;
    //    cout << "pointer adjusted:" << current << endl;
    // now need to be selective of event name.
    
    if(name == "enable saving"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue()){
            enableSaving = true;
        }
        else enableSaving = false;
    }
    
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
            if(currentSerial != "Choose Serial Device"){
                serial.setup(currentSerial,9600);
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
        currentSerial = "/dev/" + name;   // mac specific
    }

    // values of phidget calibration goes here
    else if(name == "VALUE 1")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget; 
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: ";
        }       
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "UNFOCUSSED 2";
            bridgeValuesArray[currentBridge].actValue1 = ofToFloat(textinput->getTextString());
            bridgeValuesArray[currentBridge].rawValue1 = phidget.getValues()[currentBridge];
        }
        string output = textinput->getTextString(); 
        double test = 2;
        
//        phidget.cBridge0.actValue1 = ofToFloat(output); // set actualvalue
//        phidget.cBridge0.rawValue1 = phidget.getValue(0); // get value at point in time
        
        cout << output << endl; 
    }
    else if(name == "VALUE 2")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget; 
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: "; 
        }       
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "UNFOCUSSED 2";
            bridgeValuesArray[currentBridge].actValue2 = ofToFloat(textinput->getTextString());
            bridgeValuesArray[currentBridge].rawValue2 = phidget.getValues()[currentBridge];
        }
        string output = textinput->getTextString(); 
        cout << output << endl; 
    }
    
    if (name == "Calibrate"){
        
        bridgeValuesArray[currentBridge].slope = (bridgeValuesArray[currentBridge].actValue2 - bridgeValuesArray[currentBridge].actValue1) / (bridgeValuesArray[currentBridge].rawValue2 - bridgeValuesArray[currentBridge].rawValue1);
        
        bridgeValuesArray[currentBridge].yIntercept = bridgeValuesArray[currentBridge].actValue1 - (bridgeValuesArray[currentBridge].rawValue1 * bridgeValuesArray[currentBridge].slope);
        
        bridgeValuesArray[currentBridge].calculated = true;

    }
    
    if(name == "Brdg 0"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(bridgeValuesArray[0].actValue1));
        calibLabel2->setTextString(ofToString(bridgeValuesArray[0].actValue2));
//        calibLabel1->setTextString(ofToString(phidget.cBridge0.actValue1));
//        calibLabel2->setTextString(ofToString(phidget.cBridge0.actValue2));
        
        currentBridge = 0;
        
//        double ho[2] = {1,2};
//        if(phidget.isConnected){
//            currentBridgeValue = &ho[0];
//            double *temp;
//            temp = phidget.getValues();
//            currentBridgeValue = &temp[0];
            
//            currentBridgeValue = &phidget.getValues()[0];
//            currentBridgeValue = phidget.getValue(0);
//            currentBridgeValue = &b0.currentValue;
            
            
//            cout << phidget.getValues()[0] << endl;
//        }
//        else {
//            
//        } 
        
    }
    else if(name == "Brdg 1"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(bridgeValuesArray[1].actValue1));
        calibLabel2->setTextString(ofToString(bridgeValuesArray[1].actValue2));

        currentBridge = 1;
//        if(phidget.isConnected){
//            
//        }
//        else {
//            
//        }
    }
    else if(name == "Brdg 2"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(bridgeValuesArray[2].actValue1));
        calibLabel2->setTextString(ofToString(bridgeValuesArray[2].actValue2));

        currentBridge = 2;
//        if(phidget.isConnected){
//            
//        }
//        else {
//            
//        }
    }
    else if(name == "Brdg 3"){
        // load up the calibration values
        calibLabel1->setTextString(ofToString(bridgeValuesArray[3].actValue1));
        calibLabel2->setTextString(ofToString(bridgeValuesArray[3].actValue2));

        currentBridge = 3;
//        if(phidget.isConnected){
//            
//        }
//        else {
//            
//        }
    }
    

    
}

void testApp::eventGUIPlatform(ofxUIEventArgs &e){
    string name = e.widget->getName(); 

    if(name == "xPlatform")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        xPlatform = slider->getScaledValue(); 
	}
    if(name == "yPlatform")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        yPlatform = slider->getScaledValue(); 
	}
    if(name == "platformBaseWidth")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        platformBaseWidth = slider->getScaledValue(); 
	}
    if(name == "platformBaseHeight")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        platformBaseHeight = slider->getScaledValue(); 
	}
}

void testApp::setGUIPlatform(){
    guiPlatform = new ofxUICanvas(340, 10, 320, ofGetHeight()-20);
    guiPlatform->setFont("DIN.otf");
    
    guiPlatform->addWidgetDown(new ofxUILabel("PLATFORM VISUAL", OFX_UI_FONT_LARGE));
    guiPlatform->addWidgetDown(new ofxUISpacer(300, 2)); 
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, ofGetWidth(), xPlatform, "xPlatform")); 
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, ofGetHeight(), yPlatform, "yPlatform")); 
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, 500, platformBaseWidth, "platformBaseWidth")); 
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, 500, platformBaseHeight, "platformBaseHeight"));
    ofAddListener(guiPlatform->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::setGUISetup(){
    // ofxUI - setting up UI
    
    guiSetup = new ofxUICanvas(10,10,320,ofGetHeight()-20); //ofxUICanvas(float x, float y, float width, float height)	
	guiSetup->setFont("DIN.otf");
    
    // PHIDGET SECTION
    guiSetup->addWidgetDown(new ofxUILabel("PHIDGETS CALIBRATION", OFX_UI_FONT_LARGE));
    guiSetup->addWidgetDown(new ofxUISpacer(300, 2));     
    guiSetup->addWidgetDown(new ofxUIToggle(16, 16, false, "enable saving")); 
    guiSetup->addWidgetDown(new ofxUISpacer(300, 0.5)); 
    guiSetup->addWidgetDown(new ofxUIToggle( 16, 16, false, "enable phidget")); 
    
    guiSetup->addWidgetDown(new ofxUIRadio( 16, 16, "BRIDGE NUMBER", bridgeNo, OFX_UI_ORIENTATION_HORIZONTAL)); 
    guiSetup->addWidgetDown(new ofxUILabel("Bridge Value", OFX_UI_FONT_MEDIUM));
    bridgeValueLabel = new ofxUILabel("Raw Bridge Value", OFX_UI_FONT_SMALL);
    guiSetup->addWidgetDown(bridgeValueLabel);
    
    guiSetup->addWidgetDown(new ofxUISpacer(300, 0.5)); 
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
    bridgeCalibValueLabel = new ofxUILabel("Calibrated Bridge Value", OFX_UI_FONT_SMALL);
    guiSetup->addWidgetDown(bridgeCalibValueLabel);
    
    
    // SERIAL SECTION
    guiSetup->addWidgetDown(new ofxUILabel("SERIAL CONNECTION", OFX_UI_FONT_LARGE));
    guiSetup->addWidgetDown(new ofxUISpacer(300, 2)); 
    guiSetup->addWidgetDown(new ofxUIToggle( 16, 16, false, "enable serial")); 
    
    currentSerial = "Choose Serial Device";
    serialLabel = new ofxUILabel("CHOOSE THE DEVICE", OFX_UI_FONT_MEDIUM); // rename this- essential
    guiSetup->addWidgetDown(serialLabel);
    serialDropdown = new ofxUIDropDownList(200, currentSerial, /*items*/ deviceLine, OFX_UI_FONT_MEDIUM);
    //    serialDropdown->setDrawPadding(true);
    guiSetup->addWidgetDown(serialDropdown);
    guiSetup->addWidgetDown(new ofxUILabel("Serial Value", OFX_UI_FONT_MEDIUM));
    serialValueLabel = new ofxUILabel("Raw Serial Value", OFX_UI_FONT_SMALL);
    guiSetup->addWidgetDown(serialValueLabel);
    
    ofAddListener(guiSetup->newGUIEvent, this, &testApp::guiEvent); 
    
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

//--------------------------------------------------------------
void testApp::exit(){
    
    if(enableSaving){
        saveValues("Variables/loadCells.xml", bridgeValuesArray);}

    delete guiSetup;
    
}


