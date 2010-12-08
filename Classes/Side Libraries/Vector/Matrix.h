#ifndef __MATRIX__
#define __MATRIX__

#include	"Vector.h"

class Matrix
{
public:
	double x [4][4];

	Matrix () {};
	Matrix ( double );

	Matrix& operator += ( const Matrix& );
	Matrix& operator -= ( const Matrix& );
	Matrix& operator *= ( const Matrix& );
	Matrix& operator *= ( double );
	Matrix& operator /= ( double );

	void	Invert ();
	void	Transpose ();
	friend	Matrix operator + ( const Matrix&, const Matrix& );
	friend	Matrix operator - ( const Matrix&, const Matrix& );
	friend	Matrix operator * ( const Matrix&, double );
	friend	Matrix operator * ( const Matrix&, const Matrix& );
	friend	Vector operator * ( const Matrix&, const Vector& );
};

Matrix	Translate ( const Vector& );
Matrix	Scale     ( const Vector& );
Matrix	RotateX   ( double );
Matrix	RotateY   ( double );
Matrix	RotateZ   ( double );
Matrix	Rotate    ( const Vector& v, double );
Matrix	MirrorX   ();
Matrix	MirrorY   ();
Matrix	MirrorZ   ();

#endif
