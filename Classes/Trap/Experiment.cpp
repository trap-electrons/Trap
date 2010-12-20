#include "Experiment.h"


//-----------------------------------------------------------------------------
Experiment :: Experiment(const string inputFileName, const char *defaultDir) 
{
	defaultDirectory = defaultDir+string("/");
	inputFile = inputFileName;
	if (!conf.ReadSettings(inputFile))
		if (!conf.ReadSettings(defaultDirectory + string("Main.xml"))) {
			console.warning("Can't open the main input file!");
			printf("%s", defaultDirectory.c_str());
		}

	cameraL = conf.cameraL;
	cameraD = conf.cameraD;
	isRunning = false;
	shouldStop = false;
	
	BField = new MagneticField();
	EField = new ElectricField();
	particles = new Particles();
}

//-----------------------------------------------------------------------------
Experiment :: ~Experiment()
{
	delete BField;
	delete EField;
	delete particles;
}


//-----------------------------------------------------------------------------
void Experiment :: start() 
{
	console.message("Starting");

	console.printInt("Particles count: ", particles->getNumber());
	
	isRunning = true;
	shouldStop = false;
	
	// Determinate parameters
	cameraD = conf.cameraD; 
	cameraL = conf.cameraL;
	dz = cameraL / float(conf.BFieldSizeZ-1);
	dx = cameraD / float(conf.BFieldSizeX-1); 
	invDx = 1.0/dx;
	invDz = 1.0/dz;	
	
	fi = conf.fi/180.0*pi;
	v0 = sqrt(1.-1./sqr(1.+conf.W0/511000.0));	// начальная скорость электрона в единицах С
	u0 = v0/sqrt(1.0-sqr(v0));					// начальный импульс электрона с единицах mc
	w00 = 511000.0*( sqrt(1.0+sqr(u0)) -1.0);	// тест для начальной энергии
	om = 2.0*pi*conf.f;
	rl = c/om;									// релятивистский радиус циклотронного вращения
	invDxRl = invDx*rl;
	invDyRl = invDx*rl;
	invDzRl = invDz*rl;
	halfCameraLDivRl = 0.5 * cameraL / rl; 
	piDivRl = pi / rl;
	piDivRlPow2 = piDivRl*piDivRl;
	E = conf.E * 1.0e05/3.0e04;					// Амплитуда СВЧ поля в СГСЭ
	g0 = ez*E/(me*c*om);
	b0 = me*c*om/ez;							//	Резонансное значение индукции магнитного поля 
	wmax = 511.0*2.0 * pow(g0,real(2.0/3.0));
	particlesNumber = particles->getNumber();
	halfX = (conf.BFieldSizeX)/2; halfY=halfX; halfZ = (conf.BFieldSizeZ)/2;
	console.printReal("U0 = ", u0);	
	console.printReal("B0 = ", b0);
	console.printReal("w_max = ", 2.0 * pow(g0,real(2.0/3.0)) * 511.0);

	// Magnetic fields loading and mixing
	console.message("Reading magnetic fields");
	if (!BField->readFromFileCoilsField(defaultDirectory+conf.coilsFilename))
		console.warning("Can't read the coils magnetic field");
	if (!BField->readFromFileHexField(defaultDirectory+conf.hexFilename))
		console.warning("Can't read the hexapole magnetic field");
	console.message("Mixing magnetic fields");
	BField->mixMagneticFields(b0);

	// Electric fields creation
	console.message("Creation electric fields");
	EField->createMultiMode(g0);

	// Initial distribution of particles
	console.message("Generate the initial distribution of particles");
	particles->generateDistribution(rl, u0);
	
	// Perform experiment
	prevProcessed = -1;
	//real kp = 0, km = 0;
	kd = conf.kd;
	b00 = conf.B00;
	kt = conf.kt;
	dt = EField->getDt();

	maxDumpTime = kt / kd;	
	currentDumpTime = 0;
	prepareExperiment();
	beginTime = clock();
	processExperiment();
	endTime = clock();
	finishExperiment();

	console.printTime(endTime-beginTime);
}

//-----------------------------------------------------------------------------
void Experiment :: processExperiment() 
{ 
	for (sint k=0; k<kt; k++) 
	{
		sint processed = sint(real(k)/real(kt-1)*100);
		if (processed != prevProcessed) console.printProcessed("Experiment processing", processed);
		prevProcessed = processed;

		sint l = k % EField->getPeriod();	// Расчет индекса СВЧ поля
		//real tm = (k+1) * dt;			// Текущее время
		real tp = -(1.0+b00)*dt*0.5;	// Базовое магнитное поле
	    
		motion(l, tp);
				
		if (k%kd==0) { 
			particles -> calculateEnergy();
			particles -> findMinMaxEnergy();
			currentDumpTime++;
		}
		if (shouldStop) { return; }
	}
}


//-----------------------------------------------------------------------------
void Experiment :: finishExperiment() 
{
	particles -> calculateEnergy();
	particles -> findMinMaxEnergy();
	currentDumpTime++;
	isRunning = false;
}

//-----------------------------------------------------------------------------
void Experiment :: prepareExperiment()
{ 
	/*for (int i=0; i<particlesNumber; i++) {
		printf("%f :: %f :: %f \n", 
			   particles->x[ particles -> index[i].x ], 
			   particles->x[ particles -> index[i].y ],
			   particles->x[ particles -> index[i].z ]);
	}*/
}


//-----------------------------------------------------------------------------
void Experiment :: motion(const sint l, const real tp)
{
	real *x =  &particles->x[0], *u = &particles->u[0], *gm = &particles->gm[0]; 
	const real ex = EField->g[l].x, ey = EField->g[l].y, ez = EField->g[l].z;

	#pragma omp parallel for
	for (sint m=0; m<particlesNumber; m++) 
	{
		if (particles->index[m].dead==1) continue;
		const sint	
			ix=particles->index[m].x,
			iy=particles->index[m].y, 
			iz=particles->index[m].z;

	    real bp[3], t[3], sum;

		const real 
			xn = x[ix]*invDxRl + halfX,
			yn = x[iy]*invDyRl + halfY,
			zn = x[iz]*invDzRl + halfZ;

		const sint 
			i = xn,
			j = yn,
			k = zn,
			i1 = i+1,
			j1 = j+1,
			k1 = k+1;

		const real 
			dx0 = xn-i,
			dy0 = yn-j,
			dz0 = zn-k,
			dx1 = 1.0-dx0,
			dy1 = 1.0-dy0,
			dz1 = 1.0-dz0;

		const real 
			dy0dz0 = dy0*dz0, 
			dy1dz0 = dy1*dz0,
			dy0dz1 = dy0*dz1,
			dy1dz1 = dy1*dz1;

		const real 
			v1=dx0*dy0dz0,
			v2=dx1*dy0dz0,
			v3=dx0*dy1dz0,
			v4=dx1*dy1dz0,
			v5=dx0*dy0dz1,
			v6=dx1*dy0dz1,
			v7=dx0*dy1dz1,
			v8=dx1*dy1dz1;
		
		// bxp, byp, bzp - поля в точках расположения частиц
		bp[0] = 
			BField->bx[i][j][k]*v8+BField->bx[i1][j][k]*v7+
			BField->bx[i][j1][k]*v6+BField->bx[i1][j1][k]*v5+
			BField->bx[i][j][k1]*v4+BField->bx[i][j1][k1]*v2+
			BField->bx[i1][j][k1]*v3+BField->bx[i1][j1][k1]*v1;

		bp[1] = 
			BField->by[i][j][k]*v8+BField->by[i1][j][k]*v7+
			BField->by[i][j1][k]*v6+BField->by[i1][j1][k]*v5+
			BField->by[i][j][k1]*v4+BField->by[i][j1][k1]*v2+
			BField->by[i1][j][k1]*v3+BField->by[i1][j1][k1]*v1;
		
		bp[2] = 
			BField->bz[i][j][k]*v8+BField->bz[i1][j][k]*v7+
			BField->bz[i][j1][k]*v6+BField->bz[i1][j1][k]*v5+
			BField->bz[i][j][k1]*v4+BField->bz[i][j1][k1]*v2+
			BField->bz[i1][j][k1]*v3+BField->bz[i1][j1][k1]*v1;

		
		// Схема Бориса интегрированиня ур-ний движения
		const real 
			uxm = u[ix] + ex,
			 uym = u[iy] + ey,
			 uzm = u[iz] + ez;

		const real 
			gmn = sqrt(1.0 + uxm*uxm + uym*uym + uzm*uzm),
			tg = tp/gmn;
		for (sint q=0; q<3; q++) t[q] = tg * bp[q];

		sum = 1.0;
		for(sint q=0; q<3; q++) sum += sqr(t[q]);
		const real 
			inv_txyzr = 2.0/(sum),
			sx = t[0] * inv_txyzr,
			sy = t[1] * inv_txyzr,
			sz = t[2] * inv_txyzr;

		const real
			uxr = uxm + uym*t[2] - uzm*t[1],
			uyr = uym + uzm*t[0] - uxm*t[2],
			uzr = uzm + uxm*t[1] - uym*t[0];

		const real 
			uxp = uxm + uyr*sz - uzr*sy,
			uyp = uym + uzr*sx - uxr*sz,
			uzp = uzm + uxr*sy - uyr*sx;

		u[ix] = uxp + ex;
		u[iy] = uyp + ey; 
		u[iz] = uzp + ez;

		gm[m] = sqrt(1.0 + sqr(u[ix]) + sqr(u[iy]) + sqr(u[iz])); 
		const real gt = dt/gm[m];
		
		x[ix] += u[ix]*gt;
		x[iy] += u[iy]*gt;
		x[iz] += u[iz]*gt;

		if (fabsf(x[iz]) >= halfCameraLDivRl
			|| sqr(x[ix])+sqr(x[iy]) >= piDivRlPow2) 
		{
			particles->index[m].dead = 1;
		}
	} // for

}


void Experiment :: getExperimentDump()
{
	currentDumpTime++;
}