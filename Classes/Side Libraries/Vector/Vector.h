//////////////////////////////////////////////////////////
// basic vector library                                 //
//    all major operators overloaded to support vectors //
// Author:   Alex V. Boreskoff                          //
// Last revision: 2/12/94                               //
//////////////////////////////////////////////////////////

#ifndef	__VECTOR__
#define	__VECTOR__

#include	<math.h>

class	Vector
{
public:
	double	x, y, z;

	Vector () {};
	Vector ( double v ) { x = y = z = v; };
	Vector ( const Vector& v ) { x = v.x; y = v.y; z = v.z; };
	Vector ( double vx, double vy, double vz ) { x = vx; y = vy; z = vz; };

	Vector&	operator = ( const Vector& v ) { x = v.x; y = v.y; z = v.z; return *this; };
	Vector& operator = ( double f ) { x = y = z = f; return *this; };
	Vector  operator - () const;

	Vector&	operator += ( const Vector& );
	Vector& operator -= ( const Vector& );
	Vector&	operator *= ( const Vector& );
	Vector& operator *= ( double );
	Vector&	operator /= ( double );

	friend	Vector operator + ( const Vector&, const Vector& );
	friend	Vector operator - ( const Vector&, const Vector& );
	friend	Vector operator * ( const Vector&, const Vector& );
	friend	Vector operator * ( double, const Vector&  );
	friend	Vector operator * ( const Vector&, double  );
	friend	Vector operator / ( const Vector&, double  );
	friend	Vector operator / ( const Vector&, const Vector& );
	friend	double operator & ( const Vector& u, const Vector& v ) { return u.x*v.x + u.y*v.y + u.z*v.z; };
	friend	Vector operator ^ ( const Vector&, const Vector& );

	double	operator !  () { return (double) sqrt ( x*x + y*y + z*z ); };
	double&	operator [] ( int n ) { return * ( &x + n ); };
	int	operator <  ( double v ) { return x < v && y < v && z < v; };
	int	operator >  ( double v ) { return x > v && y > v && z > v; };
};

class	Ray
{
public:
	Vector	Org;
	Vector	Dir;				// direction must be normalyzed

	Ray () {};
	Ray ( Vector& o, Vector& d ) { Org = o; Dir = d; };

	Vector	Point ( double t ) { return Org + Dir*t; };
};

//////////////////// implementation /////////////////////////

inline	Vector	Vector :: operator - () const
{
	return Vector ( -x, -y, -z );
}

inline	Vector	operator + ( const Vector& u, const Vector& v )
{
	return Vector ( u.x + v.x, u.y + v.y, u.z + v.z );
}

inline	Vector	operator - ( const Vector& u, const Vector& v )
{
	return Vector ( u.x - v.x, u.y - v.y, u.z - v.z );
}

inline	Vector	operator * ( const Vector& u, const Vector& v )
{
	return Vector ( u.x * v.x, u.y * v.y, u.z * v.z );
}

inline	Vector	operator * ( const Vector& u, double f )
{
	return Vector ( u.x * f, u.y * f, u.z * f );
}

inline	Vector	operator * ( double f, const Vector& v )
{
	return Vector ( f * v.x, f * v.y, f * v.z );
}

inline	Vector	operator / ( const Vector& v, double f )
{
	return Vector ( v.x / f, v.y / f, v.z / f );
}

inline	Vector	operator / ( const Vector& u, const Vector& v )
{
	return Vector ( u.x / v.x, u.y / v.y, u.z / v.z );
}

inline	Vector&	Vector :: operator += ( const Vector& v )
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

inline	Vector&	Vector :: operator -= ( const Vector& v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

inline	Vector&	Vector :: operator *= ( double v )
{
	x *= v;
	y *= v;
	z *= v;

	return *this;
}

inline	Vector&	Vector :: operator *= ( const Vector& v )
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

inline	Vector&	Vector :: operator /= ( double v )
{
	x /= v;
	y /= v;
	z /= v;

	return *this;
}

/////////////////////////// Functions /////////////////////////////////

inline	Vector	Normalize ( Vector& v ) { return v / !v; };
Vector	RndVector ();
Vector&	Clip ( Vector& );
#endif
