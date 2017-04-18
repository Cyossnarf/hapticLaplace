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

		cArrow(const cVector3d &pos, cColorb &col, double shaft_radius, double tip_radius, double len, double tip_rel_len = 0.25);
		~cArrow();

		void updateArrow(const cVector3d &reference);

	private:

		double length;
		double tip_len;
		double shaft_len;

	};

}

#endif
