#include "Particles.h"


//-----------------------------------------------------------------------------
Particles :: Particles() 
{
	setParticlesNumber(conf.particlesNumber);
	energyMax=0; energyMin=1.0;

	index.resize(number);
	for (size_t i=0; i<number; i++)
	{
		index[i].x = i*3; 
		index[i].y = index[i].x+1; 
		index[i].z = index[i].x+2; 
		index[i].dead = 0;
	}
}


//-----------------------------------------------------------------------------
void Particles :: setParticlesNumber(sint n)
{
	number = n;

	x.resize(n*3);
	u.resize(n*3);
	gm.resize(n);
}


//-----------------------------------------------------------------------------
void Particles :: generateDistribution(real rl, real u0)
{
	xInitRadius = conf.xInitRadius;
	yInitRadius = conf.yInitRadius;
	zInitRadius = conf.zInitRadius;
	x0 = conf.x0;
	y0 = conf.y0;
	z0 = conf.z0;

	for (sint i=0; i<number; i++)
	{
		real ss  = real(rand())/real(RAND_MAX); 
		real sss = real(rand())/real(RAND_MAX);
		real ssz = real(rand())/real(RAND_MAX);
		
		sint p = i*3;

		x[p  ] = ((x0+xInitRadius*sss)*cos(2.*pi*(ss-0.5)))/rl;
		x[p+1] = ((y0+yInitRadius*sss)*sin(2.*pi*(ss-0.5)))/rl;
		x[p+2] = (z0+zInitRadius*(ssz-0.5))/rl;

		u[p  ] = u0*cos(2.0*pi*(ss-0.5));
		u[p+1] = u0*sin(2.0*pi*(ss-0.5));
		u[p+2] = 0.5*u0*sin(2.0*pi*(ss-0.5));
	}
}

//-----------------------------------------------------------------------------
void Particles :: calculateEnergy()
{
	#pragma omp parallel for
	for (size_t i=0; i<gm.size(); i++) {
		gm[i] = 1.0 
			+ sqr(u[index[i].x])
			+ sqr(u[index[i].y])
			+ sqr(u[index[i].z]);
	}
}


//-----------------------------------------------------------------------------
void Particles :: findMinMaxEnergy()
{
	if (energyMax==0) energyMax = energyMin = gm[0];
	for (size_t i=0; i<gm.size(); i++) {
		if (energyMax<gm[i]) energyMax = gm[i];
		if (energyMin>gm[i]) energyMin = gm[i];
	}
}