#include "CMobileCam.h"
#include "math/CVector3d.h"

chai3d::cMobileCam::cMobileCam(cWorld* world, const cVector3d &eye, const cVector3d &target, const cVector3d &up) : cCamera(world)

{
	this->set(eye, target, up);
	this->eye = eye;
	this->target = target;
	this->up = up;
	this->inMovement = false;
}

void chai3d::cMobileCam::moveCam()

{
	this->inMovement = true;
	// position and orient the camera
	this->set(cVector3d(0.3, 0.3, 0.05),	// camera position (eye)
		cVector3d(0.00, 0.00, 0.00),	// lookat position (target)
		cVector3d(0.00, 0.00, 1.00));   // direction of the (up) vector
}
//void chai3d::moveCam(const cVector3d &eye, const cVector3d &target, const cVector3d &up)

//{
	
//}

