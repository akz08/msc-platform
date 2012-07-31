//
//  wiiRemoteInterf.h
//  sensePrototype
//
//  Created by Amer Kamil on 31/07/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef sensePrototype_wiiRemoteInterf_h
#define sensePrototype_wiiRemoteInterf_h

class MyClassImpl
{
public:
    MyClassImpl( void );
    ~MyClassImpl( void );
    
    void init( void ); // used to instantiate a 'MyClass' instance
    int doSomethingWith( int *aParameter );
    void logMyMessage(char * aCStr );
    
private:
    void * self; // a private void pointer to an instance of 'MyClass'
};

#endif
