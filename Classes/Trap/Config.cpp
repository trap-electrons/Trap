#include "Config.h"

//-----------------------------------------------------------------------------
Config :: Config() 
{
	f = 14.0e09;
	cameraL = 24;
	cameraD = 6.4;
	particlesNumber = 10000;
	
	EFieldSize = 250;
	BFieldSizeX = 17;
	BFieldSizeY = 17;
	BFieldSizeZ = 33;

	kt = 10000;
	kd = 10;
	E = 1;
	B00 = 1;
	x0 = 0; y0 = 0; z0 = 0;
	W0 = 1; 
	fi = 0;

	xInitRadius = 1.5; yInitRadius = 1.5; zInitRadius = 16.0;

	coilsFilename = "Coils-XYZ-MB2.dat";
	hexFilename = "Hex-pole-MB.dat";
}


//-----------------------------------------------------------------------------
bool Config :: ReadSettings(string filename) 
{
#define GET_REAL_FROM_XML(param) atof( ele->NextSiblingElement(param)->FirstChild()->Value() );
#define GET_FROM_XML(param) ele->NextSiblingElement(param)->FirstChild()->Value();
	
	TiXmlDocument doc( filename.c_str() );
	if ( !doc.LoadFile() ) return false;
	
	try {
		TiXmlElement* ele = doc.FirstChildElement("Experiment");
		ele = ele->FirstChildElement("kt");

		kt = atof(ele->FirstChild()->Value());
		kd = GET_REAL_FROM_XML("kd");
		E = GET_REAL_FROM_XML("E");
		B00 = GET_REAL_FROM_XML("B00");
		
		particlesNumber = GET_REAL_FROM_XML("ParticlesNumber");

		// Initial Distribution
		TiXmlElement *tmp = ele;
		ele = ele->NextSiblingElement("InitialDistribution");
		ele = ele->FirstChildElement("xRadius");
		xInitRadius = atof(ele->FirstChild()->Value());
		yInitRadius = GET_REAL_FROM_XML("yRadius");
		zInitRadius = GET_REAL_FROM_XML("zRadius");
		x0 = GET_REAL_FROM_XML("x0");
		y0 = GET_REAL_FROM_XML("y0");
		z0 = GET_REAL_FROM_XML("z0");
		W0 = GET_REAL_FROM_XML("W0");
		fi = GET_REAL_FROM_XML("fi");
		ele = tmp;

		// Files of fields
		coilsFilename = GET_FROM_XML("CoilsField");
		hexFilename = GET_FROM_XML("HexField");

	}
	catch (...) { console.error("Can't read Main.xml correctly"); }

	return true;
}


//-----------------------------------------------------------------------------
Config conf;