#include "ElectricField.h"

//-----------------------------------------------------------------------------
ElectricField :: ElectricField() 
{
	g.resize(conf.EFieldSize);
	period = conf.EFieldSize;
	dt = 2.0*pi/real(period);
}

//-----------------------------------------------------------------------------
/// Create an electric field with the multi mode
void ElectricField :: createMultiMode(real g0)
{
	real dt = 2.0*pi/real(period);
	for (sint i=0; i<g.size(); i++)
	{
		 g[i].x = g0 * sin(2.0*pi*(i)/real(period))*dt*0.5;
		 g[i].y = g0 * sin(2.0*pi*(i)/real(period))*dt*0.5;
		 g[i].z = g0 * sin(2.0*pi*(i)/real(period))*dt*0.5;
		 g[i].w = 0;
	}
}