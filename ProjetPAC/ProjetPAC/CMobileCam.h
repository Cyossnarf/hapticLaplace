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
		//! This method renders the the camera view in OpenGL
		virtual void renderView_custom(const int a_windowWidth,
			const int a_windowHeight,
			const int a_worldVPWidth,
			const int a_worldVPHeight,
			const int a_displayContext = 0,
			const cEyeMode a_eyeMode = C_STEREO_LEFT_EYE,
			const bool a_defaultBuffer = true);

		inline cMatrix3d getRotation() const { return rotation; }
		inline int getState() const { return state; }

	protected:

		//! Renders a 2D layer within this camera's view.
		void renderLayer_custom(cGenericObject* a_graph, int a_width, int a_height, int a_displayContext);

	private:

		cMatrix3d rotation;
		bool inMovement;
		int step;
		int state;

	};


}

#endif

