//
//  ioPhidget.cpp
//  msc-platform
//
//  Created by Amer Kamil on 08/06/2012.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "ioPhidget.h"

// redundant?
int ioPhidget::connectorStatus(bool connect){
    
    isConnected = connect;
    
    return 0;
}

// get the value of phidget bridge by specifying index
double ioPhidget::getValue(int index){
    
    double value = 0;
    CPhidgetBridge_getBridgeValue(bridge, index, &value);
    if (value == 0) {
        lc0.value = value;
    }
    //    value = lc0.value;
    return value;
}

double* ioPhidget::getValues(){

    double *array = new double[4];
    CPhidgetBridge_getBridgeValue(bridge, 0, &array[0]);
    CPhidgetBridge_getBridgeValue(bridge, 0, &array[1]);
    CPhidgetBridge_getBridgeValue(bridge, 0, &array[2]);
    CPhidgetBridge_getBridgeValue(bridge, 0, &array[3]);
    
    return array;
    
}

double ioPhidget::calibValue(int index){
    
    // init value
    double value = 0;
    CPhidgetBridge_getBridgeValue(bridge, index, &value);
    
    // hard coding the calibration for the moment;
    switch(index){
        case 0:
            value= 25.1078*value + 0.3380;
            break;
        case 1:
            value = 24.4284*value - 0.9835;
            break;
        case 2:
            value = 29.1218*value - 0.0542;
            break;
        case 3:
            value = 27.3662*value + 0.7292;
            break;
        default:
            break;
    }
    
    return value;
    
}

double ioPhidget::calibrate(int index, double current, double value1, double cur1, double value2, double cur2){
    
    // just implementing the calibration equation & returning the calibrated value
    // index is currently redundant
    double calibrated;
    double slope = (value2 - value1) / (cur2 - cur1);
    double yint = value1 - (cur1*slope);
    
    calibrated = (slope*current) + yint;
    
    return calibrated;
    
}

int ioPhidget::assignRawData(int index, double value){
    

    
    switch (index) {
            //        case 0:
            //            lc0.value = value;
            //            lc0.index = index;
            //            printf("Data Event (%d) %lf\n",lc0.index,lc0.value);
            //            break;
            //        case 1:
            //            lc1.value = value;
            //            lc1.index = index;
            //            printf("Data Event (%d) %lf\n",lc1.index,lc1.value);
            //            break;
            //        case 2:
            //            lc2.value = value;
            //            lc2.index = index;
            //            printf("Data Event (%d) %lf\n",lc2.index,lc2.value);
            //            break;
            //        case 3:
            //            lc3.value = value;
            //            lc3.index = index;
            //            printf("Data Event (%d) %lf\n",lc3.index,lc3.value);
            //            break;
            
        default:
            break;
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

