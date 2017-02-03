//==============================================================================
/*
Software License Agreement (BSD License)
Copyright (c) 2003-2015, CHAI3D.
(www.chai3d.org)

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

* Neither the name of CHAI3D nor the names of its contributors may
be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

\author    <http://www.chai3d.org>
\author    Francois Conti
\version   3.0.0 $Rev: 1795 $
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
#include "CMagnetField.h"
#include "CMagnetSphere.h"
//#include "CCircle.h"
#include "CMobileCam.h"
#include "AntTweakBar.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
#ifndef MACOSX
#include "GL/glut.h"
#else
#include "GLUT/glut.h"
#endif
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// MACROS DEFINITION
//------------------------------------------------------------------------------

#define FRAND(a, b) ((a) + ((b)-(a))*((double)(rand())/RAND_MAX))

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
C_STEREO_DISABLED:            Stereo is disabled
C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

bool hapticDeviceConnected = false;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;

// top camera enabled
bool showTopView = true;

// count the number of position reset
int resetCount = 0;

//int loop = 0;
int counterForceFactor = 0;

// variable selected for modification
string selection = "";


//------------------------------------------------------------------------------
// DECLARED CONSTANTS
//------------------------------------------------------------------------------

// radius of the sphere
const double SPHERE_RADIUS = 0.005;

// mass of the sphere (can be modified during the simulation)
double SPHERE_MASS = 0.04;

// charge of the sphere (can be modified during the simulation)
double SPHERE_CHARGE = 1.0;

// speed vector of the sphere (can be modified during the simulation)
float SPHERE_SPEED[3] = { 0.1f, 0.2f, 0.0f };//double *SPHERE_SPEED;

// damping of the sphere
const double SPHERE_DAMPING = 0.999;

// stiffness of the haptic device
const double HAPTIC_STIFFNESS = 1000.0;


//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display, and mobile btw...
cMobileCam *camera;

// a light source to illuminate the objects in the world
cSpotLight *light;
cSpotLight *light2;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;

// highest stiffness the current haptic device can render
double hapticDeviceMaxStiffness;

// the sphere the user controls
cMagnetSphere *sphere;

// magnetic field
cMagnetField *magnetField;

// circle to be drawn around the magnet field
//cCircle *circle;

// a bar to display info
TwBar *bar1;

// a label to explain what is happening
cLabel* labelMessage;

// a label to display the values of the variables of the simulation
cLabel* labelInfo;

// a label to display "game over"
cLabel* labelGameOver;

// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = true;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

// information about computer screen and GLUT display window
int screenW;
int screenH;
int windowW;
int windowH;
int windowPosX;
int windowPosY;

//information about the ATB bar
int barSizeW;
int barSizeH;
int barPosX;
int barPosY;
string barPos;
string barSize;
string barDef;

// root resource path
string resourceRoot;

// mouse interactions parameters
int mousePosx(0);
int mousePosy(0);
int userClick(0);
double scaleFactor(5e-5);

// user movement intentions
double userMovex(0.0);
double userMovey(0.0);
double userMovez(0.0);



//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when the mouse is moved while a button is pressed on the window
void mouseDragActiveCallback(int x, int y);

// callback when the mouse is moved passively on the window
void mouseDragPassiveCallback(int x, int y);

// callback when a mouse button is pressed or released on the window
void mouseClickCallback(int button, int state, int x, int y);

// callback when a key is pressed
void keySelect(unsigned char key, int x, int y);

// callback to render graphic scene
void updateGraphics(void);

// callback of GLUT timer
void graphicsTimer(int data);

// function that closes the application
void close(void);

// main haptics simulation loop
void updateHaptics(void);

// function that writes some data to be print into a text file
void logData(vector<double> data, double interval, string filename);


//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------
// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//==============================================================================
/*
DEMO:   09-magnets.cpp

This example illustrates how to create a simple dynamic simulation using
small sphere shape primitives. All dynamics and collisions are computed
in the haptics thread.
*/
//==============================================================================

int main(int argc, char* argv[])
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Demo: 09-magnets" << endl;
	cout << "Copyright 2003-2015" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[1] - select field opacity" << endl;
	cout << "[2] - select sphere mass" << endl;
	cout << "[3] - select sphere charge" << endl;
	cout << "[+/-] - increase/decrease selected variable" << endl;
	cout << "[f] - Enable/Disable full screen mode" << endl;
	cout << "[m] - Enable/Disable vertical mirroring" << endl;
	cout << "[r] - reset position of the sphere" << endl;
	cout << "[ESC] - Exit application" << endl;
	cout << endl << endl;


	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
	//--------------------------------------------------------------------------

	// initialize GLUT
	glutInit(&argc, argv);

	// initialize random numbers generation
	srand(0);

	// retrieve  resolution of computer display and position window accordingly
	screenW = glutGet(GLUT_SCREEN_WIDTH);
	screenH = glutGet(GLUT_SCREEN_HEIGHT);
	windowW = 0.8 * screenH;
	windowH = 0.5 * screenH;
	windowPosY = (screenH - windowH) / 2;
	windowPosX = windowPosY;

	// initialize the OpenGL GLUT window
	glutInitWindowPosition(windowPosX, windowPosY);
	glutInitWindowSize(windowW, windowH);

	if (stereoMode == C_STEREO_ACTIVE)
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STEREO);
	else
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// create display context and initialize GLEW library
	glutCreateWindow(argv[0]);

#ifdef GLEW_VERSION
	// initialize GLEW
	glewInit();
#endif

	// setup GLUT options
	glutDisplayFunc(updateGraphics);
	glutKeyboardFunc(keySelect);
	glutReshapeFunc(resizeWindow);
	glutSetWindowTitle("Mouvement d'une charge dans un champ magnétique");

	// set fullscreen mode
	if (fullscreen)
	{
		glutFullScreen();
	}

	// initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);

	// telling AntTweakBar the size of the graphic window
	TwWindowSize(windowW, windowH);

	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// create a new world.
	world = new cWorld();

	// set the background color of the environment
	world->m_backgroundColor.setWhite();

	// create a camera and insert it into the virtual world
	camera = new cMobileCam(world);
	world->addChild(camera);
	
										// set the near and far clipping planes of the camera
										// anything in front/behind these clipping planes will not be rendered
	camera->setClippingPlanes(0.01, 10.0);

	// set stereo mode
	camera->setStereoMode(stereoMode);

	// set stereo eye separation and focal length (applies only if stereo is enabled)
	camera->setStereoEyeSeparation(0.03);
	camera->setStereoFocalLength(1.8);

	// set vertical mirrored display mode
	camera->setMirrorVertical(mirroredDisplay);

	// create a light source
	light = new cSpotLight(world);
	light2 = new cSpotLight(world);

	// attach light to camera
	world->addChild(light);
	world->addChild(light2);

	// enable light source
	light->setEnabled(true);
	light2->setEnabled(true);

	// position the light source
	light->setLocalPos(0.4, -0.4, 0.3);//(0.0, 0.3, 0.4);
	light2->setLocalPos(-0.2, -0.1, 1.0);//(0.0, 0.3, -0.4);

	// define the direction of the light beam
	light->setDir(-0.5, 0.3, -0.2);//(0.0, -0.25, -0.4);
	light2->setDir(0.0, 0.1, -0.5);//(0.0, -0.25, 0.4);

	// enable this light source to generate shadows
	light->setShadowMapEnabled(true);
	light2->setShadowMapEnabled(true);

	// set the resolution of the shadow map
	//light->m_shadowMap->setQualityLow();
	light->m_shadowMap->setQualityMedium();
	light2->m_shadowMap->setQualityMedium();

	// set shadow factor
	world->setShadowIntensity(0.3);

	// set light cone half angle
	light->setCutOffAngleDeg(40);//(30);
	light2->setCutOffAngleDeg(40);


	//--------------------------------------------------------------------------
	// HAPTIC DEVICES / TOOLS
	//--------------------------------------------------------------------------

	// create a haptic device handler
	handler = new cHapticDeviceHandler();

	// get access to the first available haptic device
	hapticDeviceConnected = handler->getDevice(hapticDevice, 0);

	// retrieve information about the current haptic device
	cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();

	// retrieve the highest stiffness this device can render
	hapticDeviceMaxStiffness = hapticDeviceInfo.m_maxLinearStiffness;

	// if the haptic devices carries a gripper, enable it to behave like a user switch
	hapticDevice->setEnableGripperUserSwitch(true);


	//--------------------------------------------------------------------------
	// CREATE SPHERE AND FIELD
	//--------------------------------------------------------------------------

	// create the sphere
	sphere = new cMagnetSphere(SPHERE_RADIUS, SPHERE_MASS, SPHERE_CHARGE, SPHERE_DAMPING);

	// add the sphere to the world
	world->addChild(sphere);

	// set position of the sphere
	sphere->setLocalPos(0.00, -0.23, 0.0);//(-0.05, -0.18, 0.0);

	// set color of the sphere
	sphere->m_material->setBlueDark();
	
	// create a magnetic field
	magnetField = new cMagnetField();

	// add the magnetic field to the world
	world->addChild(magnetField);

	// set position of the magnetic field
	magnetField->setLocalPos(-magnetField->getLength(), 0.0, 0.0);//(0.0, 0.05, 0.01);

	//--------------------------------------------------------------------------
	// BARS
	//--------------------------------------------------------------------------

	// create a bar
	bar1 = TwNewBar("Bonjour");

	// define parameters of the bar
	TwDefine(" Bonjour refresh=0.1 ");

	// add variables
	TwAddVarRW(bar1, "Charge", TW_TYPE_DOUBLE, &SPHERE_CHARGE, "");
	TwAddVarRW(bar1, "Vitesse", TW_TYPE_DIR3F, &SPHERE_SPEED, " opened=true ");
	
	//--------------------------------------------------------------------------
	// WIDGETS
	//--------------------------------------------------------------------------

	// create a font
	cFont *font = NEW_CFONTCALIBRI20();

	// create a label with a small message
	labelMessage = new cLabel(font);
	camera->m_frontLayer->addChild(labelMessage);

	// set font color
	labelMessage->m_fontColor.setBlack();

	// set text message
	labelMessage->setText("move a charge into a magnetic field");

	// create a label to display variable values of the simulation
	labelInfo = new cLabel(font);
	labelInfo->m_fontColor.setRed();
	camera->m_frontLayer->addChild(labelInfo);

	// create a label to display "game over"
	labelGameOver = new cLabel(font);
	labelGameOver->m_fontColor.setRed();
	camera->m_frontLayer->addChild(labelGameOver);

	/*
	// create a background
	cBackground* background = new cBackground();
	camera->m_backLayer->addChild(background);

	// set aspect ration of background image a constant
	background->setFixedAspectRatio(true);

	// load background image
	bool fileload = background->loadFromFile(RESOURCE_PATH("../resources/images/background.png"));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = background->loadFromFile("../../../bin/resources/images/background.png");
#endif
	}
	if (!fileload)
	{
		cout << "Error - Image failed to load correctly." << endl;
		close();
		return (-1);
	}
	*/

	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------

	// create a thread which starts the main haptics rendering loop
	cThread* hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup mouse callbacks
	glutPassiveMotionFunc(mouseDragPassiveCallback);
	glutMotionFunc(mouseDragActiveCallback);
	glutMouseFunc(mouseClickCallback);

	// setup callback when application exits
	atexit(close);

	// start the main graphics rendering loop
	glutTimerFunc(50, graphicsTimer, 0);
	glutMainLoop();

	// exit
	return (0);
}

//------------------------------------------------------------------------------

void mouseDragActiveCallback(int x, int y)
{

	if (userClick < 0)
	{
		userMovez = -(y - mousePosy) * scaleFactor;
		userMovey = (x - mousePosx) * scaleFactor;
	}
	else if (userClick > 0)
	{
		userMovex = (y - mousePosy) * scaleFactor;
		userMovey = (x - mousePosx) * scaleFactor;
	}

	mousePosx = x;
	mousePosy = y;
	/*
	userMovex = cMin(userMovex, 0.5);
	userMovey = cMin(userMovey, 0.5);
	userMovez = cMin(userMovez, 0.5);*/
}

//------------------------------------------------------------------------------

void mouseDragPassiveCallback(int x, int y)
{

	mousePosx = x;
	mousePosy = y;

}

//------------------------------------------------------------------------------

void mouseClickCallback(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && userClick == 0 && state == GLUT_DOWN)
		userClick = -1;

	if (button == GLUT_RIGHT_BUTTON && userClick == 0 && state == GLUT_DOWN)
		userClick = 1;

	if (button == GLUT_LEFT_BUTTON && userClick == -1 && state == GLUT_UP)
	{
		userClick = 0;
		userMovex = 0.0;
		userMovey = 0.0;
		userMovez = 0.0;
	}

	if (button == GLUT_RIGHT_BUTTON && userClick == 1 && state == GLUT_UP)
	{
		userClick = 0;
		userMovex = 0.0;
		userMovey = 0.0;
		userMovez = 0.0;
	}
}

//------------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
	windowW = w;
	windowH = h;
}

//------------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
	// option ESC: exit
	if (key == 27)
	{
		// exit application
		exit(0);
	}

	// option f: toggle fullscreen
	if (key == 'f')
	{
		if (fullscreen)
		{
			windowPosX = glutGet(GLUT_INIT_WINDOW_X);
			windowPosY = glutGet(GLUT_INIT_WINDOW_Y);
			windowW = glutGet(GLUT_INIT_WINDOW_WIDTH);
			windowH = glutGet(GLUT_INIT_WINDOW_HEIGHT);
			glutPositionWindow(windowPosX, windowPosY);
			glutReshapeWindow(windowW, windowH);
			fullscreen = false;
		}
		else
		{
			glutFullScreen();
			fullscreen = true;
		}
	}

	// option m: toggle vertical mirroring
	if (key == 'm')
	{
		mirroredDisplay = !mirroredDisplay;
		camera->setMirrorVertical(mirroredDisplay);
	}

	// option r: reset position of the sphere
	if (key == 'r')
	{
		/*
		if(++resetCount % 10 == 0)
			sphere->setLocalPos(0.0, 0.0, 0.0);
		else
		*/
		sphere->setLocalPos(0.00, -0.23, FRAND(-0.05, 0.2));//(-0.05, -0.18, FRAND(-0.05, 0.2));
		sphere->setSpeed(0.0, 0.0, 0.0);
		sphere->setAcceleration(0.0, 0.0, 0.0);
	}

	// option c: start camera movement
	if (key == 'c')
	{
		camera->setInMovement();
	}

	// option 1: select the transparency level of the field
	if (key == '1')
	{
		selection = "transparency";
	}

	// option 2: select the charge of the sphere
	if (key == '2')
	{
		selection = "mass";
	}

	// option 3: select the mass of the sphere
	if (key == '3')
	{
		selection = "charge";
	}

	// option 4: select the current intensity of the turns
	if (key == '4')
	{
		selection = "intensity";
	}

	// option +: increase the selected variable
	if (key == '+')
	{
		if (selection.compare("transparency") == 0)
		{
			magnetField->setTransparency(magnetField->getTransparency() + 0.05);
		}

		if (selection.compare("charge") == 0)
		{
			sphere->setCharge(sphere->getCharge() + 0.5);
			SPHERE_CHARGE += 0.5;
		}

		if (selection.compare("mass") == 0)
		{
			sphere->setMass(sphere->getMass() + 0.01);
			SPHERE_MASS += 0.01;
		}

		if (selection.compare("intensity") == 0)
		{
			magnetField->setCurrentIntensity(magnetField->getCurrentIntensity() + 0.1);
		}
	}

	// option -: decrease the selected variable
	if (key == '-')
	{
		if (selection.compare("transparency") == 0)
		{
			magnetField->setTransparency(magnetField->getTransparency() - 0.05);
		}

		if (selection.compare("charge") == 0)
		{
			sphere->setCharge(sphere->getCharge() - 0.5);
			SPHERE_CHARGE -= 0.5;
		}

		if (selection.compare("mass") == 0)
		{
			sphere->setMass(sphere->getMass() - 0.01);
			SPHERE_MASS -= 0.01;
		}

		if (selection.compare("intensity") == 0)
		{
			magnetField->setCurrentIntensity(magnetField->getCurrentIntensity() - 0.1);
		}
	}
}

//------------------------------------------------------------------------------

void close(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }

	//uninitialize AntTweakBar
	TwTerminate();
}

//------------------------------------------------------------------------------

void graphicsTimer(int data)
{
	if (simulationRunning)
	{
		glutPostRedisplay();
	}

	glutTimerFunc(50, graphicsTimer, 0);
}

//------------------------------------------------------------------------------

void updateGraphics(void)
{
	if (camera->isInMovement())
	{
		camera->moveCam();
	}

	/////////////////////////////////////////////////////////////////////
	// UPDATE WIDGETS
	/////////////////////////////////////////////////////////////////////

	// update variable values
	labelInfo->setText("field opacity : " + cStr(magnetField->getTransparency() * 100) + "% | sphere mass : " + cStr(sphere->getMass()) + " kg | sphere charge : " + cStr(sphere->getCharge()) + " C | current intensity : " + cStr(magnetField->getCurrentIntensity()) + " A");

	// update position of info label
	labelInfo->setLocalPos((int)(0.5 * (windowW - labelInfo->getWidth())), 15);

	// update position of message label
	labelMessage->setLocalPos((int)(0.5 * (windowW - labelMessage->getWidth())), 50);

	// update position of "game over" label
	labelGameOver->setLocalPos((int)(0.5 * (windowW - labelGameOver->getWidth())), 30);
	if (SPHERE_CHARGE >= 2.0) {
		labelGameOver->setText("GAME OVER");
	}
	else {
		labelGameOver->setText(" ");
	}

	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE
	/////////////////////////////////////////////////////////////////////

	// update shadow maps (if any)
	world->updateShadowMaps(false, mirroredDisplay);
	
	// render world
	camera->renderView(windowW*0.7, windowH*0.8);

	// telling AntTweakBar the size of the graphic window
	TwWindowSize(windowW, windowH);

	// display bars
	barPosX = windowW*0.7;
	barPosY = windowH*0.1;
	barSizeW = windowW*0.2;
	barSizeH = windowH*0.6;
	barPos = to_string(barPosX)+" "+ to_string(barPosY);
	barSize = to_string(barSizeW) + " " + to_string(barSizeH);
	barDef = " Bonjour position='" + barPos + "' size='" + barSize+"'";
	const char *pbarDef = barDef.c_str();
	TwDefine(pbarDef);
	TwDraw();

	// swap buffers
	glutSwapBuffers();

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{
	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// flag to indicate if haptic forces are active
	bool flagHapticsEnabled = false;

	// reset clock
	cPrecisionClock clock;
	clock.reset();

	// open a connection to haptic device
	hapticDevice->open();

	// calibrate device (if necessary)
	hapticDevice->calibrate();

	// store the current position of the haptic device
	cVector3d devicePos(0.0, 0.0, 0.0);

	// store the force the user applies to the haptic device
	//cVector3d userForce(0.0, 0.0, 0.0);

	// store the desired position of the haptic device
	cVector3d deviceTargetPos(0.0, 0.0, 0.0);

	// store the force to apply to the device
	cVector3d forceToApply(0.0, 0.0, 0.0);

	// store desired data to write into a text file
	vector<double> data;

	// gain used to control the position of the haptic device
	int gain = 1;

	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////
		// SIMULATION TIME
		/////////////////////////////////////////////////////////////////////

		// read device sensed force
		cVector3d sensedForce;

		// rotate the turns according to the current intensity
		//magnetField->rotateAround(magnetField->getDirection(), (magnetField->getCurrentIntensity() > 0) ? 0.001 : ((magnetField->getCurrentIntensity() < 0) ? -0.001 : 0.0));

		// stop the simulation clock
		clock.stop();

		// read the time increment in seconds
		double timeInterval = cMin(0.001, clock.getCurrentTimeSeconds());

		// restart the simulation clock
		clock.reset();
		clock.start();

		// update frequency counter
		frequencyCounter.signal(1);


		/////////////////////////////////////////////////////////////////////////
		// READ HAPTIC DEVICE
		/////////////////////////////////////////////////////////////////////////

		// read device position
		hapticDevice->getPosition(devicePos);
		devicePos.x(0.0);

		// read sphere position
		cVector3d position(sphere->getLocalPos());

		// get sphere velocity from user
		cVector3d userVel(userMovex, userMovey, userMovez);
		/*
		if (resetCount == 0 || resetCount % 10 != 0)
			userVel = userVel - userVel.dot(magnetField->getDirection()) * magnetField->getDirection();
		else
			userVel = userVel.dot(magnetField->getDirection()) * magnetField->getDirection();
		*/
		userVel = userVel - userVel.dot(magnetField->getDirection()) * magnetField->getDirection();

		// limit the velocity maximal that the user can give to the sphere
		if (userVel.length() > 0.0008)
		{
			userVel.normalize();
			userVel *= 0.0008;
		}


		/////////////////////////////////////////////////////////////////////////
		// UPDATE SIMULATION
		/////////////////////////////////////////////////////////////////////////

		// clear forces for the sphere
		cVector3d sphereFce;
		sphereFce.zero();

		data.push_back(counterForceFactor);

		if (magnetField->contain(*sphere))
		{
			// compute magnetic forces
			sphereFce.add(magnetField->magnetForce(*sphere, sphere->getCharge(), sphere->getSpeed()));
			/*
			// update the trust to give to the user counter force 
			if (hapticDeviceConnected && (devicePos - deviceTargetPos).length() > 0.015)
			{
				counterForceFactor = cMin(counterForceFactor + 1, 100);
			}

			else
			{
				counterForceFactor = cMax(counterForceFactor - 20, 0);
			}

			// compute the user counter force
			userForce = -(counterForceFactor / 100) * sphereFce;
			*/
		}
		/*
		else
		{
			userForce.zero();
			counterForceFactor = 0;
		}
		*/
		/*else
		{
		loop = 0;
		}*/

		// compute acceleration
		sphere->setAcceleration(sphereFce / sphere->getMass());//((sphereFce + userForce) / sphere->getMass());

		// compute velocity
		cVector3d newVel(userVel + sphere->getSpeed() + timeInterval * sphere->getAcceleration());
		sphere->setSpeed(newVel);
		SPHERE_SPEED[2] = (float) ((sphere->getSpeed()).x());
		SPHERE_SPEED[0] = (float) ((sphere->getSpeed()).y());
		SPHERE_SPEED[1] = (float) ((sphere->getSpeed()).z());

		// compute position
		cVector3d spherePos = position + timeInterval * sphere->getSpeed() + cSqr(timeInterval) * sphere->getAcceleration();

		// update value to sphere object
		sphere->setLocalPos(spherePos);


		/////////////////////////////////////////////////////////////////////////
		// APPLY FORCES
		/////////////////////////////////////////////////////////////////////////

		// scale the force according to the max stiffness the device can render
		double stiffnessRatio = 1.0;
		if (hapticDeviceMaxStiffness < HAPTIC_STIFFNESS)
			stiffnessRatio = hapticDeviceMaxStiffness / HAPTIC_STIFFNESS;

		// send computed force to haptic device
		deviceTargetPos = (1.0 - devicePos.length()) * 800 * stiffnessRatio * sphereFce;

		double norm = deviceTargetPos.length();
		deviceTargetPos.x(0.0);
		deviceTargetPos.normalize();
		deviceTargetPos *= 0.03;

		if ((devicePos - deviceTargetPos).length() > 0.001)
		{
			gain += 1;
			forceToApply = gain * norm * (deviceTargetPos - devicePos);
		}
		else
		{
			gain = 1;
			forceToApply.zero();
		}

		hapticDevice->setForce(forceToApply);

		
	}

	// register data to be print into a text file
	logData(data, 0.1, "data.txt");

	// close  connection to haptic device
	hapticDevice->close();

	// exit haptics thread
	simulationFinished = true;
}


void logData(vector<double> data, double interval, string filename)

{
	filename = "../../projetIJL/GLUT/ForceDeLorentz/" + filename;

	ofstream file(filename, ios::out | ios::trunc);
	
	char sep = 9;
	if (!file)
	{
		cout << "l'ouverture du fichier " << filename << " a échoué." << endl;
		return;
	}

	for (int i = 0; i < data.size(); i++)
	{
		file << data[i] << sep << i * interval << endl;
	}

	file.close();
}


// TODO
// ajuster les valeurs des paramètres correctement
// permettre de compenser la force de Laplace avec le bras
// démo en 2 temps : - utilisateur 1 fait entrer la particule dans le champs et observe le changement de trajectoire (plusieurs essais)
//                   - même chose sauf que utilisateur 2 essaye de compenser la force de Laplace pour permettre à l'utilisateur 1 de traverser en ligne droite le champs
//                       (dessiner une ligne montrant la trajectoire à suivre)

//------------------------------------------------------------------------------