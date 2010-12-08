#ifndef TRAP_ELECTRIC_FIELD
#define TRAP_ELECTRIC_FIELD

#include "Config.h"
#include "math.h"
extern Config conf;


/// UHF Electric Field in the camera
struct ElectricField {
public: 
	/// Electric field components (x, y, z)
	vector<float4>	g;

private:
	/// UHF electric field period 
	sint	period;
	real	dt;

public:

	/// Default constructor
	ElectricField();
	/// Create an electric field with the multi mode
	void createMultiMode(real g0);

public:
	inline real getDt() { return dt; }
	inline sint getPeriod() { return period; }

};

#endif