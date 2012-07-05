//
//  ioPhidget.cpp
//  msc-platform
//
//  Created by Amer Kamil on 08/06/2012.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "ioPhidget.h"

    
int CCONV AttachHandler(CPhidgetHandle phid, void *userptr)
{
    // need to show flag to show attached
    
	CPhidgetBridgeHandle bridge = (CPhidgetBridgeHandle)phid;
    
    // enable all 4 bridges
	CPhidgetBridge_setEnabled(bridge, 0, PTRUE);
	CPhidgetBridge_setEnabled(bridge, 1, PTRUE);
	CPhidgetBridge_setEnabled(bridge, 2, PTRUE);
	CPhidgetBridge_setEnabled(bridge, 3, PTRUE);
    
    // need to decide on gain
	CPhidgetBridge_setGain(bridge, 0, PHIDGET_BRIDGE_GAIN_128);
	CPhidgetBridge_setGain(bridge, 1, PHIDGET_BRIDGE_GAIN_16);
	CPhidgetBridge_setGain(bridge, 2, PHIDGET_BRIDGE_GAIN_32);
	CPhidgetBridge_setGain(bridge, 3, PHIDGET_BRIDGE_GAIN_64);
    
    // setting the rate of data read-in
	CPhidgetBridge_setDataRate(bridge, 100);
    
	printf("Attach handler ran!\n");
	return 0;
}

int CCONV DetachHandler(CPhidgetHandle phid, void *userptr)
{
    // need to set a flag to show detached
	printf("Detach handler ran!\n");
	return 0;
}

int CCONV ErrorHandler(CPhidgetHandle phid, void *userptr, int ErrorCode, const char *errorStr)
{
	printf("Error event: %s\n",errorStr);
	return 0;
}


int CCONV data(CPhidgetBridgeHandle phid, void *userPtr, int index, double val)
{
	//CPhidgetBridgeHandle bridge = (CPhidgetBridgeHandle)phid; // what is the purpose of this?

    
	printf("Data Event (%d) %lf\n",index,val); // index refers to the 'port' of the phidget
    
    
	return 0;
}



