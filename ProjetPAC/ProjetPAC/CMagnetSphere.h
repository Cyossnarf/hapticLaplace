#ifndef CMagnetSphereh
#define CMagnetSphereh

#include "world/CShapeSphere.h"
#include <vector>

namespace chai3d {

	class cMagnetSphere : public cShapeSphere
	{

	public:

		cMagnetSphere(double a_radius, double a_mass = 9.11e-31, double a_charge = 1.602176565e-19, double a_damping = 0.999, int a_courseSize = 50);

		inline void setCharge(double value) { m_charge = value; }
		inline double getCharge() const { return m_charge; }

		inline void setMass(double value) { m_mass = value; }
		inline double getMass() const { return m_mass; }

		inline void setDamping(double value) { m_damping = value; }
		inline double getDamping() const { return m_damping; }

		inline void setAcceleration(const cVector3d &acc) { m_acceleration = acc; }
		inline void setAcceleration(double x, double y, double z) { setAcceleration(cVector3d(x, y, z)); }
		inline cVector3d getAcceleration() const { return m_acceleration; }

		inline void setSpeed(const cVector3d &sp) { m_speed = sp; }
		inline void setSpeed(double x, double y, double z) { setSpeed(cVector3d(x, y, z)); }
		inline cVector3d getSpeed() const { return m_speed; }
		
		inline int getCourseSize() const { return m_courseSize; }
		inline void addToCourse(cShapeSphere* miniSphere) { m_course.push_back(miniSphere); }
		void updateCourse();
		void resetCourse();

	private:

		double m_mass;
		double m_charge;
		double m_damping;

		cVector3d m_acceleration;
		cVector3d m_speed;

		std::vector<cShapeSphere*> m_course;
		int m_courseFront;
		int m_courseSize;

	};

}

#endif

