#ifndef TRAP_EXPERIMENT_FIELD
#define TRAP_EXPERIMENT_FIELD

#include "Particles.h"
#include "ElectricField.h"
#include "MagneticField.h"

#include "Console.h"
#include "Config.h"
#include "time.h"
#include "math.h"

extern Config conf;
extern Console console;


/// Main object. All the mathematical calculations are here. 
class Experiment {
public:
	ElectricField	*EField;
	MagneticField	*BField;
	Particles		*particles;

	/// Processing time 
	clock_t beginTime, endTime;
	sint prevProcessed;
	sint currentDumpTime, maxDumpTime;
	bool isRunning, shouldStop;

	/// Basic parameters
	real fi, v0, u0, om, w00, g0, rl, E, wmax, b0, dt, b00;
	sint kd, kt;
	real dz, dx, invDx, invDz, invDxRl, invDyRl, invDzRl, piDivRlPow2;
	sint particlesNumber;
	sint halfX, halfY, halfZ;
	real cameraL, cameraD, halfCameraLDivRl, piDivRl;

	/// Input file name
	string inputFile;
	/// Default directory
	string defaultDirectory;

public:
	/// Default constructor
	Experiment(const string inputFileName, const char *defaultDir);
	~Experiment();
	void start();
	void stop() { shouldStop = true; }
	bool isExperimentRunning() { return isRunning; }
	void setDefaultDirectory(char *s) { defaultDirectory = s; }
	
protected:
	/// Some preparations
	virtual void prepareExperiment();
	/// After experiment processing
	virtual void processExperiment();
	/// After experiment processing
	virtual void finishExperiment();
	/// Motion procedure: main function which starts the experiment processing
	inline virtual void motion(sint l, real tp);
	/// Experiment dump in time
	virtual void getExperimentDump();
};


#endif