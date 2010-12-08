#ifndef TRAP_VISUALIZATION
#define TRAP_VISUALIZATION

#include "../Common/threads.h"
#include "../LQEditor/TrapEditor.h"
#include <boost/shared_ptr.hpp>

/// Visual representation of the experiment
struct Visualization {
private:
	list<shptr<TrapEditor>> windows;
	sint timer;

public:
	void newVisualArea(shptr<Experiment> exp, sint timerSlowMode);

private:
	static unsigned int threadStartTrapWindow(void *experiment);

};

#endif
