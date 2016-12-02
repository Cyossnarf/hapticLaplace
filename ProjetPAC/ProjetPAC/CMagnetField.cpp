#include "CMagnetField.h"
#include "graphics\CPrimitives.h"
#include <iostream>
#include <cmath>

#define MU_0 (4 * C_PI * 1e-7)

chai3d::cMagnetField::cMagnetField(const cVector3d &pos, double in_radius, double out_radius, double intensity, double height) : cMesh()

{
	// set properties of the field
	this->fieldEnabled = true;
	this->direction = cVector3d(0.0, 0.0, 1.0);
	this->radial = cVector3d(0.0, 1.0, 0.0);
	this->intensity = intensity;
	this->innerRadius = in_radius;
	this->outerRadius = out_radius;

	cVector3d currentPos(pos);
	double height_sp = 0.008;
	
	double gap_sp = 0.015;
	// create the first turn
	cCreatePipe(this, height_sp, in_radius, out_radius);//cCreateCylinder(this, height_sp, in_radius);
	setLocalPos(currentPos);

	// set the appearance of the first turn
	cTexture2dPtr turnTexture = cTexture2d::create();
	turnTexture->loadFromFile("../resources/images/magnet-color.png");

	setTexture(turnTexture);
	setUseTexture(true);
	
	// create the rest of the turns	(18 en 2015)
	for (int i = 0; i < 18; i++) {
		currentPos.add(gap_sp * direction);
		cMesh* mesh = new cMesh();
		turns.push_back(mesh);
		this->addChild(mesh);
		mesh->setLocalPos(currentPos);
		cCreatePipe(mesh, height_sp, innerRadius, outerRadius);
		mesh->m_material->setBlueLightSky();
		mesh->setTexture(turnTexture);
		mesh->setUseTexture(true);
		mesh->rotateAboutGlobalAxisDeg(cVector3d(0.0, 0.0, 1.0), 50 * cSinRad((i + 1) * C_PI / 18));
	}
	
	/*
	cCreateCylinder(this,
		height_sp,
		in_radius
	);
	tube->m_material->setBlueLightSky();
	tube->setLocalPos(currentPos);
	*/

	// disable haptic effects
	setHapticEnabled(false);

	// configure the appearance
	m_material->setBlueLightSky();
	setTransparency(1.0);

	// set the orientation of the field
	rotateAround(cVector3d(0.0, 1.0, 0.0), -C_PI_DIV_2 * 0.8);
	//rotateAround(cVector3d(0.0, 0.0, 1.0), C_PI_DIV_2 / 8);
}

chai3d::cMagnetField::~cMagnetField()
{
	clear();
}


void chai3d::cMagnetField::rotateAround(cVector3d &rotationAxis, double rotation)

{
	// compute the new direction of the field
	cVector3d cross;
	rotationAxis.crossr(direction, cross);
	direction = cCosRad(rotation) * direction + (1 - cCosRad(rotation)) * (direction.dot(rotationAxis)) * rotationAxis + cSinRad(rotation) * cross;
	rotationAxis.crossr(radial, cross);
	radial = cCosRad(rotation) * radial + (1 - cCosRad(rotation)) * (radial.dot(rotationAxis)) * rotationAxis + cSinRad(rotation) * cross;
	
	// rotate
	rotateAboutGlobalAxisRad(rotationAxis, rotation);
}


bool chai3d::cMagnetField::contain(const cGenericObject &obj) const

{
	cVector3d ortho;
	radial.crossr(direction, ortho);

	cVector3d objPos = obj.getLocalPos();
	cVector3d diff = objPos - getLocalPos();
	
	cVector3d projOrth = diff.dot(radial) * radial + diff.dot(ortho) * ortho;

	return projOrth.length() < innerRadius;

}

chai3d::cVector3d chai3d::cMagnetField::magnetForce(const cGenericObject &obj, double charge, const cVector3d &vel, bool enabled)

{
	cVector3d force(0.0, 0.0, 0.0);
	if (enabled && contain(obj))
	{
		force = charge*vel;
		force.cross((MU_0 * 18 * 1e4 * intensity) * getDirection());//((MU_0 * turns.size() * 1e4 * intensity) * getDirection());
	}

	return force;
}

void chai3d::cMagnetField::setCurrentIntensity(double value)

{
	value = (cAbs(value) < 1e-8) ? 0.0 : value;

	if (value == 0 && intensity > 0)
	{
		for (int i = 0; i < 18; i++)//turns.size->18
		{
			turns[i]->rotateAboutGlobalAxisDeg(cVector3d(0.0, 0.0, -1.0), 50 * cSinRad((i + 1) * C_PI / 18));
		}
	}

	if (value == 0 && intensity < 0)
	{
		for (int i = 0; i < 18; i++)//turns.size->18
		{
			turns[i]->rotateAboutGlobalAxisDeg(cVector3d(0.0, 0.0, 1.0), 50 * cSinRad((i + 1) * C_PI / 18));
		}
	}

	if (value > 0 && intensity == 0)
	{
		for (int i = 0; i < 18; i++)//turns.size->18
		{
			turns[i]->rotateAboutGlobalAxisDeg(cVector3d(0.0, 0.0, 1.0), 50 * cSinRad((i + 1) * C_PI / 18));
		}
	}

	if (value < 0 && intensity == 0)
	{
		for (int i = 0; i < 18; i++)//turns.size->18
		{
			turns[i]->rotateAboutGlobalAxisDeg(cVector3d(0.0, 0.0, -1.0), 50 * cSinRad((i + 1) * C_PI / 18));
		}
	}

	intensity = value;
}

void chai3d::cMagnetField::setTransparency(const double value)

{
	transparency = std::max(0.0, std::min(1.0, value));
	setTransparencyLevel(transparency);
}