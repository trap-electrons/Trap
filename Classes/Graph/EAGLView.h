//
//  EAGLView.h
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import "TrapEditor.h"
#import "../Trap/Experiment.h"

@interface EAGLView : UIView
{    
@private
    TrapEditor *renderer;
	Experiment *experiment;
	
    BOOL animating;
    BOOL displayLinkSupported;
    NSInteger animationFrameInterval;
    // Use of the CADisplayLink class is the preferred method for controlling your animation timing.
    // CADisplayLink will link to the main display and fire every vsync when added to a given run-loop.
    // The NSTimer class is used only as fallback when running on a pre 3.1 device where CADisplayLink
    // isn't available.
    id displayLink;
    NSTimer *animationTimer;
	
	float initAngle, prevScale, prevScaleDiff;
}

@property Experiment *experiment;
@property (nonatomic, readonly) TrapEditor *renderer;
@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView:(id)sender;
- (void)makeRenderer;
- (void)freeRenderer;

- (void)resetViewTransform;

@end
