#include "CMobileCam.h"
#include "math/CVector3d.h"
#include "math/CMatrix3d.h"
#include <cmath>

#include "display/CCamera.h"
using namespace std;
//------------------------------------------------------------------------------
#include "world/CWorld.h"
#include "lighting/CSpotLight.h"
#include "lighting/CDirectionalLight.h"
//------------------------------------------------------------------------------
#ifdef C_USE_OPENGL
#ifdef MACOSX
#include "OpenGL/glu.h"
#else
#include "GL/glu.h"
#endif
#endif

chai3d::cMobileCam::cMobileCam(cWorld* world, const cVector3d &eye, const cVector3d &target, const cVector3d &up) : cCamera(world)

{
	this->set(eye, target, up);
	this->rotation.identity();
	this->inMovement = false;
	this->step = 0;
	this->state = 1;
}

void chai3d::cMobileCam::moveCam()

{
	if (this->step == 30 * (this->state + 1) / 2)//dans le sens aller on s'arrete pour step==30, dans le sens retour pour step==0
	{
		this->inMovement = false;
		this->state *= -1;
	}
	else
	{
		this->step += this->state;
		double alpha = this->step * C_PI / (30 * 4);
		// position and orient the camera
		this->set(cVector3d(0.4*cCosRad(alpha), 0.4*cSinRad(alpha), step * 0.15 / 30),	// camera position (eye)
			cVector3d(-step * 0.1 / 30, 0.0, 0.0),	// lookat position (target)
			cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

		// on calcule la matrice rotation pour orienter correctement les vecteurs dans la barre
		const double beta = cAngle(cVector3d(1.0, 0.0, 0.0), cVector3d(0.4*cCosRad(alpha) + step*0.1/30, 0.4*cSinRad(alpha), 0.0));
		this->rotation = cMatrix3d(0.0, 0.0, 1.0, -beta);
		//printf((cStr(beta)+"\n").c_str());//debug
		//printf((this->rotation.str()+"\n").c_str());//debug
	}
	
}

// On a reproduit la fonction renderView pour pouvoir afficher
// le monde 3D et le front layer dans des viewports differents
//==============================================================================
/*!
This method renders the scene viewed by the camera.

\param  a_VPWidth     Width of viewport.
\param  a_VPHeight    Height of viewport.
\param  a_displayContext  Current display context.
\param  a_eyeMode         When using stereo mode C_STEREO_PASSIVE_DUAL_DISPLAY,
specifies which eye view to render.
\param  a_defaultBuffer   If __true__ then the scene is rendered in the default
OpenGL buffer. If __false_ then the scene is rendered
in a framebuffer (cFrameBuffer) that will have been
previously setup.
*/
//==============================================================================
void chai3d::cMobileCam::renderView_custom(const int a_VPx,
	const int a_VPy,
	const int a_VPWidth,
	const int a_VPHeight,
	const int a_windowWidth,
	const int a_windowHeight,
	const int a_displayContext,
	const cEyeMode a_eyeMode,
	const bool a_defaultBuffer)
{
#ifdef C_USE_OPENGL

	//-----------------------------------------------------------------------
	// (1) SHADOW CASTING
	//-----------------------------------------------------------------------

	bool useShadowCasting = false;

	// check if shadow casting is supported and if shadow maps are available
	if (m_parentWorld != NULL)
	{
		if (m_parentWorld->getUseShadowCastring() && !m_parentWorld->m_shadowMaps.empty())
		{
			useShadowCasting = true;
		}
	}

	//-----------------------------------------------------------------------
	// (2) INITIALIZE CURRENT VIEWPORT
	//-----------------------------------------------------------------------

	// check window size
	if (a_VPHeight == 0) { return; }

	// store most recent size of display
	m_lastDisplayWidth = a_windowWidth;
	m_lastDisplayHeight = a_windowHeight;

	// compute aspect ratio
	double glAspect = ((double)a_VPWidth / (double)a_VPHeight);

	// compute global pose
	computeGlobalPositionsFromRoot(true);

	// set background color
	if (m_parentWorld != NULL)
	{
		glClearColor(m_parentWorld->getBackgroundColor().getR(),
			m_parentWorld->getBackgroundColor().getG(),
			m_parentWorld->getBackgroundColor().getB(),
			1.0);
	}
	else
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
	}


	//-----------------------------------------------------------------------
	// (3) VERIFY IF STEREO DISPLAY IS ENABLED
	//-----------------------------------------------------------------------
	GLboolean stereo = false;
	unsigned int numStereoPass = 1;
	cEyeMode eyeMode = C_STEREO_LEFT_EYE;

	if (m_stereoMode != C_STEREO_DISABLED)
	{
		/////////////////////////////////////////////////////////////////////
		// ACTIVE STEREO
		/////////////////////////////////////////////////////////////////////
		if (m_stereoMode == C_STEREO_ACTIVE)
		{
			// verify if stereo is available by the graphics hardware and camera is of perspective model
			glGetBooleanv(GL_STEREO, &stereo);

			if (stereo)
			{
				// stereo is available - we shall perform 2 rendering passes for LEFT and RIGHT eye.
				numStereoPass = 2;
			}
		}

		/////////////////////////////////////////////////////////////////////
		// PASSIVE STEREO (LEFT/RIGHT)
		/////////////////////////////////////////////////////////////////////
		else if (m_stereoMode == C_STEREO_PASSIVE_LEFT_RIGHT)
		{
			stereo = true;
			numStereoPass = 2;
		}

		/////////////////////////////////////////////////////////////////////
		// PASSIVE STEREO (TOP/BOTTOM)
		/////////////////////////////////////////////////////////////////////
		else if (m_stereoMode == C_STEREO_PASSIVE_TOP_BOTTOM)
		{
			stereo = true;
			numStereoPass = 2;
		}

		/////////////////////////////////////////////////////////////////////
		// PASSIVE STEREO (DUAL DISPLAY)
		/////////////////////////////////////////////////////////////////////
		else if (m_stereoMode == C_STEREO_PASSIVE_DUAL_DISPLAY)
		{
			stereo = true;
			numStereoPass = 1;
		}
	}


	//-----------------------------------------------------------------------
	// (4) RENDER THE ENTIRE SCENE
	//-----------------------------------------------------------------------
	for (unsigned int i = 0; i<numStereoPass; i++)
	{
		//-------------------------------------------------------------------
		// (4.1) SELECTING THE DISPLAY BUFFER (MONO / STEREO)
		//-------------------------------------------------------------------
		if ((stereo) && (m_stereoMode != C_STEREO_PASSIVE_DUAL_DISPLAY))
		{
			/////////////////////////////////////////////////////////////////
			// LEFT EYE
			/////////////////////////////////////////////////////////////////
			if (i == 0)
			{
				switch (m_stereoMode)
				{
				case C_STEREO_ACTIVE:
					glViewport(0, 0, a_VPWidth, a_VPHeight);
					glDrawBuffer(GL_BACK_LEFT);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					break;

				case C_STEREO_PASSIVE_LEFT_RIGHT:
					glViewport(0, 0, a_VPWidth / 2, a_VPHeight);
					if (a_defaultBuffer)
						glDrawBuffer(GL_BACK);
					else
						glDrawBuffer(GL_COLOR_ATTACHMENT0);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					break;

				case C_STEREO_PASSIVE_TOP_BOTTOM:
					glViewport(0, a_VPHeight / 2, a_VPWidth, a_VPHeight / 2);
					if (a_defaultBuffer)
						glDrawBuffer(GL_BACK);
					else
						glDrawBuffer(GL_COLOR_ATTACHMENT0);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					break;

				default: break;
				}

				// select left eye for rendering
				eyeMode = C_STEREO_LEFT_EYE;
			}

			/////////////////////////////////////////////////////////////////
			// RIGHT EYE
			/////////////////////////////////////////////////////////////////
			else
			{
				switch (m_stereoMode)
				{
				case C_STEREO_ACTIVE:
					glDrawBuffer(GL_BACK_RIGHT);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					break;

				case C_STEREO_PASSIVE_LEFT_RIGHT:
					glViewport(a_VPWidth / 2, 0, a_VPWidth / 2, a_VPHeight);
					break;

				case C_STEREO_PASSIVE_TOP_BOTTOM:
					glViewport(0, 0, a_VPWidth, a_VPHeight / 2);
					break;

				default: break;
				}

				// select right eye for rendering
				eyeMode = C_STEREO_RIGHT_EYE;
			}
		}
		else
		{
			glViewport(a_VPx, a_VPy, a_VPWidth, a_VPHeight);

			if (a_defaultBuffer)
				glDrawBuffer(GL_BACK);
			else
				glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// select eye for rendering
			eyeMode = a_eyeMode;
		}

		//-------------------------------------------------------------------
		// (4.2) SETUP GENERAL RENDERING SETTINGS
		//-------------------------------------------------------------------

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glShadeModel(GL_SMOOTH);

		//-------------------------------------------------------------------
		// (4.3)  RENDER BACK PLANE
		//-------------------------------------------------------------------

		// render the 2D backlayer
		// it will set up its own projection matrix
		glViewport(0, 0, a_windowWidth, a_windowHeight);//marqueur

		if (m_backLayer->getNumChildren())
		{
			renderLayer(m_backLayer,
				a_windowWidth,//marqueur
				a_windowHeight);
				//a_displayContext);//mq_maj
		}

		// clear depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);

		glViewport(a_VPx, a_VPy, a_VPWidth, a_VPHeight);//marqueur

		//-------------------------------------------------------------------
		// (4.4a) SETUP CAMERA  (MONO RENDERING)
		//-------------------------------------------------------------------
		if (!stereo)
		{
			// init projection matrix
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			// adjust display for mirroring
			glScalef((GLfloat)m_scaleH, (GLfloat)m_scaleV, (GLfloat)1.0);
			if (m_mirrorStatus)
			{
				glFrontFace(GL_CW);
			}
			else
			{
				glFrontFace(GL_CCW);
			}

			// create projection matrix depending of camera mode
			if (m_useCustomProjectionMatrix)
			{
				glLoadMatrixd(m_projectionMatrix.getData());
			}
			else
			{
				if (m_perspectiveMode)
				{
					// setup perspective camera
					gluPerspective(
						m_fieldViewAngleDeg, // Field of View angle.
						glAspect,            // Aspect ratio of viewing volume.
						m_distanceNear,      // Distance to Near clipping plane.
						m_distanceFar);      // Distance to Far clipping plane.
				}
				else
				{
					// setup orthographic camera
					double left = -m_orthographicWidth / 2.0;
					double right = -left;
					double bottom = left / glAspect;
					double top = -bottom;

					glOrtho(left,                // Left vertical clipping plane.
						right,               // Right vertical clipping plane.
						bottom,              // Bottom vertical clipping plane.
						top,                 // Top vertical clipping plane.
						m_distanceNear,      // Distance to Near clipping plane.
						m_distanceFar        // Distance to Far clipping plane.
					);
				}
			}
			// setup camera position
			glMatrixMode(GL_MODELVIEW);
			if (m_useCustomModelViewMatrix)
			{
				glLoadMatrixd(m_modelViewMatrix.getData());
			}
			else
			{
				glLoadIdentity();
			}

			// compute camera location
			cVector3d lookAt = m_globalRot.getCol0();
			cVector3d lookAtPos;
			m_globalPos.subr(lookAt, lookAtPos);
			cVector3d up = m_globalRot.getCol2();

			// setup modelview matrix
			gluLookAt(m_globalPos(0), m_globalPos(1), m_globalPos(2),
				lookAtPos(0), lookAtPos(1), lookAtPos(2),
				up(0), up(1), up(2));
		}


		//-------------------------------------------------------------------
		// (4.4b) SETUP CAMERA  (STEREO RENDERING)
		//-------------------------------------------------------------------
		else
		{
			// init projection matrix
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			// adjust display for mirroring
			glScalef((GLfloat)m_scaleH, (GLfloat)m_scaleV, (GLfloat)1.0);
			if (m_mirrorStatus)
			{
				glFrontFace(GL_CW);
			}
			else
			{
				glFrontFace(GL_CCW);
			}

			if (m_perspectiveMode)
			{
				double radians = 0.5 * cDegToRad(m_fieldViewAngleDeg);
				double wd2 = m_distanceNear * tan(radians);
				double ndfl = m_distanceNear / m_stereoFocalLength;

				// compute the look, up, and cross vectors
				cVector3d lookv = m_globalRot.getCol0();
				lookv.mul(-1.0);

				cVector3d upv = m_globalRot.getCol2();
				cVector3d offsetv = cCross(lookv, upv);

				offsetv.mul(m_stereoEyeSeparation / 2.0);

				// decide whether to offset left or right
				double stereo_multiplier;
				if (eyeMode == C_STEREO_LEFT_EYE)
				{
					// left eye
					stereo_multiplier = -1.0;
					offsetv.mul(-1.0);
				}
				else
				{
					// right eye
					stereo_multiplier = 1.0;
				}

				double left = -1.0 * glAspect * wd2 + stereo_multiplier * 0.5 * m_stereoEyeSeparation * ndfl;
				double right = glAspect * wd2 + stereo_multiplier * 0.5 * m_stereoEyeSeparation * ndfl;
				double top = wd2;
				double bottom = -1.0 * wd2;

				glFrustum(left, right, bottom, top, m_distanceNear, m_distanceFar);

				// initialize modelview matrix
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				// compute the offset we should apply to the current camera position
				cVector3d pos = cAdd(m_globalPos, offsetv);

				// compute the shifted camera position
				cVector3d lookAtPos;
				pos.addr(lookv, lookAtPos);

				// setup modelview matrix
				gluLookAt(pos(0), pos(1), pos(2),
					lookAtPos(0), lookAtPos(1), lookAtPos(2),
					upv(0), upv(1), upv(2));
			}
			else
			{
				// setup orthographic camera
				double left = -m_orthographicWidth / 2.0;
				double right = -left;
				double bottom = left / glAspect;
				double top = -bottom;

				glOrtho(left,            // Left vertical clipping plane.
					right,               // Right vertical clipping plane.
					bottom,              // Bottom vertical clipping plane.
					top,                 // Top vertical clipping plane.
					m_distanceNear,      // Distance to Near clipping plane.
					m_distanceFar        // Distance to Far clipping plane.
				);

				// setup camera position
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				// compute camera location
				cVector3d lookAt = m_globalRot.getCol0();
				cVector3d lookAtPos;
				m_globalPos.subr(lookAt, lookAtPos);
				cVector3d up = m_globalRot.getCol2();

				// setup modelview matrix
				gluLookAt(m_globalPos(0), m_globalPos(1), m_globalPos(2),
					lookAtPos(0), lookAtPos(1), lookAtPos(2),
					up(0), up(1), up(2));
			}
		}

		// Backup the view and projection matrix for future reference
		glGetDoublev(GL_PROJECTION_MATRIX, m_projectionMatrix.getData());
		glGetDoublev(GL_MODELVIEW_MATRIX, m_modelViewMatrix.getData());


		//-------------------------------------------------------------------
		// (4.5) RENDER THE 3D WORLD
		//-------------------------------------------------------------------

		// Set up reasonable default OpenGL state
		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// rendering options
		cRenderOptions options;

		if (m_parentWorld != NULL)
		{
			// optionally perform multiple rendering passes for transparency
			if (m_useMultipassTransparency)
			{
				////////////////////////////////////////////////////////////////////
				// MULTI PASS - USING SHADOW CASTING
				////////////////////////////////////////////////////////////////////
				if (useShadowCasting)
				{
					//--------------------------------------------------------------
					// OPAQUE OBJECTS
					//--------------------------------------------------------------

					// setup rendering options
					//options.m_displayContext = a_displayContext;//mq_maj
					options.m_camera = this;
					options.m_single_pass_only = false;
					options.m_render_opaque_objects_only = true;
					options.m_render_transparent_front_faces_only = false;
					options.m_render_transparent_back_faces_only = false;
					options.m_enable_lighting = true;
					options.m_render_materials = true;
					options.m_render_textures = true;
					options.m_creating_shadow_map = false;
					options.m_rendering_shadow = true;
					options.m_shadow_light_level = 1.0 - m_parentWorld->getShadowIntensity();
					options.m_storeObjectPositions = false;
					options.m_markForUpdate = m_markForUpdate;

					// render 1st pass (opaque objects - shadowed regions)
					m_parentWorld->renderSceneGraph(options);

					// setup rendering options
					options.m_rendering_shadow = false;
					options.m_shadow_light_level = 1.0;

					// render 2nd pass (opaque objects - non shadowed regions)
					list<cShadowMap*>::iterator lst;
					for (lst = m_parentWorld->m_shadowMaps.begin(); lst != m_parentWorld->m_shadowMaps.end(); ++lst)
					{
						cShadowMap *shadowMap = *lst;
						shadowMap->render(options);

						if (m_parentWorld != NULL)
						{
							m_parentWorld->renderSceneGraph(options);
						}

						// restore states
						glActiveTexture(shadowMap->m_depthBuffer->getTextureUnit());
						glDisable(GL_TEXTURE_2D);
						glDisable(GL_TEXTURE_GEN_S);
						glDisable(GL_TEXTURE_GEN_T);
						glDisable(GL_TEXTURE_GEN_R);
						glDisable(GL_TEXTURE_GEN_Q);
						glDisable(GL_ALPHA_TEST);
					}

					//--------------------------------------------------------------
					// TRANSPARENT OBJECTS
					//--------------------------------------------------------------

					// setup rendering options
					options.m_render_opaque_objects_only = false;
					options.m_render_transparent_back_faces_only = true;
					options.m_render_transparent_front_faces_only = false;
					options.m_rendering_shadow = false;

					// render 3rd pass (transparent objects - back faces only)
					m_parentWorld->renderSceneGraph(options);

					// modify rendering options for third pass
					options.m_render_opaque_objects_only = false;
					options.m_render_transparent_back_faces_only = false;
					options.m_render_transparent_front_faces_only = true;
					options.m_rendering_shadow = true;
					options.m_shadow_light_level = 1.0 - m_parentWorld->getShadowIntensity();

					// render 4th pass (transparent objects - front faces only - shadowed areas)
					m_parentWorld->renderSceneGraph(options);

					for (lst = m_parentWorld->m_shadowMaps.begin(); lst != m_parentWorld->m_shadowMaps.end(); ++lst)
					{
						cShadowMap *shadowMap = *lst;
						shadowMap->render(options);

						if (m_parentWorld != NULL)
						{
							m_parentWorld->renderSceneGraph(options);
						}

						// restore states
						glActiveTexture(shadowMap->m_depthBuffer->getTextureUnit());
						glDisable(GL_TEXTURE_2D);
						glDisable(GL_TEXTURE_GEN_S);
						glDisable(GL_TEXTURE_GEN_T);
						glDisable(GL_TEXTURE_GEN_R);
						glDisable(GL_TEXTURE_GEN_Q);
						glDisable(GL_ALPHA_TEST);
					}

					// modify rendering options for 5th pass
					options.m_rendering_shadow = false;
					options.m_shadow_light_level = 1.0;

					// render 5th pass (transparent objects - front faces only - lighted regions)
					for (lst = m_parentWorld->m_shadowMaps.begin(); lst != m_parentWorld->m_shadowMaps.end(); ++lst)
					{
						cShadowMap *shadowMap = *lst;
						shadowMap->render(options);

						if (m_parentWorld != NULL)
						{
							m_parentWorld->renderSceneGraph(options);
						}

						// restore states
						glActiveTexture(shadowMap->m_depthBuffer->getTextureUnit());
						glDisable(GL_TEXTURE_2D);
						glDisable(GL_TEXTURE_GEN_S);
						glDisable(GL_TEXTURE_GEN_T);
						glDisable(GL_TEXTURE_GEN_R);
						glDisable(GL_TEXTURE_GEN_Q);
						glDisable(GL_ALPHA_TEST);
					}
				}


				////////////////////////////////////////////////////////////////////
				// MULTI PASS - WITHOUT SHADOWS
				////////////////////////////////////////////////////////////////////
				else
				{
					// setup rendering options for first pass
					//options.m_displayContext = a_displayContext;//mq_maj
					options.m_camera = this;
					options.m_single_pass_only = false;
					options.m_render_opaque_objects_only = true;
					options.m_render_transparent_front_faces_only = false;
					options.m_render_transparent_back_faces_only = false;
					options.m_enable_lighting = true;
					options.m_render_materials = true;
					options.m_render_textures = true;
					options.m_creating_shadow_map = false;
					options.m_rendering_shadow = false;
					options.m_shadow_light_level = 1.0;
					options.m_storeObjectPositions = true;
					options.m_markForUpdate = m_markForUpdate;

					// render 1st pass (opaque objects - all faces)
					if (m_parentWorld != NULL)
					{
						m_parentWorld->renderSceneGraph(options);
					}

					// modify rendering options
					options.m_render_opaque_objects_only = false;
					options.m_render_transparent_back_faces_only = true;
					options.m_storeObjectPositions = false;

					// render 2nd pass (transparent objects - back faces only)
					if (m_parentWorld != NULL)
					{
						m_parentWorld->renderSceneGraph(options);
					}

					// modify rendering options
					options.m_render_transparent_back_faces_only = false;
					options.m_render_transparent_front_faces_only = true;

					// render 3rd pass (transparent objects - front faces only)
					if (m_parentWorld != NULL)
					{
						m_parentWorld->renderSceneGraph(options);
					}
				}
			}
			else
			{
				////////////////////////////////////////////////////////////////////
				// SINGLE PASS - USING SHADOW CASTING 
				////////////////////////////////////////////////////////////////////
				if (useShadowCasting)
				{
					// setup rendering options for single pass
					//options.m_displayContext = a_displayContext;//mq_maj
					options.m_camera = this;
					options.m_single_pass_only = true;
					options.m_render_opaque_objects_only = true;
					options.m_render_transparent_front_faces_only = false;
					options.m_render_transparent_back_faces_only = false;
					options.m_enable_lighting = true;
					options.m_render_materials = true;
					options.m_render_textures = true;
					options.m_creating_shadow_map = false;
					options.m_rendering_shadow = true;
					options.m_shadow_light_level = 1.0 - m_parentWorld->getShadowIntensity();
					options.m_storeObjectPositions = false;
					options.m_markForUpdate = m_markForUpdate;

					// render 1st pass (opaque objects - all faces - shadowed regions)
					m_parentWorld->renderSceneGraph(options);

					// setup rendering options
					options.m_rendering_shadow = false;
					options.m_shadow_light_level = 1.0;

					// render 2nd pass (opaque objects - all faces - lighted regions)
					list<cShadowMap*>::iterator lst;
					for (lst = m_parentWorld->m_shadowMaps.begin(); lst != m_parentWorld->m_shadowMaps.end(); ++lst)
					{
						cShadowMap *shadowMap = *lst;
						shadowMap->render(options);

						if (m_parentWorld != NULL)
						{
							m_parentWorld->renderSceneGraph(options);
						}

						// restore states
						glActiveTexture(shadowMap->m_depthBuffer->getTextureUnit());
						glDisable(GL_TEXTURE_2D);
						glDisable(GL_TEXTURE_GEN_S);
						glDisable(GL_TEXTURE_GEN_T);
						glDisable(GL_TEXTURE_GEN_R);
						glDisable(GL_TEXTURE_GEN_Q);
						glDisable(GL_ALPHA_TEST);
					}

					// setup rendering options
					options.m_render_opaque_objects_only = false;
					options.m_render_transparent_front_faces_only = true;
					options.m_render_transparent_back_faces_only = true;

					// render 3rd pass (transparent objects - all faces)
					if (m_parentWorld != NULL)
					{
						m_parentWorld->renderSceneGraph(options);
					}
				}


				////////////////////////////////////////////////////////////////////
				// SINGLE PASS - WITHOUT SHADOWS
				////////////////////////////////////////////////////////////////////
				else
				{
					// setup rendering options for single pass
					//options.m_displayContext = a_displayContext;//mq_maj
					options.m_camera = this;
					options.m_single_pass_only = true;
					options.m_render_opaque_objects_only = false;
					options.m_render_transparent_front_faces_only = false;
					options.m_render_transparent_back_faces_only = false;
					options.m_enable_lighting = true;
					options.m_render_materials = true;
					options.m_render_textures = true;
					options.m_creating_shadow_map = false;
					options.m_rendering_shadow = false;
					options.m_shadow_light_level = 1.0;
					options.m_storeObjectPositions = true;
					options.m_markForUpdate = m_markForUpdate;

					// render single pass (all objects)
					if (m_parentWorld != NULL)
					{
						m_parentWorld->renderSceneGraph(options);
					}
				}
			}
		}

		//-------------------------------------------------------------------
		// (4.6) RENDER FRONT PLANE
		//-------------------------------------------------------------------

		// clear depth buffer
		glClear(GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, a_windowWidth, a_windowHeight);//marqueur

		// render the 'front' 2d object layer; it will set up its own
		// projection matrix
		if (m_frontLayer->getNumChildren() > 0)
		{
			renderLayer(m_frontLayer,
				a_windowWidth,//marqueur
				a_windowHeight);
				//a_displayContext);//mq_maj
		}

		// if requested, display reset has now been completed
		m_markForUpdate = false;
	}

	//-----------------------------------------------------------------------
	// (5) AUDIO DEVICE
	//-----------------------------------------------------------------------

	// update position of optionally attached audio device
	if (m_audioDevice != NULL)
	{
		m_audioDevice->setListenerPos(m_globalPos);
		m_audioDevice->setListenerRot(m_globalRot);
	}

#endif
}
