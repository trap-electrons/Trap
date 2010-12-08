//
//  ESRenderer.h
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import "Trap/Experiment.h"

@interface ESRenderer <NSObject> {
	Experiment *experiment;
}

@property Experiment *experiment;

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

@end
