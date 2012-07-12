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

//struct to hold data from load cells
struct raw {
    int index;
    double value;
    double xLoc;
    double yLoc;
};

class ioPhidget
{
public:
    CPhidgetBridgeHandle bridge;
    
    // define variables & pointers for phidget_bridge data
    int serialNumber, version;
	const char *deviceptr;
    
    int test;
    
    
    double dat[2];
    
    struct raw lc0, lc1, lc2, lc3;
    
    double cell0, cell1, cell2, cell3;
    double cells[4];
    
    
    //int dat;
    void connect(int timeout);
    
    void rawOutput();
    
    
    
    int isAttached;
    
    
    
    
    bool isConnected;
    
    
    
    //    //struct to hold data from load cells
    //    struct raw {
    //        int index;
    //        double value;
    //        double xLoc;
    //        double yLoc;
    //    }lc0,lc1, lc2, lc3;
    
    struct calibrationValues {
        double rawValue1;
        double rawValue2;
        double actValue1;
        double actValue2;
        double slope;
        double yIntercept;
        double currentValue;
        double currentCalibratedValue;
        bool calculated;
    };
    

    
    void display_generic_properties(CPhidgetHandle phid);
    
    double getValue(int index);
    double *getValues();
    
    double calibValue(int index);
    double calibrate(int index, double current, double value1, double cur1, double value2, double cur2);
    
    int assignRawData(int index, double value);
    int connectorStatus(bool connect);
    
    // Constructor
    ioPhidget()
    {
        isConnected = false;
        
        // create the bridge object
        CPhidgetBridge_create(&bridge);
        
        // set handlers to be run when: plugged in/opened from software; unplugged/closed from software; generates error.
        CPhidget_set_OnAttach_Handler((CPhidgetHandle)bridge, &AttachHandler, this);
        CPhidget_set_OnDetach_Handler((CPhidgetHandle)bridge, &DetachHandler, this);
        CPhidget_set_OnError_Handler((CPhidgetHandle)bridge, &ErrorHandler, this);
        
        
        
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
