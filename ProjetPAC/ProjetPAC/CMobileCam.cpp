#include "CMobileCam.h"
#include "math/CVector3d.h"
#include "math/CMatrix3d.h"
#include <cmath>

chai3d::cMobileCam::cMobileCam(cWorld* world, const cVector3d &eye, const cVector3d &target, const cVector3d &up) : cCamera(world)

{
	this->set(eye, target, up);
	this->rotation.identity();
	this->inMovement = false;
	this->step = 0;
	this->state = 1;
}

void chai3d::cMobileCam::moveCam()

{
	if (this->step == 30 * (this->state + 1) / 2)//dans le sens aller on s'arrete pour step==30, dans le sens retour pour step==0
	{
		this->inMovement = false;
		this->state *= -1;
	}
	else
	{
		this->step += this->state;
		double alpha = this->step * C_PI / (30 * 4);
		// position and orient the camera
		this->set(cVector3d(0.4*cCosRad(alpha), 0.4*cSinRad(alpha), step * 0.15 / 30),	// camera position (eye)
			cVector3d(-step * 0.1 / 30, 0.0, 0.0),	// lookat position (target)
			cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

		// on calcule la matrice rotation pour orienter correctement les vecteurs dans la barre
		const double beta = cAngle(cVector3d(1.0, 0.0, 0.0), cVector3d(0.4*cCosRad(alpha) + step*0.1/30, 0.4*cSinRad(alpha), 0.0));
		this->rotation = cMatrix3d(0.0, 0.0, 1.0, -beta);
		//printf((cStr(beta)+"\n").c_str());//debug
		//printf((this->rotation.str()+"\n").c_str());//debug
	}
	
}


