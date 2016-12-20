#ifndef CMobileCamh
#define CMobileCamh

#include "display/CCamera.h"

namespace chai3d {

	class cMobileCam : public cCamera
	{

	public:
		
		cMobileCam(cWorld* world, const cVector3d &eye = cVector3d(0.4, 0.0, 0.0), const cVector3d &target = cVector3d(0.0, 0.0, 0.0), const cVector3d &up = cVector3d(0.0, 0.0, 1.0));
		void moveCam();

	private:

		cVector3d eye;
		cVector3d target;
		cVector3d up;
		bool inMovement;

	};

	//void moveCam(const cVector3d &eye, const cVector3d &target, const cVector3d &up);

}

#endif

