#ifndef CMobileCamh
#define CMobileCamh

#include "display/CCamera.h"

namespace chai3d {

	class cMobileCam : public cCamera
	{

	public:
		
		cMobileCam(cWorld* world, const cVector3d &eye = cVector3d(0.4, 0.0, 0.0), const cVector3d &target = cVector3d(0.0, 0.0, 0.0), const cVector3d &up = cVector3d(0.0, 0.0, 1.0));
		
		void setInMovement(const int a_mvtType = 1);
		inline int isInMovement() const { return inMovement; }
		
		void moveCam();

		//! This method renders the the camera view in OpenGL
		virtual void renderView_custom(const int a_VPx,
			const int a_VPy,
			const int a_VPWidth,
			const int a_VPHeight,
			const int a_windowWidth,
			const int a_windowHeight,
			const int a_displayContext = 0,
			const cEyeMode a_eyeMode = C_STEREO_LEFT_EYE,
			const bool a_defaultBuffer = true);

		inline cMatrix3d getRotation() const { return rotation; }
		inline int getSense() const { return sense; }
		inline int getSense2() const { return sense2; }
		inline int getStep2() const { return step2; }

	private:

		cMatrix3d rotation;
		// describes the camera's movement: 0 -> immobile, 1 -> turning around, 2 -> changing scale
		int inMovement;
		// step in the turning around motion
		int step;
		// sense of the turning around motion: 1 -> 2d to 3d, -1 -> 3d to 2d
		int sense;
		// step in the changing scale motion
		int step2;
		// sense of the changing scale motion: 1 -> close to far, -1 -> far to close
		int sense2;
		// end positions of the changing scale motion
		cVector3d pos1;
		cVector3d pos2;
		// end lookat positions of the changing scale motion
		cVector3d lookatPos1;
		cVector3d lookatPos2;
		// initial positions of the camera
		cVector3d pos0;
		cVector3d lookatPos0;
	};


}

#endif

