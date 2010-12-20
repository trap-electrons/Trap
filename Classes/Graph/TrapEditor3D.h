//
//  ES1Renderer.h
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//

#import "TrapEditor.h"

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "GraphHelper.h"

#import "../Common/Types.h"
#import "../Trap/Experiment.h"

@interface TrapEditor3D : TrapEditor {
@private
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth;
    GLint backingHeight;
    GLuint defaultFramebuffer, colorRenderbuffer, m_depthRenderbuffer, m_framebuffer, m_colorRenderbuffer;
	GLfloat rotateAngle, cameraR, cameraL, translateToBack, ratioSides, bigSide, smallSide;
	GLfloat energyMax, energyMin;
	
	/// particles colors
	vector< GLubyte >	particlesColors;
	
	/// colors range
	vector< RangeColor >	colors;
	
	/// particles number in the experiment
	uint	particlesNumber;
	
	/// current time of experiment dump 
	sint	currentDumpTime;
}

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

- (void)setColors:(vector< RangeColor >)colors_;
- (void)setDefaultColors;

// protected
- (void)prepareLights;
- (void)colorRamp:(const GLfloat)val color4b:(GLubyte *)color4b invMaxMinSub:(GLfloat)invMaxMinSub min:(GLfloat)min;
- (void)prepareResonator;
- (void)resizeWindow;

@end
