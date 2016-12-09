#ifndef CMagnetFieldh
#define CMagnetFieldh

#include "world/CMesh.h"
#include "world/CGenericObject.h"
#include <vector>

namespace chai3d {

	class cMagnetField : public cMesh
	{

	public :
		
		cMagnetField(const cVector3d &pos = cVector3d(0.0, 0.0, 0.0), double in_radius = 0.12, double out_radius = 0.13, double intensity = 0.25, double height = 1);
		~cMagnetField();
		
		void rotateAround(cVector3d &rotationAxis, double rotation);

		bool contain(const cGenericObject &obj) const;

		cVector3d magnetForce(const cGenericObject &obj, double charge, const cVector3d &vel, bool enabled = true);

		inline cVector3d getDirection() const { return direction; }
		inline cVector3d getRadial() const { return radial; }

		inline double getInnerRadius() const { return innerRadius; }
		inline double getOuterRadius() const { return outerRadius; }

		inline double getCurrentIntensity() const { return intensity; }
		void setCurrentIntensity(const double value);

		inline double getTransparency() const { return transparency; }
		void setTransparency(double value);
		
		void activate(bool active) { fieldEnabled = active; }

	private :
		
		double innerRadius;
		double outerRadius;
		cVector3d direction;
		cVector3d radial;
		double intensity;
		bool fieldEnabled;
		std::vector<cMesh*> turns;
		double transparency;

	};

}

#endif
