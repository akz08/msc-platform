//
//  ioPhidget.cpp
//  msc-platform
//
//  Created by Amer Kamil on 08/06/2012.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "ioPhidget.h"

   
int ioPhidget::connectorStatus(bool connect){
    
    isConnected = connect;
    
    return 0;
}

int ioPhidget::assignRawData(int index, double value){
    
//    switch (index) {
//
//        case 0:
//            lc0.index = index;
//            lc0.value = value;
//            break;
//            
//        case 1:
//            lc1.index = index;
//            lc1.value = value;
//            break;
//            
//        case 2:
//            lc2.index = index;
//            lc2.value = value;
//            break;
//            
//        case 3:
//            lc3.index = index;
//            lc3.value = value;
//            break;
//            
//        default:
//            break;
//    }

    if (index == 0 ){
        lc0.index = index;
        lc0.value = value;
    }
    
    else if (index == 1){
        lc1.value = value;
        lc1.index = index;
    }
    
    else if (index == 2){
        lc2.value = value;
        lc2.index = index;
    }
    
    else if (index == 3){
        lc3.value = value;
        lc3.index = 3;
    }
    
    
    return 0;
}

int CCONV AttachHandler(CPhidgetHandle phid, void *userptr)
{
    
	CPhidgetBridgeHandle bridge = (CPhidgetBridgeHandle)phid;
    
    // enable all 4 bridges
	CPhidgetBridge_setEnabled(bridge, 0, PTRUE);
	CPhidgetBridge_setEnabled(bridge, 1, PTRUE);
	CPhidgetBridge_setEnabled(bridge, 2, PTRUE);
	CPhidgetBridge_setEnabled(bridge, 3, PTRUE);
    
    // need to decide on gain
	CPhidgetBridge_setGain(bridge, 0, PHIDGET_BRIDGE_GAIN_128);
	CPhidgetBridge_setGain(bridge, 1, PHIDGET_BRIDGE_GAIN_128);
	CPhidgetBridge_setGain(bridge, 2, PHIDGET_BRIDGE_GAIN_128);
	CPhidgetBridge_setGain(bridge, 3, PHIDGET_BRIDGE_GAIN_128);
    
    // setting the rate of data read-in
	CPhidgetBridge_setDataRate(bridge, 100);
    
    //CPhidget_getDeviceStatus((CPhidgetHandle)phid, &iophidgetIsConnected);

	printf("Attach handler ran!");
    
    // send a flag that attach handler ran
    bool connected = true;

	return ((ioPhidget*)userptr)->connectorStatus(connected);
}

int CCONV DetachHandler(CPhidgetHandle phid, void *userptr)
{
	printf("Detach handler ran!\n");
    
    // send a flag that attach handler ran
    bool connected = false;

	return ((ioPhidget*)userptr)->connectorStatus(connected);
}

int CCONV ErrorHandler(CPhidgetHandle phid, void *userptr, int ErrorCode, const char *errorStr)
{
	printf("Error event: %s\n",errorStr);
	return 0;
}


int CCONV data(CPhidgetBridgeHandle phid, void *userPtr, int index, double val)
{
	//CPhidgetBridgeHandle bridge = (CPhidgetBridgeHandle)phid; // what is the purpose of this?

    
//	printf("Data Event (%d) %lf\n",index,val); // index refers to the 'port' of the phidget

//    userPtr = &index;
    
//    double dat[2] = {(double)index, val};
//    userPtr = &dat;
//
//    printf("test %f, %f", dat[0],dat[1]);
//    userPtr = &index;
    
	return ((ioPhidget*)userPtr)->assignRawData(index, val);
}

void ioPhidget::connect(int timeout = 10000){
    
    
    const char *err;
    int result;
    
    
    // this starts reading in data.
    //sets a bridge data event handler called by rate set by 'datarate'
    CPhidgetBridge_set_OnBridgeData_Handler(bridge, &data, &dat);
    
    // open bridge for device connections
    CPhidget_open((CPhidgetHandle)bridge, -1);
    
    //Wait for 10 seconds, otherwise exit
    printf("Waiting for attachment...");
    if((result = CPhidget_waitForAttachment((CPhidgetHandle)bridge, timeout)))
    {
        
        CPhidget_getErrorDescription(result, &err);
        printf("Problem waiting for attachment: %s\n", err);
        return;
    }
    
    display_generic_properties((CPhidgetHandle)bridge);
    
    // return if the device is attached or not (int)
    CPhidget_getDeviceStatus((CPhidgetHandle)bridge, &isAttached);

    
    
    return;
}

void ioPhidget::display_generic_properties(CPhidgetHandle phid)
{
    const char *deviceptr;
    CPhidget_getDeviceType(phid, &deviceptr);
    CPhidget_getSerialNumber(phid, &serialNumber);
    CPhidget_getDeviceVersion(phid, &version);
    
    printf("%s\n", deviceptr);
    printf("Version: %8d SerialNumber: %10d\n", version, serialNumber);
    return;
}

