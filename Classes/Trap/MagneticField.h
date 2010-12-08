#ifndef TRAP_MAGNETIC_FIELD
#define TRAP_MAGNETIC_FIELD

#include "Config.h"
extern Config conf;


//-----------------------------------------------------------------------------
#define ARRAY3D_RESIZE(massive)\
	massive.resize( conf.BFieldSizeX );\
	for (sint i=0; i<massive.size(); i++) {\
		massive[i].resize( conf.BFieldSizeY );\
		for (sint j=0; j<massive[i].size(); j++)\
			massive[i][j].resize( conf.BFieldSizeZ ); }
//-----------------------------------------------------------------------------
#define ARRAY2D_RESIZE(massive)\
	massive.resize( conf.BFieldSizeX );\
	for (sint i=0; i<massive.size(); i++) {\
		massive[i].resize( conf.BFieldSizeY );\
	}
//-----------------------------------------------------------------------------


/// Magnetic Field class
struct MagneticField {
private: 
	/// Coils magnetic field
	vector< vector< vector<real> > > bxc, byc, bzc;
	/// Hexapole magnetic field
	vector< vector<real> > bxh, byh;
	
public:
	/// Integral magnetic field
	vector< vector< vector<real> > > bx, by, bz;

public:

	/// Default constructor
	MagneticField();

private:

	/// Helpful function
	inline void getArray3DFromFile(fstream &f, vector< vector< vector<real> > > &a);
	/// Helpful function
	inline void getArray2DFromFile(fstream &f, vector< vector<real> > &a);

public:
	
	/// Read coils magnetic field from file
	bool readFromFileCoilsField(string filename);
	/// Read hexapole magnetic field from file
	bool readFromFileHexField(string filename);
	/// Mixing of all the magnetic fields (hexapole, coils, ...)
	void mixMagneticFields(real b0);

};

#endif