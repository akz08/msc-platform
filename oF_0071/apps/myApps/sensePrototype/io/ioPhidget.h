//
//  ioPhidget.h
//  msc-platform
//
//  Created by Amer Kamil on 08/06/2012.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
// Based on 

#ifndef msc_platform_ioPhidget_h
#define msc_platform_ioPhidget_h

#include "Phidget21.h"
#include "stdio.h"

// these determine the rate of polling of data from phidgets (i think)
#define FREQS_SIZE 20
//double bridges[FREQS_SIZE] = {0};

int CCONV AttachHandler(CPhidgetHandle phid, void *userptr);
int CCONV DetachHandler(CPhidgetHandle phid, void *userptr);
int CCONV ErrorHandler(CPhidgetHandle phid, void *userptr, int ErrorCode, const char *errorStr);
int CCONV data(CPhidgetBridgeHandle phid, void *userPtr, int index, double val);


class ioPhidget
{
public:
    CPhidgetBridgeHandle bridge;
    
    // define variables & pointers for phidget_bridge data
    int serialNo, version;
	const char *deviceptr;
    
    // Constructor
    ioPhidget(int timeout = 10000)
    {
        const char *err;
        int result;
        
        // create the bridge object
        CPhidgetBridge_create(&bridge);
        
        // set handlers to be run when: plugged in/opened from software; unplugged/closed from software; generates error.
        CPhidget_set_OnAttach_Handler((CPhidgetHandle)bridge, &AttachHandler, this);
        CPhidget_set_OnDetach_Handler((CPhidgetHandle)bridge, &DetachHandler, this);
        CPhidget_set_OnError_Handler((CPhidgetHandle)bridge, &ErrorHandler, this);
        
        //sets a bridge data event handler called by rate set by 'datarate'
        CPhidgetBridge_set_OnBridgeData_Handler(bridge, &data, this);
        
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
        
        //Wait for enter
        //getchar();
        
        
        
        return;
    }
    
    void display_generic_properties(CPhidgetHandle phid)
    {
        int sernum, version;
        const char *deviceptr;
        CPhidget_getDeviceType(phid, &deviceptr);
        CPhidget_getSerialNumber(phid, &sernum);
        CPhidget_getDeviceVersion(phid, &version);
        
        printf("%s\n", deviceptr);
        printf("Version: %8d SerialNumber: %10d\n", version, sernum);
        return;
    }
    
    // Destructor
    ~ioPhidget(){
        printf("Closing Phidget...\n");
        
        //Cleaning up memory
        CPhidget_close((CPhidgetHandle)bridge);
        CPhidget_delete((CPhidgetHandle)bridge);
    }
    
};

#endif
