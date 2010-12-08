#ifndef LQ_GRAPH_LQGL
#define LQ_GRAPH_LQGL

#define M_LEFTBUTTON 1
#define M_RIGHTBUTTON 2
#define M_MIDDLEBUTTON 3

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "../Common/Types.h"

//--- Event Smoother ----------------------------------------------------------
/// EventSmother allows to make interpolate from FALSE to TRUE, and inverse.
/*! 
	Example :: \n
	time: 0; input: false => Calculate(false) = 0.0 \n
	time: 1; input: true => Calculate(true) = 0.1 \n
	time: 2; input: true => Calculate(true) = 0.2 \n
	time: 3; input: true => Calculate(true) = 0.3 \n
	... \n
	time: 10; input: true => Calculate(true) = 1.0 \n
	time: 11; input: true => Calculate(true) = 1.0 \n
	time: 12; input: false => Calculate(false) = 0.9 \n
	e.t.c. 
*/
class EventSmoother {
protected:
	/// up value (top, maximal value, default 1.0)
	GLfloat upLimit;
	/// down value (bottom, minimal value, default 0.0)
	GLfloat downLimit;
	/// will summ with output value at every time pass 
	GLfloat	step;
	/// Calculate returns this value, this is main variable
	GLfloat	value;
	
public:
	EventSmoother() 
	{
		value = 0.0f;
		step = 0.1f;
		downLimit = 0.0f; 
		upLimit = 1.0f; 
	}
	
	inline void setStep(GLfloat _step) 
	{ step=_step; }

	inline void setLimits(GLfloat up, GLfloat down) 
	{ upLimit=up; downLimit=down; value=downLimit; }

	inline GLfloat getUpLimit() { return upLimit; }
	inline GLfloat getValue() { return value; }

	/// Main function - calculates and returns a new value (interpolated beetween TRUE and FALSE).
	inline GLfloat Calculate(bool _event) 
	{
		if (_event) 
		{ 
			value += step;
			if (value>upLimit) value=upLimit;
		}
		else 
		{
			value -= step;
			if (value<downLimit) value=downLimit;
		}
		return value;
	}
};


//--- Rect --------------------------------------------------------------------
/// Rect3D storages left top back point and right bottom front point of rectangular parallelepiped
struct Rect3D {
public:
	GLfloat x, y, z, x2, y2, z2;

	Rect3D(GLfloat x_, GLfloat y_, GLfloat z_, GLfloat x2_, GLfloat y2_, GLfloat z2_) 
		: x(x_), y(y_), z(z_), x2(x2_), y2(y2_), z2(z2_) {}
	
	Rect3D() { x=0; y=0; z=0; x2=0; y2=0; z2=0; }

	/// return true if point (x_, y_, z_) is in Rect3D
	inline bool isInRect(GLfloat x_, GLfloat y_, GLfloat z_) {
		return
			(x_>=x && x_<=x2 || x_>=x2 && x_<=x) &&
			(y_>=y && y_<=y2 || y_>=y2 && y_<=y) &&
			(z_>=z && z_<=z2 || z_>=z2 && z_<=z);
	}
};


//--- Draw Quad ---------------------------------------------------------------
/// Draw simple Quad with Texture 
/*static inline void drawQuad(Rect3D rect) 
{
	glBegin(GL_QUADS);
		glTexCoord2f(0,  0);
		glVertex3f(rect.x,  rect.y,  rect.z);

		glTexCoord2f(1, 0);
		glVertex3f(rect.x2, rect.y,  rect.z);

		glTexCoord2f(1, -1);
		glVertex3f(rect.x2, rect.y2, rect.z);

		glTexCoord2f(0, -1);
		glVertex3f(rect.x,  rect.y2, rect.z);
	glEnd();
}*/


//-----------------------------------------------------------------------------
/// Helps to color interpolation
class RangeColor { 
public :
		GLubyte r,g,b,a;
		GLfloat tag, range;

		RangeColor() {}
		RangeColor(GLbyte r_, GLbyte g_, GLbyte b_, GLbyte a_, GLfloat range_) 
			: r(r_), g(g_), b(b_), a(a_), range(range_) {}
};


//-----------------------------------------------------------------------------
// normalize vector
static void normalize(vertex3 *v)
{
    // calculate the length of the vector
    float len = (float)(sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z)));
	
    // avoid division by 0
    if (len == 0.0f)
        len = 1.0f;
	
    // reduce to unit size
    v->x /= len;
    v->y /= len;
    v->z /= len;
}


//-----------------------------------------------------------------------------
// calculate normal
static void getNormal(vertex3 v[3], vertex3 *normal)
{
    vertex3 a, b;
	
    // calculate the vectors A and B
    // note that v[3] is defined with counterclockwise winding in mind
    // a
    a.x = v[1].x - v[0].x;
    a.y = v[1].y - v[0].y;
    a.z = v[1].z - v[0].z;
    // b
    b.x = v[2].x - v[0].x;
    b.y = v[2].y - v[0].y;
    b.z = v[2].z - v[0].z;
	
    // calculate the cross product and place the resulting vector
    // into the address specified by vertex_t *normal
    normal->x = (a.y * b.z) - (a.z * b.y);
    normal->y = (a.z * b.x) - (a.x * b.z);
    normal->z = (a.x * b.y) - (a.y * b.x);
	
    // normalize
    normalize(normal);
}


static void averageVector(const vertex3 *a, const vertex3 *b, vertex3 *c) 
{
	c->x = (a->x + b->x)*0.5;
	c->y = (a->y + b->y)*0.5;
	c->z = (a->z + b->z)*0.5;
}

#endif
