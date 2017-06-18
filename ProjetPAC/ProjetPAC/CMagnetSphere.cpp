#include "CMagnetSphere.h"

chai3d::cMagnetSphere::cMagnetSphere(double a_radius, double a_mass, double a_charge, double a_damping, int a_courseSize) : cShapeSphere(a_radius)
{
	setMass(a_mass);
	setCharge(a_charge);
	setDamping(a_damping);

	setSpeed(0.0, 0.0, 0.0);
	setAcceleration(0.0, 0.0, 0.0);

	// set the number of dots in the course
	this->m_courseSize = a_courseSize;
	// set the initial id of the course's front point
	this->m_courseFront = 0;
}

void chai3d::cMagnetSphere::updateCourse()
{
	m_course[m_courseFront]->setLocalPos(this->getLocalPos());
	m_courseFront = (m_courseFront + 1) % m_courseSize;
}

void chai3d::cMagnetSphere::resetCourse()
{
	for (int i = 0; i < m_courseSize; i++) {
		m_course[i]->setLocalPos(this->getLocalPos());
	}
}