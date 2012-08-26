#include "testApp.h"
//#include "wiiRemoteInterf.h"

//testApp::testApp(void) 
//    : _impl (NULL)
//{ }
//
//void testApp::init(void)
//{
//    _impl = new MyClassImpl();
//}
//
//testApp::~testApp(void)
//{
//    if(_impl) {delete _impl; _impl = NULL; }
//}
//
//void testApp::doSomethingWithMyClass(void)
//{
//    _myValue = 42;
//    int result = _impl->doSomethingWith( _myValue);
////    int result = 42;
////    char* test = "hello";
////    if (result == cBLABLA)
////    {
////        _impl->logMyMessage(test/*"Hello,Arthur!"*/);
////    }
////    else
////    {
////        _impl->logMyMessage(test/*"Don't worry."*/);
////    }
//}

//--------------------------------------------------------------
void testApp::setup(){
    
//    doSomethingWithMyClass();
    
    // SETUP WINDOW & LOAD DEPENDENCIES
    ofBackground(250, 250, 250);
    ofSetFrameRate(60);                                             // set to 60fps (use as base for serial read)
    ofSetLogLevel(OF_LOG_VERBOSE /*OF_LOG_FATAL_ERROR*/);
    font.loadFont("DIN.otf",12);                                    // located in /bin/data for now
    
    // OSC 
    // open an outgoing connection
    sender.setup(HOST, PORT_SEND);
    // open incoming connection for testing... use a different port from sender
    receiver.setup(PORT_RECEIVE);
    
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
    
    // WII BALANCE BOARD (REMOTES)
//    reloadRemotes = 0;
    
    // INITIALISE GUI
    setGUISetup();
    setGUIPlatform();
    
    // SAVE/LOAD
    enableSaving = false;
    loadValues("Variables/loadCells.xml", bridgeValuesArray);
    
    // DRAWING
    enableVisualisation = true;
    xPlatform = 990;
    yPlatform = 400;
    
    platformBaseWidth = 230;
    platformBaseHeight = 250;
    scooterHandleLength = 200;
    
//    simulateBalance = false; // simulateBalance & wiiBalance bool activation now contained within toggles
    cogPlatformX = 0;
    cogPlatformY = 0;
    
//    wiiBalance = false;
    
//    thread.startThread(true,false);
//    boost::thread workerThread(workerFunction, bBoard);
//    workerThread.join(); 
}

//void testApp::workerFunction(struct balanceBoard){
//    cout << "workerFunction" << endl;
//}

//--------------------------------------------------------------
void testApp::update(){

//    thread.lock();
//    thread.threadedFunction();
//    thread.unlock();
/*
    // TESTING "BALANCE BOARD" POINT
//    testPlatformY = (bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[2].currentCalibratedValue)/(bridgeValuesArray[1].currentCalibratedValue + bridgeValuesArray[3].currentCalibratedValue+bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[2].currentCalibratedValue);
//    testPlatformX = (bridgeValuesArray[1].currentCalibratedValue + bridgeValuesArray[0].currentCalibratedValue)/(bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[2].currentCalibratedValue+bridgeValuesArray[1].currentCalibratedValue + bridgeValuesArray[3].currentCalibratedValue);
    
    testPlatformX = (bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[2].currentCalibratedValue)/(bridgeValuesArray[1].currentCalibratedValue + bridgeValuesArray[3].currentCalibratedValue);
    testPlatformY = (bridgeValuesArray[2].currentCalibratedValue + bridgeValuesArray[3].currentCalibratedValue)/(bridgeValuesArray[0].currentCalibratedValue + bridgeValuesArray[1].currentCalibratedValue);
//    
    cout << "x = " << testPlatformX << "y = " << testPlatformY << endl;
*/    
 
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
    if(serialConnected)
    {
        serialValueLabel->setLabel(ofToString(bytesReadString));
        
        if(ofGetFrameNum() % 10 == 0)
        {
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
    }
    
    // TRIAL FOOT PRESS
    if(footFound && !oscFootPressSim)
    {
        // do the opencv calls to get 0-1 reading on foot 'height'
    }
    else if(oscFootPressSim)
    {
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            if(m.getAddress() == "/1/fader1"){
                footPressL = m.getArgAsFloat(0);
            }
            else if(m.getAddress() == "/1/fader2"){
                footPressR = m.getArgAsFloat(0);
            }
        }
    }
    
    // to do: group the balance processing stuff
    // ACTUAL BALANCE
    if(phidget.isConnected && !oscBalanceSim && !wiibbBalanceSim)
    {
        // do the calculation of X-Y stuff here.........
        cout << "normal phidget vis" << endl;
        // do stuff to output cogPlatformX & Y
        // remembering that we are using the standard is -1 to +1
        // anticlockwise from top left standard (check circuit)
        double topLeft = bridgeValuesArray[0].currentCalibratedValue;
        double bottomLeft = bridgeValuesArray[1].currentCalibratedValue;
        double bottomRight = bridgeValuesArray[2].currentCalibratedValue;
        double topRight = bridgeValuesArray[3].currentCalibratedValue;
        
        // normalise the values to ||1|| . we divide the value by the users weight . temp set to 20kg
        double totalWeight = 20;
        topLeft = topLeft / totalWeight;
        bottomLeft = bottomLeft / totalWeight;
        bottomRight = bottomRight / totalWeight;
        topRight = topRight / totalWeight;
        
        cogPlatformX = (topRight + bottomRight) - (topLeft + bottomLeft); // correct ?
        cogPlatformY = (bottomLeft + bottomRight) - (topLeft + topRight); // correct ?
        
        cout << "platformX" << cogPlatformX << endl;
        cout << "platformY" << cogPlatformY << endl;
    }
    
    // SIMULATE BALANCE
    // touchOSC
    else if(oscBalanceSim){
        
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            // check for xy
            if(m.getAddress() == "/3/xy"){
                cogPlatformX = m.getArgAsFloat(0);
                cogPlatformX = cogPlatformX*2 - 1;

                cogPlatformY = m.getArgAsFloat(1);
                cogPlatformY = cogPlatformY*2 - 1;
            }
        }
        
    }
    // Wii Balance Board ^TM
    else if(wiibbBalanceSim){
        // get the OSC messages from OSCulator
        // OSCulator set to have 0-1 range for each loadcell
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            // check for mouse moved message
            if(m.getAddress() == "/wii/1/balance/0"){
                wiiBottomLeft = m.getArgAsFloat(0);
                cout << "wiiBottomLeft: "<< wiiBottomLeft <<endl;
            }
            else if(m.getAddress() == "/wii/1/balance/1"){
                wiiBottomRight = m.getArgAsFloat(0);
                cout<<"wiiBottomRight: "<<wiiBottomRight<<endl;
            }
            else if(m.getAddress() == "/wii/1/balance/2"){
                wiiTopLeft = m.getArgAsFloat(0);
                cout<<"wiiTopLeft: "<<wiiTopLeft<<endl;
            }
            else if(m.getAddress() == "/wii/1/balance/3"){
                wiiTopRight = m.getArgAsFloat(0);
                cout<<"wiiTopRight: "<<wiiTopRight<<endl;
            }
        }
        
        // NOTE : follow a standard similar to the wii balance board. 
        // i.e. BL[bridge 0], BR[bridge 1], TL[bridge 2], TR[bridge 3]
        // TO DO: include a setting to 'rotate' wii bB values for 'skateboard' orientation [switch when calculating testPlatformX & Y?]
        
        // processing the values into useful location information
        // temporarily using testPlatformX & Y
        
        // effectively the difference between the extremeties
        cogPlatformX = (wiiTopRight + wiiBottomRight) - (wiiTopLeft + wiiBottomLeft); // correct
        cogPlatformY = (wiiBottomLeft + wiiBottomRight) - (wiiTopLeft + wiiTopRight); // correct
        cout << "X: " << cogPlatformX <<endl; // centre on zero (both weights cancel out)
        cout << "Y: " << cogPlatformY <<endl; // centre on zero
        
        // X & Y values vary greatly from 0. how to 'normalise' the values without knowing a maximum?
    }
    
    
    
    //WII BALANCE BOARD READ
//    if(thread.boardReading){
     
//        thread.lock();
//        bBoard.total = thread.total;
//        bBoard.topLeft = thread.topLeft;
//        bBoard.topRight = thread.topRight;
//        bBoard.bottomLeft = thread.bottomLeft;
//        bBoard.bottomRight = thread.bottomRight;
//        
//        cout<<"transfarring"<<endl;
//        cout<< bBoard.total << endl;
//        cout<< bBoard.topLeft << endl;
//        thread.unlock();
        
//    }


    
    //SEND OSC
    float rawSerial;
    rawSerial = ofToFloat(bytesReadString);
    
    ofxOscMessage inputSerial;
    inputSerial.setAddress("/input/serial");
    inputSerial.addFloatArg(rawSerial);
    sender.sendMessage(inputSerial);
    
    ofxOscMessage inputBalance;
    inputBalance.setAddress("/input/balance");
    inputBalance.addFloatArg(cogPlatformX);
    inputBalance.addFloatArg(cogPlatformY);
    sender.sendMessage(inputBalance);
    
    ofxOscMessage inputFootPress;
    inputFootPress.setAddress("/input/footPress");
    inputFootPress.addFloatArg(footPressL);
    inputFootPress.addFloatArg(footPressR);
    sender.sendMessage(inputFootPress);
}

//--------------------------------------------------------------
void testApp::draw(){

    // PRINT RELEVANT NETWORK INFORMATION
    ofSetColor(0);
    string portInfo, hostInfo, inportInfo;
    inportInfo = "Port(Incoming: " + ofToString(PORT_RECEIVE);
    portInfo = "Port(Outgoing): " + ofToString(PORT_SEND);
    hostInfo = "Host: " + ofToString(HOST); 
    font.drawString(inportInfo, ofGetWidth() - 200, ofGetHeight() - 10);
    font.drawString(portInfo, ofGetWidth() - 200, ofGetHeight() - 30);
    font.drawString(hostInfo, ofGetWidth() - 200, ofGetHeight() - 50);
    
    // RENDER THE SCOOTER PLATFORM
    if(enableVisualisation){
        
        // setup scooter handle (actual full length of handle is 29cm. one foam handle is 11cm 
        ofRectangle scooterHandle; // to do: insert a scale. proper scaling for handle needed.
        scooterHandle.x = -scooterHandleLength/2; 
        scooterHandle.y = -11;
        scooterHandle.width = scooterHandleLength;
        scooterHandle.height = 22;

        // setup platform base (for CENTRE origin) (actual base is 44x51cm)
        ofRectangle platformBase;
        platformBase.x = - platformBaseWidth/2;
        platformBase.y = - platformBaseHeight/2; 
        platformBase.width = platformBaseWidth;
        platformBase.height = platformBaseHeight;        
                
        glPushMatrix();
            glTranslatef(xPlatform, yPlatform, 0); // the 'origin' is now the center
            ofSetColor(200); // set to grey or "gray"
            ofRect(platformBase);   // draw the base first
                   
            glPushMatrix();
                ofSetColor(90);    // set to black for dots
                // we use the convention that x & y ranges from -1 to 1 with a specified "buffer" value
                ofCircle(cogPlatformX*platformBaseWidth/2, cogPlatformY*platformBaseHeight/2, 4);
            glPopMatrix();

           // draw the handle & sort for rotation
            glPushMatrix();
                glTranslatef(0,-platformBaseHeight/2 - 20,0); // relative to centre of platform
                // drawing the base attachement...for fun
                ofSetColor(210);        
                ofRect(-20,-35,40,55);
                ofSetColor(205);
                ofRect(-15,-35,30,55);
                ofSetColor(212);
                ofRect(-11,-35,22,25);
                glRotatef(ofToFloat(bytesReadString),0,0,1);
                ofSetColor(215);
                ofRect(scooterHandle);
                ofSetColor(220);
                ofRect(-25,-13,50,26);
            glPopMatrix();
        
        glPopMatrix();
        
    }
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
        bvArray[i].calculated = XML->getValue("calculated",false);
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
    else if(name == "yPlatform")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        yPlatform = slider->getScaledValue(); 
	}
    else if(name == "platformBaseWidth")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        platformBaseWidth = slider->getScaledValue(); 
	}
    else if(name == "platformBaseHeight")
	{
		ofxUISlider *slider = (ofxUISlider *) e.widget; 
        platformBaseHeight = slider->getScaledValue(); 
	}
    else if(name == "scooterHandleLength")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget; 
        scooterHandleLength = slider->getScaledValue(); 
    }
    else if(name == "enable TouchOSC balance simulation")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue())
        {
            oscBalanceSim = true; // flip the value (since initially is false)
            wiibbBalanceSimToggle->setValue(false);
            wiibbBalanceSim = false; // hacky implementation, should do it better
        }
        else
        {
            oscBalanceSim = false;
        }
    }
    else if(name == "enable TouchOSC foot-press simulation")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue())
        {
            oscFootPressSim = true; // flip the value (since initially is false)
        }
        else
        {
            oscFootPressSim = false;
        }
    }
    else if(name == "enable Wii Balance Board")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue())
        {
            wiibbBalanceSim = true;
            oscBalanceSimToggle->setValue(false);
            oscBalanceSim = false;
        }
        else
        {
            wiibbBalanceSim = false;
        }
    }
    else if(name == "enable visualisation")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if(toggle->getValue())
        {
            enableVisualisation = true;
        }
        else
        {
            enableVisualisation = false;
        }
    }
}

void testApp::setGUIPlatform(){
    guiPlatform = new ofxUICanvas(340, 10, 320, ofGetHeight()-20);
    guiPlatform->setFont("DIN.otf");
    
    guiPlatform->addWidgetDown(new ofxUILabel("PLATFORM VISUAL", OFX_UI_FONT_LARGE));
    guiPlatform->addWidgetDown(new ofxUISpacer(300, 2)); 
    guiPlatform->addWidgetDown(new ofxUIToggle(16, 16, true, "enable visualisation"));
    guiPlatform->addWidgetDown(new ofxUILabel("Balance Simulation", OFX_UI_FONT_MEDIUM));
    oscBalanceSimToggle = new ofxUIToggle(16, 16, false, "enable TouchOSC balance simulation");
    guiPlatform->addWidgetDown(oscBalanceSimToggle); 
    wiibbBalanceSimToggle = new ofxUIToggle(16, 16, false, "enable Wii Balance Board");
    guiPlatform->addWidgetDown(wiibbBalanceSimToggle); 
    guiPlatform->addWidgetDown(new ofxUILabel("Foot-press Simulation", OFX_UI_FONT_MEDIUM));
    oscFootPressSimToggle = new ofxUIToggle(16, 16, false, "enable TouchOSC foot-press simulation");
    guiPlatform->addWidgetDown(oscFootPressSimToggle);
    
    guiPlatform->addWidgetDown(new ofxUILabel("Platform Parameters", OFX_UI_FONT_MEDIUM));
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, ofGetWidth(), xPlatform, "xPlatform")); 
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, ofGetHeight(), yPlatform, "yPlatform")); 
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, 500, platformBaseWidth, "platformBaseWidth")); 
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 0.0, 500, platformBaseHeight, "platformBaseHeight"));
    guiPlatform->addWidgetDown(new ofxUISlider(300,16, 200, 400, scooterHandleLength, "scooterHandleLength"));
    ofAddListener(guiPlatform->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::setGUISetup(){
    // ofxUI - setting up UI
    
    guiSetup = new ofxUICanvas(10,10,320,ofGetHeight()-20); //ofxUICanvas(float x, float y, float width, float height)	
	guiSetup->setFont("DIN.otf");
    
    // PHIDGET SECTION
    guiSetup->addWidgetDown(new ofxUILabel("PHIDGETS CALIBRATION", OFX_UI_FONT_LARGE));
    guiSetup->addWidgetDown(new ofxUISpacer(300, 2));  
    guiSetup->addWidgetDown(new ofxUIToggle( 16, 16, false, "enable phidget")); 
//    guiSetup->addWidgetDown(new ofxUISpacer(300, 0.5)); 
    guiSetup->addWidgetDown(new ofxUIToggle(16, 16, false, "enable saving"));
    
    guiSetup->addWidgetDown(new ofxUIRadio( 16, 16, "Bridge Number", bridgeNo, OFX_UI_ORIENTATION_HORIZONTAL)); 
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
//    thread.stopThread();
}


