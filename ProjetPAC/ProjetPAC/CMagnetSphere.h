#ifndef CMagnetSphereh
#define CMagnetSphereh

#include "world/CShapeSphere.h"

namespace chai3d {

	class cMagnetSphere : public cShapeSphere
	{

	public:

		cMagnetSphere(double radius, double mass = 9.11e-31, double charge = 1.602176565e-19, double damping = 0.999);

		inline void setCharge(double value) { charge = value; }
		inline double getCharge() const { return charge; }

		inline void setMass(double value) { mass = value; }
		inline double getMass() const { return mass; }

		inline void setDamping(double value) { damping = value; }
		inline double getDamping() const { return damping; }

		inline void setAcceleration(const cVector3d &acc) { acceleration = acc; }
		inline void setAcceleration(double x, double y, double z) { setAcceleration(cVector3d(x, y, z)); }
		inline cVector3d getAcceleration() const { return acceleration; }

		inline void setSpeed(const cVector3d &sp) { speed = sp; }
		inline void setSpeed(double x, double y, double z) { setSpeed(cVector3d(x, y, z)); }
		inline cVector3d getSpeed() const { return speed; }
		double* getSpeed3();

	private:

		double mass;
		double charge;
		double damping;

		cVector3d acceleration;
		cVector3d speed;

	};

}

#endif

