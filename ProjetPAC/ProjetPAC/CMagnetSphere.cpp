#include "CMagnetSphere.h"


chai3d::cMagnetSphere::cMagnetSphere(double radius, double mass, double charge, double damping) : cShapeSphere(radius)
{
	setMass(mass);
	setCharge(charge);
	setDamping(damping);

	setSpeed(0.0, 0.0, 0.0);
	setAcceleration(0.0, 0.0, 0.0);
}

