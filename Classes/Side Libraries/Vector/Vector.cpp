#include	<math.h>
#include	<stdlib.h>
#include	"Vector.h"

Vector	operator ^ ( const Vector& u, const Vector& v )
{
	return Vector ( u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x );
}

Vector	RndVector ()
{
	Vector	v ( rand () - 0.5*RAND_MAX, rand () - 0.5*RAND_MAX, rand () - 0.5*RAND_MAX );

	return Normalize ( v );
}

Vector&	Clip ( Vector& v )
{
	if ( v.x < 0.0 )
		v.x = 0.0;
	else
	if ( v.x > 1.0 )
		v.x = 1.0;

	if ( v.y < 0.0 )
		v.y = 0.0;
	else
	if ( v.y > 1.0 )
		v.y = 1.0;

	if ( v.z < 0.0 )
		v.z = 0.0;
	else
	if ( v.z > 1.0 )
		v.z = 1.0;

	return v;
}
