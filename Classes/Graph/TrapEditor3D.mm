//
//  ES1Renderer.m
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//

#import "TrapEditor3D.h"

@implementation TrapEditor3D

//-----------------------------------------------------------------------------
- (id)init
{
    if ((self = [super init]))
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

        if (!context || ![EAGLContext setCurrentContext:context])
        {
            [self release];
            return nil;
        }

        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
	    glGenFramebuffersOES(1, &defaultFramebuffer);
        glGenRenderbuffersOES(1, &colorRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, 
									GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);
				
		translateToBack = -100.0f;
		[self setDefaultColors];
		[self prepareResonator];
		[self resizeWindow];
		//[self prepareLights];
		
		glShadeModel(GL_SMOOTH);
	}
	
    return self;
}


//-----------------------------------------------------------------------------
- (void)prepareLights {
	GLfloat light1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light1_position[] = { cameraR*2.0, cameraR*2.0, cameraL, 0.0 };
	GLfloat spot_direction[] = { 0, 1, 1 };
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5); 
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2); 
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction); 
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
	glEnable(GL_LIGHT1);
	glDisable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	
	glShadeModel(GL_SMOOTH);
}


//-----------------------------------------------------------------------------
- (void)setExperiment:(Experiment *)e {
	experiment = e;
	
	particlesNumber = experiment->particles->getNumber();
	particlesColors.resize(particlesNumber*4);
}


//-----------------------------------------------------------------------------
- (void)refreshParticlesColors {
		
	if (currentDumpTime != experiment->currentDumpTime) {
		
		if (energyMax<experiment->particles->getEnergyMax())
			energyMax = experiment->particles->getEnergyMax();
		
		if (energyMin>experiment->particles->getEnergyMin()) 
			energyMin = experiment->particles->getEnergyMin();
		if (energyMin==0) energyMin = (float)experiment->particles->getEnergyMin();
		
		GLfloat 
		max = energyMax, 
		min = energyMin,
		invMaxMinSub = 1.0/(max-min);
		
		for (size_t i=0; i<particlesNumber; i++) {
			if (experiment->particles->index[i].dead!=1) {
				GLfloat energy = experiment->particles->gm[i];
				[self colorRamp:energy color4b:&particlesColors[i*4] invMaxMinSub:invMaxMinSub min:min];
			}
			else {
				particlesColors[i*4+0] = 0;
				particlesColors[i*4+1] = 0;
				particlesColors[i*4+2] = 0;
				particlesColors[i*4+3] = 255;
			}
		}	
		currentDumpTime = experiment->currentDumpTime;
	}
}


//-----------------------------------------------------------------------------
- (void)setDefaultColors {
	//							R    G    B    A        value
	colors.push_back(RangeColor(000, 000, 255, 200,		0.0));
	colors.push_back(RangeColor(255, 255, 000, 200,		0.2));
	colors.push_back(RangeColor(255, 000, 000, 200,		0.4));
	colors.push_back(RangeColor(255, 255, 255, 200,		1.0));
	
	[self setColors:colors];
}


//-----------------------------------------------------------------------------
- (void)setColors:(vector< RangeColor >)colors_ {
	colors = colors_;
	if (colors.size()<2) [self setDefaultColors];
	
	colors[0].tag=0.0;
	for (size_t i=1; i<colors.size(); i++)
		colors[i].tag = 1.0/(colors[i].range-colors[i-1].range);
}


//-----------------------------------------------------------------------------
- (void)colorRamp:(const GLfloat)val color4b:(GLubyte *)color4b invMaxMinSub:(GLfloat)invMaxMinSub min:(GLfloat)min {
	if (val <= 1.0) { color4b[0]=color4b[1]=color4b[2]=color4b[3]=1; return; }
	GLfloat p = (val-min)*invMaxMinSub;
	GLfloat p2; GLint i_1;
	
	for (size_t i=1; i<colors.size(); i++)
		if (p < colors[i].range) {
			i_1 = i-1;
			p2 = (p-colors[i_1].range)*colors[i].tag; // p2 is in [0; 1]
			color4b[0] = colors[i_1].r + (colors[i].r-colors[i_1].r)*p2;
			color4b[1] = colors[i_1].g + (colors[i].g-colors[i_1].g)*p2;
			color4b[2] = colors[i_1].b + (colors[i].b-colors[i_1].b)*p2;
			color4b[3] = colors[i_1].a + (colors[i].a-colors[i_1].a)*p2;
			return;
		}
}


//-----------------------------------------------------------------------------
static const int resonatorQuality = 80;
static float cx[resonatorQuality] = {0};
static float cy[resonatorQuality] = {0};	
static vector3 resonatorCoords[resonatorQuality*6];
static vertex3 resonatorNormals[resonatorQuality*6];

//-----------------------------------------------------------------------------
- (void)prepareResonator {
	int i, j;
	
	for (i = 0; i < resonatorQuality; i++)
	{
		GLfloat angle = 2.0 * M_PI * ((float)i) / (GLfloat)(resonatorQuality-1);
		cx[i] = sin(angle);
		cy[i] = cos(angle);
	}
	
	for (i = 0, j=0; j<resonatorQuality-1; i+=6, j++)
	{
		// triangle 1
		resonatorCoords[i+0].v[0] = cx[j];		// 0
		resonatorCoords[i+0].v[1] = cy[j]; 
		resonatorCoords[i+0].v[2] = 0; 

		resonatorCoords[i+1].v[0] = cx[j+1];	// 1
		resonatorCoords[i+1].v[1] = cy[j+1]; 
		resonatorCoords[i+1].v[2] = 0; 
		
		resonatorCoords[i+2].v[0] = cx[j];		// 2
		resonatorCoords[i+2].v[1] = cy[j]; 
		resonatorCoords[i+2].v[2] = 1;
		
		getNormal((vertex3*)&resonatorCoords[i+0], &resonatorNormals[i+0]);
		resonatorNormals[i+1] = resonatorNormals[i+0];
		resonatorNormals[i+2] = resonatorNormals[i+0];

		// triangle 2
		resonatorCoords[i+3].v[0] = cx[j+1];	// 1
		resonatorCoords[i+3].v[1] = cy[j+1]; 
		resonatorCoords[i+3].v[2] = 0; 		
		
		resonatorCoords[i+4].v[0] = cx[j+1];	// 3
		resonatorCoords[i+4].v[1] = cy[j+1]; 
		resonatorCoords[i+4].v[2] = 1; 

		resonatorCoords[i+5].v[0] = cx[j];		// 2
		resonatorCoords[i+5].v[1] = cy[j]; 
		resonatorCoords[i+5].v[2] = 1; 
		
		getNormal((vertex3*)&resonatorCoords[i+3], &resonatorNormals[i+3]);
		resonatorNormals[i+4] = resonatorNormals[i+3];
		resonatorNormals[i+5] = resonatorNormals[i+3];
	}
	
	for (i = 0, j=0; j<resonatorQuality-2; i+=3, j++)
	{
		//averageVector(&resonatorNormals[i+0], &resonatorNormals[i+6], &resonatorNormals[i+3]);
		//averageVector(&resonatorNormals[i+1], &resonatorNormals[i+7], &resonatorNormals[i+4]);
		//averageVector(&resonatorNormals[i+2], &resonatorNormals[i+8], &resonatorNormals[i+5]);
		
		//resonatorNormals[i+6] = resonatorNormals[i+3];
		//resonatorNormals[i+7] = resonatorNormals[i+4];
		//resonatorNormals[i+8] = resonatorNormals[i+5];		
	}
	
	for (i = 0, j=0; j<resonatorQuality; i++, j++) {
		//resonatorNormals[i].x = -resonatorNormals[i].x;
		//resonatorNormals[i].y = resonatorNormals[i].y;
		//resonatorNormals[i].z = -resonatorNormals[i].z;
	}
}

//-----------------------------------------------------------------------------
- (void)renderResonator:(GLenum)cullfaceMode {
	glPushMatrix();
	
	glTranslatef(0,0,-cameraL*0.5);
	glEnable(GL_CULL_FACE);
	glCullFace(cullfaceMode);
	
	glScalef(cameraR, cameraR, cameraL);

    GLfloat   diffuse[] = { 1, 1, 1, 0.2 };
   // GLfloat   defaultEmission[] = { 0.0, 0.0, 0.0, 0.2 };
   // GLfloat   greenSpecular[] = { 0.0, 1.0, 0.0, 0.2 };
	
	//glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, defaultEmission );
	//glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, redAmbient );
	//glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, greenSpecular );
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &resonatorCoords[0].v[0]);
	glNormalPointer(GL_FLOAT, 0, &resonatorNormals[0].x);
	glDrawArrays(GL_TRIANGLES, 0, resonatorQuality*6);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glPopMatrix();
}


//-----------------------------------------------------------------------------
- (void)renderParticles {
	glPushMatrix();
	
	[self refreshParticlesColors];
	
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &experiment->particles->x[0]);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, &particlesColors[0]);
	glDrawArrays(GL_POINTS, 0, experiment->particles->getNumber());
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);	
	
	glPopMatrix();
}


//-----------------------------------------------------------------------------
- (void)renderAxes {
	static float axesCoords[6*3] = {-1, 0, 0, 1, 0, 0,		// x
									0, -1, 0, 0, 1, 0,		// y
									0, 0, -1, 0, 0, 1};		// z
	
	static float axesColors[4*2*3] = {
									0.0, 0.7, 0.0, 0.5,		// x - green
									0.0, 0.7, 0.0, 0.5,
		
									0.0, 0.0, 0.7, 0.5,		// y - blue
									0.0, 0.0, 0.7, 0.5,		
		
									0.7, 0.0, 0.0, 0.5,		// z - red
									0.7, 0.0, 0.0, 0.5};	
	
	glPushMatrix();
	glScalef(cameraR, cameraR, cameraL*0.5);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, axesCoords);
	glColorPointer (4, GL_FLOAT, 0, axesColors);
	glDrawArrays(GL_LINES, 0, 3*2);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
}


//-----------------------------------------------------------------------------
- (void)renderBackground {
	static float backCoords[4*3] = {
		-1,  1, 0,	// left bottom
		-1, -1, 0,	// left top
 	 	 1,  1, 0,	// right bottom
		 1, -1, 0};	// right top 	 
	
	static GLubyte backColors[4*4] = {
		32,  34,  42,  255,
		123, 125, 133, 255,
		32,  34,  42,  255,
		123, 125, 133, 255};	
	
	glPushMatrix();
	if (backingWidth>backingHeight) {
		glScalef(ratioSides, 1, 1);
	}
	else {
		glScalef(1, ratioSides, 1);		
	}
	glTranslatef(0, 0, translateToBack*1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, backCoords);
	glColorPointer (4, GL_UNSIGNED_BYTE, 0, backColors);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopMatrix();
}


//-----------------------------------------------------------------------------
- (void)renderMagneticField {
	vector< vector4 > magFieldCoords, magFieldColors;
	vector< vector< vector<real> > > &bx = experiment->BField->bx;
	vector< vector< vector<real> > > &by = experiment->BField->by;
	vector< vector< vector<real> > > &bz = experiment->BField->bz;
	
	GLint countX = bx.size(), countZ = bx[0][0].size();
	GLint count = countX * countZ; 
	magFieldCoords.resize(count);
	magFieldColors.resize(count);
	
	for (sint k=0; k<bx[0][0].size(); k++) // z
		//for (sint j=0; j<bx[0].size(); j++) // y
			for (sint i=0; i<bx.size(); i++) // x
			{
				int ind = i+k*bx[0].size();
				magFieldCoords[ind].v[0] = float(i)/float(countX);
				GLfloat tmp = sqrt(sqr(bx[i][0][k]) + sqr(by[i][0][k]) + sqr(bz[i][0][k]));;
				magFieldCoords[ind].v[1] = tmp;
				magFieldCoords[ind].v[2] = float(k)/float(countZ);
				
				magFieldColors[ind].v[0] = bx[i][0][k]/10.0;
				magFieldColors[ind].v[1] = by[i][0][k]/10.0;
				magFieldColors[ind].v[2] = bz[i][0][k]/10.0;
				magFieldColors[ind].v[3] = 0.7;
			}
	
	glPushMatrix();
	glScalef(cameraR, cameraR, cameraL);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &magFieldCoords[0]);
	glColorPointer (4, GL_FLOAT, 0, &magFieldColors[0]);
	glDrawArrays(GL_POINTS, 0, count);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glPopMatrix();
}


//-----------------------------------------------------------------------------
- (void)render
{
    [EAGLContext setCurrentContext:context];
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
	
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(2.0f);
	
	glPushMatrix();
	[self renderBackground];

	glScalef(scale, scale, scale);
	glTranslatef(0,0,translateToBack);
	glRotatef(rotateAngle, 0, 1, 0);
	const GLfloat dCoef = experiment->rl / experiment->cameraD;
	const GLfloat lCoef = experiment->rl / experiment->cameraL;
	const GLfloat coef = lCoef>dCoef ? dCoef : lCoef;
	glScalef(coef, coef, coef);
	rotateAngle+=0.1;
	
	// Resonator sizes
	GLfloat invRl = 1.0/experiment->rl;
	cameraR = experiment->cameraD*0.5*invRl;
	cameraL = experiment->cameraL*invRl;
		
	[self renderResonator:GL_BACK];
	[self renderAxes];
	[self renderParticles];
	//[self renderMagneticField];
	[self renderResonator:GL_FRONT];

	glPopMatrix();
	glDisable(GL_BLEND);
	
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
}


//-----------------------------------------------------------------------------
- (void)resizeWindow {
	GLint width = backingWidth, height = backingHeight;

	GLfloat left, right, top, bottom;
	
	if (width>height) {
		left	= -ratioSides;
		right	=  ratioSides;
		top		= -1;
		bottom	=  1;
	}
	else {
		left	= -1;
		right	=  1;
		top		= -ratioSides;
		bottom	=  ratioSides;		
	}
	
    glViewport(0, 0, width, height);	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(left, right, top, bottom, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


//-----------------------------------------------------------------------------
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{	
	// Allocate color buffer backing based on the current layer size
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);

    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }

	if (backingWidth>backingHeight) {
		bigSide = backingWidth;
		smallSide = backingHeight;
	}
	else {
		bigSide = backingHeight;
		smallSide = backingWidth;
	}
	ratioSides = float(bigSide)/float(smallSide);
	
	[self resizeWindow];
    return YES;
}


//-----------------------------------------------------------------------------
- (void)dealloc
{
    // Tear down GL
    if (defaultFramebuffer) {
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }

    if (colorRenderbuffer) {
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }

    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    [context release];
    context = nil;

    [super dealloc];
}

@end
