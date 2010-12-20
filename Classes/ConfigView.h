//
//  ConfigView.h
//  Trap
//
//  Created by Max Tkachenko on 12/18/10.
//  Copyright 2010 Maxim Tkachenko. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "Experiment.h"

@interface ConfigView : NSObject {
	Experiment *experiment;
}

@property (nonatomic, retain) IBOutlet UITextField	*E;
@property (nonatomic, retain) IBOutlet UITextField	*frequency;
@property (nonatomic, retain) IBOutlet UILabel		*mode;

@property (nonatomic, retain) IBOutlet UITextField	*B00;

@property (nonatomic, retain) IBOutlet UITextField	*length;
@property (nonatomic, retain) IBOutlet UITextField	*diameter;

@property (nonatomic, retain) IBOutlet UITextField	*x;
@property (nonatomic, retain) IBOutlet UITextField	*y;
@property (nonatomic, retain) IBOutlet UITextField	*z;
@property (nonatomic, retain) IBOutlet UITextField	*angle;
@property (nonatomic, retain) IBOutlet UITextField	*energy;
@property (nonatomic, retain) IBOutlet UILabel		*distribution;

- (void)setExperiment:(Experiment *)e;
- (void)startExperiment; 
- (void)stopExperiment; 

- (IBAction)configApplyTouched:(id)sender;

@end
