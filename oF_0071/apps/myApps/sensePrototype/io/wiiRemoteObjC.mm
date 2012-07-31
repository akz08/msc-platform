// based on code from wiiScale by Ford Parsons

#import "wiiRemoteObjC.h"

@implementation wiiRemoteObjC

MyClassImpl::MyClassImpl(void) // using the class in the interface header
: self(NULL)
{   }

MyClassImpl::~MyClassImpl(void)
{
    [(id)self dealloc];
}


void MyClassImpl::init(void)
{
    self = [[wiiRemoteObjC alloc] init]; // swap MyClass with MyClassImpl
}

int MyClassImpl::doSomethingWith(int *aParameter)
{
    return [(id)self doSomethingWith:aParameter];
}

void MyClassImpl::logMyMessage(char *aCStr)
{
    [(id)self logMyMessage:aCStr]; // this may be incorrectly named!!
}

// obj-c working functions

- (int) doSomethingWith:(int *)aParameter
{
    int result;
    
    result = 1 + 1;
    
    return result;
}

- (void) logMyMessage:(char *) aCStr
{
    NSLog(@"testing"/*aCStr*/);
}

/////////

#pragma mark Window

- (id)init
{
    self = [super init];
    if (self) {
		
		weightSampleIndex = 0;
		
		if(!discovery) {
			[self performSelector:@selector(doDiscovery:) withObject:self afterDelay:0.0f];
		}
        
		
    }
    return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)awakeFromNib {    
    
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(expansionPortChanged:)
												 name:@"WiiRemoteExpansionPortChangedNotification"
											   object:nil];
}



#pragma mark NSApplication

- (void)applicationWillTerminate:(NSNotification *)notification
{
	[wii closeConnection];
}


#pragma mark Wii Balance Board

- (void)doDiscovery {
	
	if(!discovery) {
		discovery = [[WiiRemoteDiscovery alloc] init];
//		[discovery setDelegate:self];
		[discovery start];
		
//		[spinner startAnimation:self];
//		[bbstatus setStringValue:@"Searching..."];
//		[fileConnect setTitle:@"Stop Searching for Balance Board"];
//		[status setStringValue:@"Press the red 'sync' button..."];
	} else {
		[discovery stop];
		[discovery release];
		discovery = nil;
		
		if(wii) {
			[wii closeConnection];
			[wii release];
			wii = nil;
		}
		
//		[spinner stopAnimation:self];
//		[bbstatus setStringValue:@"Disconnected"];
        NSLog (@"Disconnected");
//		[fileConnect setTitle:@"Connect to Balance Board"];
        NSLog(@"Connect to Balance Board");
//		[status setStringValue:@""];
	}
}

- (void)doTare {
	tare = 0.0 - lastWeight;
}

#pragma mark Magic?

- (void)expansionPortChanged:(NSNotification *)nc{
    
	WiiRemote* tmpWii = (WiiRemote*)[nc object];
	
	// Check that the Wiimote reporting is the one we're connected to.
	if (![[tmpWii address] isEqualToString:[wii address]]){
		return;
	}
	
	if ([wii isExpansionPortAttached]){
		[wii setExpansionPortEnabled:YES];
	}	
}

#pragma mark    

- (void) buttonChanged:(WiiButtonType) type isPressed:(BOOL) isPressed
{	
	[self doTare];
}

- (void) wiiRemoteDisconnected:(IOBluetoothDevice*) device
{	
//	[spinner stopAnimation:self];
//	[bbstatus setStringValue:@"Disconnected"];
	
	[device closeConnection];
}

#pragma mark WiiRemoteDelegate methods (optional)

// cooked values from the Balance Beam
- (void) balanceBeamKilogramsChangedTopRight:(float)topRight
                                 bottomRight:(float)bottomRight
                                     topLeft:(float)topLeft
                                  bottomLeft:(float)bottomLeft {
	
	lastWeight = topRight + bottomRight + topLeft + bottomLeft;
	
	if(!tare) {
		[self doTare];
	}
	
	float trueWeight = lastWeight + tare;
//	[weightIndicator setDoubleValue:trueWeight];
	
	if(trueWeight > 10.0) {
		weightSamples[weightSampleIndex] = trueWeight;
		weightSampleIndex = (weightSampleIndex + 1) % 100;
		
		float sum = 0;
		float sum_sqrs = 0;
		
		for (int i = 0; i < 100; i++)
		{
			sum += weightSamples[i];
			sum_sqrs += weightSamples[i] * weightSamples[i];
		}
		
		avgWeight = sum / 100.0;
		float var = sum_sqrs / 100.0 - (avgWeight * avgWeight);
		float std_dev = sqrt(var);
        
		if(!sent);
//			[status setStringValue:@"Please hold still..."];
		else;
//			[status setStringValue:[NSString stringWithFormat:@"Sent weight of %4.1fkg.  Thanks!", avgWeight]];
        
		
		if(std_dev < 0.1 && !sent)
		{
			sent = YES;
		}
		
	} else {
		sent = NO;
//		[status setStringValue:@"Tap the button to tare, then step on..."];
	}
    
//	[weight setStringValue:[NSString stringWithFormat:@"%4.1fkg  %4.1flbs", MAX(0.0, trueWeight), MAX(0.0, (trueWeight) * 2.20462262)]];
}

#pragma mark WiiRemoteDiscoveryDelegate methods

- (void) WiiRemoteDiscovered:(WiiRemote*)wiimote {
	
	[wii release];
	wii = [wiimote retain];
//	[wii setDelegate:self];
    
//	[spinner stopAnimation:self];
//	[bbstatus setStringValue:@"Connected"];
	
//	[status setStringValue:@"Tap the button to tare, then step on..."];
}

- (void) WiiRemoteDiscoveryError:(int)code {
	
	NSLog(@"Error: %u", code);
    
	// Keep trying...
//	[spinner stopAnimation:self];
	[discovery stop];
	sleep(1);
	[discovery start];
//	[spinner startAnimation:self];
}

- (void) willStartWiimoteConnections {
    
}


@end
