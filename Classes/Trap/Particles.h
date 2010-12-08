#ifndef TRAP_PARTICLES
#define TRAP_PARTICLES

#include "Config.h"
#include "math.h"
extern Config conf;


/// Particles
struct Particles {
private: 
	/// Particles number
	sint number;
	/// Initial distribution
	real xInitRadius, yInitRadius, zInitRadius, x0, y0, z0;
	/// maximum and minimum energy
	real energyMax, energyMin;
	
public:
	/// Electron positions and impulses: x[0] ~ x, x[1] ~ y, x[2] ~ z
	vector<real> x, u;
	/// Particles energy
	vector<real> gm;
	/// Indexes of arrays x and u
	struct Index { size_t x,y,z; bool dead; };
	vector<Index> index;

public:
	/// Default constructor
	Particles();

	/// Set the particles number
	void setParticlesNumber(sint n);
	/// Generate initial particles distribution
	void generateDistribution(real rl, real u0);
	/// Get number of particles
	inline sint getNumber() { return number; } 
	/// Get energy max and min
	void findMinMaxEnergy();
	/// Calculate energy for every particle
	void calculateEnergy();
	
	inline real getEnergyMax() { return energyMax; }
	inline real getEnergyMin() { return energyMin; }
};

#endif