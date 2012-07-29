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
#include "testApp.h"

class ioThreading : public ofThread{
public:
    CWii wii;
    std::vector<CWiimote>::iterator i;
    int exType;
    std::vector<CWiimote> *pointerWiimotes;
    std::vector<CWiimote> referenceWiimotes;
    
    // variable to access from main thread
    
    bool boardConnected;
    bool boardReading;
    float total, topLeft, topRight, bottomLeft, bottomRight;
    
    void threadedFunction(){
        
        boardConnected = false;
        boardReading = false;

        while(isThreadRunning()) {
            
            wii.Find(1);
            std::vector<CWiimote>& wiimotes = wii.Connect();
            pointerWiimotes = &wiimotes;
            referenceWiimotes = *pointerWiimotes;
            
            boardConnected = true;
            
            while(boardConnected&&wiimotes.size()){
                
                if(wii.Poll()){
                    boardReading = true;
                    for(i = referenceWiimotes.begin(); i != referenceWiimotes.end(); ++i)
                    {
                        // Use a reference to make working with the iterator handy.
                        CWiimote & wiimote = *i;
                        switch(wiimote.GetEvent())
                        {
                            case CWiimote::EVENT_EVENT:
                                exType = wiimote.ExpansionDevice.GetType();
                                if(exType == wiimote.ExpansionDevice.TYPE_BALANCE_BOARD){
                                    lock();
                                    CBalanceBoard &bb = wiimote.ExpansionDevice.BalanceBoard;
                                    bb.WeightSensor.GetWeight(total, topLeft, topRight, bottomLeft, bottomRight);
                                    
                                    
                                    printf("balance board top left weight: %f\n", topLeft);
                                    printf("balance board top right weight: %f\n", topRight);
                                    printf("balance board bottom left weight: %f\n", bottomLeft);
                                    printf("balance board bottom right weight: %f\n", bottomRight);
                                    printf("balance board total weight: %f\n", total);
                                    unlock();
                                    
                                }
                                
                                break;
                            default:
                                break;
   
                        }
                        
                    }
                    
                }
            }
        }
    }
            
            
//            cout<<"testing"<<endl;
        
        
//        for(int i; i < 1;  i++){
//            CWii wii;
//            std::vector<CWiimote>::iterator i;
//            wii.Find(5);
//            found = true;
//            return;}
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
