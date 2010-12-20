//
//  ConfigView.m
//  Trap
//
//  Created by Max Tkachenko on 12/18/10.
//  Copyright 2010 Maxim Tkachenko. All rights reserved.
//

#import "ConfigView.h"


@implementation ConfigView

@synthesize E, frequency, mode, B00, length, diameter, x, y, z, angle, energy, distribution;

- (id)init {
	if ((self = [super init])) {
	}
	return self;
}

- (IBAction)configApplyTouched:(id)sender {
	[self stopExperiment];
	
	conf.E = [[E text] floatValue];
	conf.f = [[frequency text] floatValue] * 1E+10;
	//conf. = [[mode text] floatValue];	
	
	conf.B00 = [[B00 text] floatValue];
	// coils
	// hexapole
	
	conf.cameraL = [[length text] floatValue];
	conf.cameraD = [[diameter text] floatValue];
	
	conf.xInitRadius = [[x text] floatValue];
	conf.yInitRadius = [[y text] floatValue];
	conf.zInitRadius = [[z text] floatValue];
	conf.W0 = [[energy text] floatValue];
	conf.fi = [[angle text] floatValue];
	// distribution type
	
	[self startExperiment];
}

- (void)setExperiment:(Experiment *)e {
	experiment = e;
}

- (void)startExperimentInBackground {
	experiment->start();
}

- (void)startExperiment {
	if (!experiment->isExperimentRunning()) {
		[NSThread detachNewThreadSelector:@selector(startExperimentInBackground) toTarget:self withObject:nil];
	}
}

- (void)stopExperiment {
	experiment->stop();
	while (experiment->isExperimentRunning()) {
		; //sleep(1);
	}
}

@end
