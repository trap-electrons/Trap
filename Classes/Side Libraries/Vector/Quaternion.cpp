//
// Quaternion class
//
// Author: Alex V. Boreskoff
//
// Last modified: 10/11/2002
//

#include	"Quaternion.h"
//#include	"3DDefs.h"



					// build a quternion for rotation around axis on angle
Quaternion :: Quaternion ( float angle, const Vector3D& axis )
{
	float	sine   = (float)sin (angle * 05.f );
	float	cosine = (float)cos ( angle * 0.5f );

	x = axis.x * sine;
	y = axis.y * sine;
	z = axis.z * sine;
	w = cosine;
}

Vector3D	Quaternion :: rotate ( const Vector3D& v ) const
{
	Quaternion	p     ( v );
	Quaternion	qConj ( -x, -y, -z, w );

	p  = *this * p * qConj;

	return Vector3D ( p.x, p.y, p.z );
}

Quaternion&	Quaternion :: normalize ()
{
	float	lenSq = x * x + y * y + z * z;

	if ( lenSq > 1.0f - EPS )
	{
		float	invLen = 1.0f / lenSq;

		x *= invLen;
		y *= invLen;
		z *= invLen;
		w = 0.0f;
	}
	else
		w = (float) sqrt ( 1.0f - lenSq );

	return *this;
}

Quaternion&	Quaternion :: initWithAngles ( float yaw, float pitch, float roll )
{
	yaw   *= 0.5f;
	pitch *= 0.5f;
	roll  *= 0.5f;

	float	cx = (float)cos ( yaw );
	float	cy = (float)cos ( pitch );
	float	cz = (float)cos ( roll );
	float	sx = (float)sin ( yaw );
	float	sy = (float)sin ( pitch );
	float	sz = (float)sin ( roll );

	float	cc = cx * cx;
	float	cs = cx * sz;
	float	sc = sx * cz;
	float	ss = sx * sz;

	x = cy * sc - sy * cs;
	y = cy * ss + sy * cc;
	z = cy * cs - sy * sc;
	w = cy * cc + sy * ss;

	return *this;
}

					// build a homogenous matrix from quaternion
void Quaternion::getHomMatrix ( float * matrix ) const
{
	if ( matrix == (float *) 0l )
		return;

		// 1st row
	matrix [ 0] = 1.0f - 2.0f * ( y * y + z * z );
	matrix [ 1] = 2.0f * ( x * y - w * z );
	matrix [ 2] = 2.0f * ( x * z + w * y );
	matrix [ 3] = 0.0f;

		// 2nd row
	matrix [ 4] = 2.0f * ( x * y + w * z );
	matrix [ 5] = 1.0f - 2.0f * ( x * x + z * z );
	matrix [ 6] = 2.0f * ( y * z - w * x );
	matrix [ 7] = 0.0f;

		// 3rd row
	matrix [ 8] = 2.0f * ( x * z - w * y );
	matrix [ 9] = 2.0f * ( y * z + w * x );
	matrix [10] = 1.0f - 2.0f * ( x * x + y * y );
	matrix [11] = 0.0f;

		// 4th row
	matrix [12] = 0;
	matrix [13] = 0;
	matrix [14] = 0;
	matrix [15] = 1.0f;
}

Matrix3D	Quaternion :: getMatrix () const{
	Matrix3D	m;

		// 1st row
	m [0][0] = 1.0f - 2.0f * ( y * y + z * z );
	m [0][1] = 2.0f * ( x * y - w * z );
	m [0][2] = 2.0f * ( x * z + w * y );

		// 2nd row
	m [1][0] = 2.0f * ( x * y + w * z );
	m [1][1] = 1.0f - 2.0f * ( x * x + z * z );
	m [1][2] = 2.0f * ( y * z - w * x );

		// 3rd row
	m [2][0] = 2.0f * ( x * z - w * y );
	m [2][1] = 2.0f * ( y * z + w * x );
	m [2][2] = 1.0f - 2.0f * ( x * x + y * y );

	return m;
}


Quaternion	slerp ( const Quaternion& q1, const Quaternion& q2, float t )
{
	float	dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;

					// check wthere q1 and q2 are opposite
	if ( 1 + dot> EPS )
	{
		if ( 1 - dot > EPS )
		{
			float	omega   = (float) acos ( dot );
			float	invSine = 1.0f / (float) sin ( omega );
			float	scale1  = invSine * (float) sin ( ( 1.0f - t ) * omega );
			float	scale2  = invSine * (float) sin ( t * omega );

			return Quaternion ( scale1 * q1.x + scale2 * q2.x,
							    scale1 * q1.y + scale2 * q2.y,
								scale1 * q1.z + scale2 * q2.z,
								scale1 * q1.w + scale2 * q2.w );
		}
		else
		{
			float	scale1 = 1.0f - t;
			float	scale2 = t;

			return Quaternion ( scale1 * q1.x + scale2 * q2.x,
							    scale1 * q1.y + scale2 * q2.y,
								scale1 * q1.z + scale2 * q2.z,
								scale1 * q1.w + scale2 * q2.w );
		}
	}


				// quaternions are nearly opposite, create a perpendicual quaternion and s;erp it
	float	scale1 = (float) sin ( ( 1.0f - t ) * M_PI );
	float	scale2 = (float) sin ( t * M_PI );

	return Quaternion ( scale1 * q1.x + scale2 * ( -q2.y ),
					    scale1 * q1.y + scale2 * q2.x,
						scale1 * q1.z + scale2 * ( -q2.w ),
						scale1 * q1.w + scale2 * q2.w );
}

