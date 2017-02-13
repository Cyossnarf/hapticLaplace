#ifndef CMobileCamh
#define CMobileCamh

#include "display/CCamera.h"

namespace chai3d {

	class cMobileCam : public cCamera
	{

	public:
		
		cMobileCam(cWorld* world, const cVector3d &eye = cVector3d(0.4, 0.0, 0.0), const cVector3d &target = cVector3d(0.0, 0.0, 0.0), const cVector3d &up = cVector3d(0.0, 0.0, 1.0));
		
		inline void setInMovement() { inMovement = true; }
		inline bool isInMovement() const { return inMovement; }
		
		void moveCam();

		inline cMatrix3d getRotation() const { return rotation; }
		inline int getState() const { return state; }

	private:

		cMatrix3d rotation;
		bool inMovement;
		int step;
		int state;

	};


}

#endif

