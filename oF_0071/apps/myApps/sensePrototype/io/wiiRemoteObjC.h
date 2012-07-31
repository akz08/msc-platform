#import "WiiRemote.h"
#import "WiiRemoteDiscovery.h"

#import "wiiRemoteInterf.h"

@interface wiiRemoteObjC : NSObject {

    WiiRemoteDiscovery* discovery;
    WiiRemote* wii;
    
    float tare;
    float avgWeight;
    float sentWeight;
    float lastWeight;
    float weightSamples[100]; // may not be necessary
    int weightSampleIndex; // may also not be necessary
    BOOL sent;

    int someVar;
    
}


// (re?)defining some of the stuff we defined in the interface header file
- (int) doSomethingWith:(int*) aParameter;
- (void) logMyMessage:(char*) aCStr;


@end