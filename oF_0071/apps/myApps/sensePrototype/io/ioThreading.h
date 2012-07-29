//
//  ioThreading.h
//  sensePrototype
//
//  Created by Amer Kamil on 29/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef sensePrototype_ioThreading_h
#define sensePrototype_ioThreading_h

#include "ofThread.h"
#include "stdio.h"
#include "wiicpp.h"
#include "ofMain.h"

class ioThreading : public ofThread{
public:
    
    bool dostuff;
    
    bool found; 
    CWii wii;
    
    void threadedFunction(){
        //start
        dostuff = true;
        found = false;
        
        while(isThreadRunning()) {
//            cout<<"testing"<<endl;
        
        
//        for(int i; i < 1;  i++){
//            CWii wii;
            std::vector<CWiimote>::iterator i;
            wii.Find(5);
            found = true;
            return;}
//            std::vector<CWiimote>& wiimotes = wii.Connect();
//       
//        do{
//            
//            if(wii.Poll()){
//                for(i = wiimotes.begin(); i != wiimotes.end(); ++i)
//                {
//                    CWiimote & wiimote = *i;
//                    switch(wiimote.GetEvent())
//                    {
//                        case CWiimote::EVENT_EVENT:
//                            HandleEvent(wiimote);
//                            break;
//                        default:
//                            break;
//                    }
//                }
//            }
//            
//        }while(wiimotes.size()||dostuff);
//        
//        }
        //done
    }
    
    void HandleEvent(CWiimote &wm)
    {
        int exType = wm.ExpansionDevice.GetType();
    if(exType == wm.ExpansionDevice.TYPE_BALANCE_BOARD) 
    {
		CBalanceBoard &bb = wm.ExpansionDevice.BalanceBoard;
		float total, topLeft, topRight, bottomLeft, bottomRight;
		
		bb.WeightSensor.GetWeight(total, topLeft, topRight, bottomLeft, bottomRight);
		printf("balance board top left weight: %f\n", topLeft);
		printf("balance board top right weight: %f\n", topRight);
		printf("balance board bottom left weight: %f\n", bottomLeft);
		printf("balance board bottom right weight: %f\n", bottomRight);
		printf("balance board total weight: %f\n", total);
	}
    }
    
    ~ioThreading(){
//        stopThread();
    }


    
};

#endif
