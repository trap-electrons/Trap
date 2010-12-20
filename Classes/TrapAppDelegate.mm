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

@synthesize window, progressView, glView, configView, configControlView;

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

- (void)newExperiment {
	if (experiment) delete experiment;
	NSString *source = [[NSBundle mainBundle] resourcePath];
	experiment = new Experiment("", (const char*)[source UTF8String]);
	
	[configControlView setExperiment:experiment];
	[glView setExperiment:experiment];

}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	sleep(1);
	[self newExperiment];
	
	[configControlView startExperiment];
	stateTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 20.0)) target:self selector:@selector(stateRender:) userInfo:nil repeats:TRUE];
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
	[configControlView stopExperiment];
}

- (IBAction)playExperimentTouched:(id)sender {
	[configControlView startExperiment];
}

- (IBAction)configTouched:(id)sender {
	[configView setHidden:!configView.hidden];
}

@end
