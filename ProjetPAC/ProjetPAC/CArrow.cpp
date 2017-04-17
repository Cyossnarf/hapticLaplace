#include "CArrow.h"
#include "graphics\CPrimitives.h"
#include <iostream>
#include <cmath>

chai3d::cArrow::cArrow(const cVector3d &pos, double shaft_radius, double tip_radius, double len, double tip_rel_len) : cMesh()

{
	this->length = len;
	this->tip_len = len * tip_rel_len;
	this->shaft_len = len * (1 - tip_rel_len);

	// create the shaft
	cCreateCylinder(this, this->shaft_len, shaft_radius);
	setLocalPos(pos);
	// configure the appearance
	m_material->setBlue();

	// create the tip
	cMesh* tip = new cMesh();
	this->addChild(tip);
	cCreateCone(tip, this->tip_len, tip_radius);
	tip->setLocalPos(cVector3d(0.0, 0.0, this->shaft_len));
	// configure the appearance
	tip->m_material->setBlue();

	// set the orientation of the arrow
	rotateAboutGlobalAxisRad(cVector3d(0.0, 1.0, 0.0), C_PI_DIV_2);
	
	/*
	cCreateArrow(this, len, shaft_radius, this->tip_len, tip_radius, false, 32U, this->direction);
	setLocalPos(pos);
	*/

	// disable haptic effects
	setHapticEnabled(false);
	
}

chai3d::cArrow::~cArrow()

{
	clear();
}

void chai3d::cArrow::updateArrow(const cVector3d &reference)

{
	double alpha = atan2(reference.z(), reference.y());
	setLocalRot(cMatrix3d(alpha - C_PI_DIV_2, 0.0, 0.0, C_EULER_ORDER_XYZ));
}