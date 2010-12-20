/*
 *  ESRenderer.cpp
 *  Trap
 *
 *  Created by Max Tkachenko on 10/18/10.
 *  Copyright 2010 Maxim Tkachenko. All rights reserved.
 *
 */

#include "TrapEditor.h"

@implementation TrapEditor

@synthesize experiment;
@synthesize scale;

- (id)init {
	if (self = [super init]) {
		scale = 1.0;
	}
	return self;
}

- (void)render {}; 
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer { return false; }
- (void)setExperiment:(Experiment *)e { experiment = e; }

@end


