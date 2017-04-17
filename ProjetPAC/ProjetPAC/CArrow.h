#ifndef CArrowh
#define CArrowh

#include "world/CMesh.h"
//#include "world/CMultiMesh.h"
#include "world/CGenericObject.h"
#include <vector>

namespace chai3d {

	class cArrow : public cMesh
	{

	public:

		cArrow(const cVector3d &pos = cVector3d(-0.35, 0.0, 0.0), double shaft_radius = 0.01, double tip_radius = 0.02, double len = 0.3, double tip_rel_len = 0.25);
		~cArrow();

		void updateArrow(const cVector3d &reference);

	private:

		double length;
		double tip_len;
		double shaft_len;

	};

}

#endif
