#include "CMobileCam.h"
#include "math/CVector3d.h"
#include <cmath>

chai3d::cMobileCam::cMobileCam(cWorld* world, const cVector3d &eye, const cVector3d &target, const cVector3d &up) : cCamera(world)

{
	this->set(eye, target, up);
	this->eye = eye;
	this->target = target;
	this->up = up;
	this->inMovement = false;
	this->step = 0;
}

void chai3d::cMobileCam::moveCam()

{
	if (this->step == 30)
	{
		this->inMovement = false;
		this->step = 0;
	}
	else
	{
		this->step += 1;
		double alpha = this->step * C_PI / (30 * 4);
		// position and orient the camera
		this->set(cVector3d(0.4*cCosRad(alpha), 0.4*cSinRad(alpha), step * 0.15 / 30),	// camera position (eye)
			cVector3d(-step * 0.1 / 30, 0.0, 0.0),	// lookat position (target)
			cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector
	}
	
}


