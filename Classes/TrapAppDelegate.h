//
//  TrapAppDelegate.h
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Trap/Experiment.h"
#import "ConfigView.h"

@class EAGLView;

@interface TrapAppDelegate : NSObject <UIApplicationDelegate> {
	Experiment	*experiment;
	ConfigView	*configControlView; 
	NSTimer		*stateTimer;
	
	UIWindow	*window;
    EAGLView	*glView;
	UIProgressView *progressView;
	UIView		*configView;
}

@property (nonatomic, retain) IBOutlet UIWindow			*window;
@property (nonatomic, retain) IBOutlet EAGLView			*glView;
@property (nonatomic, retain) IBOutlet UIProgressView	*progressView;
@property (nonatomic, retain) IBOutlet UIView			*configView;
@property (nonatomic, retain) IBOutlet ConfigView		*configControlView;

- (IBAction)stopExperimentTouched:(id)sender; 
- (IBAction)playExperimentTouched:(id)sender; 
- (IBAction)configTouched:(id)sender;

@end

