#include "MagneticField.h"


//-----------------------------------------------------------------------------
MagneticField :: MagneticField() 
{
	ARRAY3D_RESIZE(bxc); // coils
	ARRAY3D_RESIZE(byc);
	ARRAY3D_RESIZE(bzc);

	ARRAY2D_RESIZE(bxh); // hexapole
	ARRAY2D_RESIZE(byh);

	ARRAY3D_RESIZE(bx); // mixed field
	ARRAY3D_RESIZE(by);
	ARRAY3D_RESIZE(bz);
}


//-----------------------------------------------------------------------------
/// Helpful function
inline void MagneticField :: getArray3DFromFile(fstream &f, vector< vector< vector<real> > > &a)
{
	sint ksize = a[0][0].size(), 
		 jsize = a[0].size(), 
		 isize = a.size();
	for (sint k=0; k<ksize; k++) // z				
		for (sint j=0; j<jsize; j++) // y
			for (sint i=0; i<isize; i++) // x
				f >> a[i][j][k];
}


//-----------------------------------------------------------------------------
/// Helpful function
inline void MagneticField :: getArray2DFromFile(fstream &f, vector< vector<real> > &a)
{
	for (sint j=0; j<a[0].size(); j++) // y
		for (sint i=0; i<a.size(); i++) // x
			f >> a[i][j];
}


//-----------------------------------------------------------------------------
/// Read coils magnetic field from file
bool MagneticField :: readFromFileCoilsField(string filename)
{
	fstream f(filename.c_str(), ios_base::in);
	if (!f) return false;

	getArray3DFromFile(f, bxc);
	getArray3DFromFile(f, byc);
	getArray3DFromFile(f, bzc);
	
	
	real tmp = bxc[7][5][1];
	f >> tmp ;

	return true;
}


//-----------------------------------------------------------------------------
/// Read hexapole magnetic field from file
bool MagneticField :: readFromFileHexField(string filename)
{
	fstream f(filename.c_str(), ios_base::in);
	if (!f) return false;

	getArray2DFromFile(f, bxh);
	getArray2DFromFile(f, byh);
	
	return true;
}


//-----------------------------------------------------------------------------
/// Mixing of all the magnetic fields (hexapole, coils, ...)
void MagneticField :: mixMagneticFields(real b0)
{
	for (sint k=0; k<bx[0][0].size(); k++) // z
		for (sint j=0; j<bx[0].size(); j++) // y
			for (sint i=0; i<bx.size(); i++) // x
			{
				bx[i][j][k] = (bxc[i][j][k]+bxh[i][j])/(b0*0.001);
				by[i][j][k] = (byc[i][j][k]+byh[i][j])/(b0*0.001);
				bz[i][j][k] = bzc[i][j][k]/(b0*0.001);

				// Cross section calculate
				/*bt(i,j,k) = sqrt(
					bxc[i][j][k]*bxc[i][j][k]
					+ byc[i,j,k]*byc[i][j][k] + bzc[i][j][k]*bzc[i][j][k] );*/
			}
}