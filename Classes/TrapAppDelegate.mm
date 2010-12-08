//
//  TrapAppDelegate.m
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//

#import "TrapAppDelegate.h"
#import "Graph/EAGLView.h"

@implementation TrapAppDelegate

@synthesize window, progressView, glView, configView;

- (void)startExperimentInBackground {
	experiment->start();
}

- (void)stateRender:(id)sender {
	
	if (experiment->maxDumpTime!=0) {
		[progressView setProgress:float(experiment->currentDumpTime)/float(experiment->maxDumpTime)];
	}
	else {
		[progressView setProgress:0.0];
	}
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	
	sleep(1);
	
	NSString *source = [[NSBundle mainBundle] resourcePath];	
	experiment = new Experiment("", (const char*)[source UTF8String]);
	[NSThread detachNewThreadSelector:@selector(startExperimentInBackground) toTarget:self withObject:nil];
	
	stateTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 20.0)) target:self selector:@selector(stateRender:) userInfo:nil repeats:TRUE];
	
	glView.experiment = experiment;
	[glView makeRenderer];
    [glView startAnimation];
    return YES;
}
	 
- (void)applicationWillResignActive:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)dealloc
{
	stateTimer = nil;
	delete experiment;
	[window release];
    [glView release];

    [super dealloc];
}

#pragma mark Interface

- (IBAction)stopExperimentTouched:(id)sender {
	experiment->stop();
}

- (IBAction)playExperimentTouched:(id)sender {
	if (!experiment->isExperimentRunning()) {
		[NSThread detachNewThreadSelector:@selector(startExperimentInBackground) toTarget:self withObject:nil];
	}
}

- (IBAction)configTouched:(id)sender {
	[configView setHidden:!configView.hidden];
}

@end
