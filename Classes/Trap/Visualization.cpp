#include "Visualization.h"


//-----------------------------------------------------------------------------
void Visualization :: newVisualArea(shptr<Experiment> exp, sint timerSlow) 
{
	timer = timerSlow;
	thread_type t;
	thread			drawThread;
	unsigned int	drawThreadId;
	shptr<TrapEditor> win(new TrapEditor());
	win->setExperiment(exp);
	win->setSlowMode(timer);
	windows.push_back(win);

	thread_create(t, Visualization::threadStartTrapWindow, win.get(), 0, &drawThreadId);
}


//-----------------------------------------------------------------------------
unsigned int Visualization :: threadStartTrapWindow(void *win)
{
	TrapEditor* t = reinterpret_cast<TrapEditor*>(win);
	
	t -> open(NULL);
	t -> createGraphicObjects();
	t -> dispatch();
	
	return 0;
}
