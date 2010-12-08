#ifndef TRAP_CONFIG
#define TRAP_CONFIG

#include "../Common/Types.h"
#include "../Side Libraries/XmlParser/tinyxml.h"
#include "Console.h"
extern Console console;

struct Config {
public:
	/// Electric field frequency
	real	f;
	/// Resonator (camera) length and diameter
	real	cameraL, cameraD;
	/// Electrons number in the camera
	sint	particlesNumber;
	/// Electric field array size
	sint	EFieldSize;
	/// Magnetic field array sizes
	sint	BFieldSizeX, BFieldSizeY, BFieldSizeZ;
	/// Number of iterations in the motion and the step
	lint	kt, kd;
	/// Beginning E and B
	real	E, B00;
	/// Beginning electron position, impulse and angle fi
	real	x0, y0, z0, W0, fi;

	/// Initial distribution
	real	xInitRadius, yInitRadius, zInitRadius;
	
	/// Data files for magnetic fields
	string coilsFilename, hexFilename;

public:
	/// Default values
	Config();

	/// Read settings from main
	bool ReadSettings(string filename);

private:
	TiXmlElement *ele;
	inline real readFromXml(const char *param);
	
};

#endif