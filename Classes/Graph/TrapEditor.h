/*
 *  ESRenderer.h
 *  Trap
 *
 *  Created by Max Tkachenko on 10/18/10.
 *  Copyright 2010 Maxim Tkachenko. All rights reserved.
 *
 */
#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import "../Trap/Experiment.h"


@interface TrapEditor : NSObject {
	Experiment *experiment;
	float scale;
}

@property Experiment *experiment;
@property float scale;

- (void)render; 
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

@end