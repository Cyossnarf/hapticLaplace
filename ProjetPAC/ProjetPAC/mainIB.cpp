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
#include "CMobileCam.h"
#include "CArrow.h"
#include "CGauge.h"
#include "CConsol.h"
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

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


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
bool fullscreen = true;

// top camera enabled
bool showTopView = true;

int counterForceFactor = 0;

// variable selected for modification
int selection = 0;

// mouse input mode
bool mouseMode = false;


//------------------------------------------------------------------------------
// DECLARED CONSTANTS
//------------------------------------------------------------------------------

// radius of the sphere
const double SPHERE_RADIUS = 0.006;
// mass of the sphere (initial value)
const double SPHERE_MASS = 0.04;
// charge of the sphere (initial value)
const double SPHERE_CHARGE = 1.0;
// damping of the sphere
const double SPHERE_DAMPING = 0.999;
// launch position of the sphere in the first level
const cVector3d SPHERE_POS1 = cVector3d(0.0, -0.23, 0.0);
// launch position of the sphere in the second level
const cVector3d SPHERE_POS2 = cVector3d(0.02, 0.0, 0.0);
// radius of the target
const double TARGET_RADIUS = 0.012;
// intensity of the current (initial value)
const double CURRENT_INTENSITY = 0.5;

// stiffness of the haptic device
const double HAPTIC_STIFFNESS = 1000.0;

// launch number per level
const int LAUNCH_NUMBER = 15;

// colors
const cColorf backgroundColor = cColorf(52 / 255.f, 48 / 255.f, 71 / 255.f);
const cColorf panelColor = cColorf(125 / 255.f, 117 / 255.f, 164 / 255.f);
const cColorf blackColor = cColorf(8 / 255.f, 9 / 255.f, 16 / 255.f);
const cColorf panelSelectedColor = cColorf(175 / 255.f, 117 / 255.f, 164 / 255.f);
const cColorf lvlColor = cColorf(255.f, 255.f, 255.f);
const cColorf lvlSelectedColor = cColorf(255.f, 255.f, 255.f);


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

// the target to strike
cShapeSphere *cible;

// (big) visual representation of the magnetic field vector
cArrow *bigMagnetFieldVector;
// visual representation of the magnetic field vector
cArrow *magnetFieldVector;
// visual representation of the sphere speed vector
cArrow *speedVector;
// visual representation of the magnetic force vector
cArrow *forceVector;
// visual representation of the aim vector (~initial speed)
cArrow *aimVector;
cArrow *aimXVector;
cArrow *aimYZVector;

// a bar to display info
TwBar *bar1;
// a label to explain what is happening
cLabel* labelMessage;
// a label to display the values of the variables of the simulation
cLabel* labelInfo;

// panels for the GUI
cPanel* pan1;
cPanel* pan2;
cPanel* pan3;

// consols to display info about the game
cConsol* levelCon;
cConsol* scoreCon;
cConsol* highscoreCon;
cConsol* launchCon;

// gauges to graphically represent variable values of the simulation
cGauge* masseGauge;
cGauge* chargeGauge;
cGauge* intensiteGauge;
cGauge* opacityGauge;
cGauge* viewGauge;

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

// information about the ATB bar
int barSizeW;
int barSizeH;
int barPosX;
int barPosY;
string barPos;
string barSize;
string barDef;

// indicates if the field/speed/force triaedra is displayed
bool triaedraDisplay = true;

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

// sphere velocity given by user
cVector3d userVel;

// clock's time step
double timeInterval;

// position of the device
cVector3d devicePos;

// position of the sphere
cVector3d spherePos;

// launch position of the sphere
cVector3d posref = SPHERE_POS1;

// nombre de tirs
int nbvisees = 0;
// wether or not the sphere hit the target
bool touche = false;
// score
int score = 0;
// highscore
int highscore = 0;
// mode de jeu
int mode_j = 1;

// buttons input
bool shiftButtonPressed;
bool plusButtonPressed;
bool minusButtonPressed;
bool launchButtonPressed;
bool anyPreviouslyPressed = false;

//affichage infos hors jeu
bool tirpossible = true;//gdo

// Ce qui suit est utilisé pour l'affichage du GUI (AntTweakBar précisément)
// C'est un peu bizarre parce qu'on devrait logiquement appeler des attributs des objets magnetsphere et magnetfield
// Mais le code n'a pas été adapté pour ça...

// speed vector of the sphere
float SPHERE_SPEED[3] = { 0.0f, 0.0f, 0.0f };//double *SPHERE_SPEED;
cVector3d sphere_speed;

// magnetic force applied to the sphere
float SPHERE_FORCE[3] = { 0.0f, 0.0f, 0.0f };
cVector3d sphere_force;

// magnetic field's orientation
float MAGN_FIELD[3] = { 0.0f, 0.0f, 1.0f };
cVector3d magn_field;


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

// function that loads an image
bool loadImage(cBitmap* image, const string filename);

// reset function
void reset();


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
	cout << "Copyright 2003-2015" << endl;
	cout << "Haptic Laplace / MAGNETICA STRIKE" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[0] - accept/launch" << endl;
	cout << "[2] - select next variable" << endl;
	cout << "[+/-] - increase/decrease selected variable" << endl;
	cout << "[b] - Enable/Disable big vector display" << endl;
	cout << "[c] - Change camera position" << endl;
	cout << "[d] - Enable/Disable triaedra display" << endl;
	cout << "[e] - Enable/Disable info display" << endl;
	cout << "[f] - Enable/Disable full screen mode" << endl;
	cout << "[m] - Enable/Disable mousemode input" << endl;
	cout << "[r] - reset position of the sphere" << endl;
	cout << "[ESC] - Exit application" << endl;
	cout << endl << endl;

	// parse first arg to try and locate resources
	resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1);


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
	windowW = (int)(0.8 * screenH);
	windowH = (int)(0.5 * screenH);
	windowPosY = (screenH - windowH) / 2;
	windowPosX = (screenW - windowW) / 2;

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
	//world->m_backgroundColor = backgroundColor;

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
	// CREATE SPHERE, FIELD, TRAJECTORY AND ARROWS
	//--------------------------------------------------------------------------

	// create the sphere
	sphere = new cMagnetSphere(SPHERE_RADIUS, SPHERE_MASS, SPHERE_CHARGE, SPHERE_DAMPING);
	// add the sphere to the world
	world->addChild(sphere);
	// set position of the sphere
	sphere->setLocalPos(SPHERE_POS1);
	// set color of the sphere
	sphere->m_material->setBlueDark();

	// create the dots of the sphere's trajectory
	cShapeSphere* miniSphere;
	for (int i = 0; i < sphere->getCourseSize(); i++) {
		miniSphere = new cShapeSphere(SPHERE_RADIUS * 0.2);
		world->addChild(miniSphere);
		miniSphere->setLocalPos(SPHERE_POS1);
		sphere->addToCourse(miniSphere);
	}

	posref = sphere->getLocalPos();

	// create the target
	cible = new cShapeSphere(TARGET_RADIUS);
	cible->setLocalPos(0.00, 0.23, 0.0);
	cible->m_material->setRed();
	world->addChild(cible);

	// create a magnetic field
	magnetField = new cMagnetField();

	// add the magnetic field to the world
	world->addChild(magnetField);

	// set position of the magnetic field
	magnetField->setLocalPos(-magnetField->getLength(), 0.0, 0.0);//(0.0, 0.05, 0.01);

	// create arrows
	bigMagnetFieldVector = new cArrow(cVector3d(-0.35, 0.0, 0.0), cColorb(0, 0, 255), 0.01, 0.02, 0.3);
	magnetFieldVector = new cArrow(SPHERE_POS1, cColorb(0, 0, 255), 0.002, 0.004, 0.03);
	speedVector = new cArrow(SPHERE_POS1, cColorb(0, 255, 0), 0.002, 0.004, 0.03);
	forceVector = new cArrow(SPHERE_POS1, cColorb(255, 0, 0), 0.002, 0.004, 0.03);
	aimVector = new cArrow(SPHERE_POS1, cColorb(0, 255, 0), 0.002, 0.004, 0.03, 0.25, 5);
	aimXVector = new cArrow(SPHERE_POS1, cColorb(255, 255, 255), 0.002, 0.004, 0.03, 0.25, 5);
	aimYZVector = new cArrow(SPHERE_POS1, cColorb(255, 255, 255), 0.002, 0.004, 0.03, 0.25, 5);

	// add the arrows to the world
	world->addChild(bigMagnetFieldVector);
	world->addChild(magnetFieldVector);
	world->addChild(speedVector);
	world->addChild(forceVector);
	world->addChild(aimVector);
	world->addChild(aimXVector);
	aimXVector->setEnabled(false, true);// this arrow is not used at the beginning
	world->addChild(aimYZVector);
	aimYZVector->setEnabled(false, true);// this arrow is not used at the beginning

	//--------------------------------------------------------------------------
	// BARS
	//--------------------------------------------------------------------------

	// create a bar
	bar1 = TwNewBar("Bonjour");

	// define parameters of the bar
	TwDefine(" Bonjour refresh=0.1 ");

	// add variables
	//TwAddVarRW(bar1, "Charge", TW_TYPE_DOUBLE, &SPHERE_CHARGE, "");
	TwAddVarRW(bar1, "Champ magnétique", TW_TYPE_DIR3F, &MAGN_FIELD, " opened=true showval=false arrowcolor='0 0 255' ");
	TwAddVarRW(bar1, "Vitesse", TW_TYPE_DIR3F, &SPHERE_SPEED, " opened=true showval=false arrowcolor='0 255 0' ");
	TwAddVarRW(bar1, "Force magnétique", TW_TYPE_DIR3F, &SPHERE_FORCE, " opened=true showval=false arrowcolor='255 0 0' ");

	//--------------------------------------------------------------------------
	// WIDGETS
	//--------------------------------------------------------------------------

	// create a background
	cBackground* background = new cBackground();
	camera->m_backLayer->addChild(background);

	// set aspect ration of background image a constant
	background->setFixedAspectRatio(true);

	// load background image
	bool fileload = loadImage(background, "background.png");
	if (!fileload) { return (-1); }

	// create panels for the GUI
	pan1 = new cPanel();
	pan1->setColor(backgroundColor);
	camera->m_backLayer->addChild(pan1);

	pan2 = new cPanel();
	pan2->setColor(panelColor);
	pan2->setTransparencyLevel(0.5f);
	camera->m_frontLayer->addChild(pan2);

	pan3 = new cPanel();
	pan3->setColor(panelColor);
	pan3->setSize(530, 80);
	pan3->setCornerRadius(5, 5, 5, 5);
	camera->m_frontLayer->addChild(pan3);

	// create fonts
	cFontPtr font_pt = cFontPtr(new cFont());
	fileload = font_pt->loadFromFile(RESOURCE_PATH("../resources/futura20.fnt"));
	cFontPtr titleFont_pt = cFontPtr(new cFont());
	fileload = fileload && titleFont_pt->loadFromFile(RESOURCE_PATH("../resources/earthorbiter72b.fnt"));
	cFontPtr scoreFont_pt = cFontPtr(new cFont());
	fileload = fileload && scoreFont_pt->loadFromFile(RESOURCE_PATH("../resources/crystal40.fnt"));
	//cFontPtr font = NEW_CFONTCALIBRI20();
	if (!fileload)
	{
		cout << "Error - Font failed to load correctly." << endl;
		close();
		return (-1);
	}

	// create a label with a small message
	labelMessage = new cLabel(titleFont_pt);
	camera->m_frontLayer->addChild(labelMessage);
	// set font color
	labelMessage->m_fontColor.setWhite();
	// set text message
	labelMessage->setText("MAGNETICA STRIKE");
	//labelMessage->s

	// create a label to display variable values of the simulation
	labelInfo = new cLabel(font_pt);
	labelInfo->m_fontColor.setRed();
	camera->m_frontLayer->addChild(labelInfo);
	labelInfo->setEnabled(false);

	// create consols to display info about the game
	levelCon = new cConsol(100.0, 60.0, blackColor, font_pt, scoreFont_pt, "LEVEL");
	levelCon->setColor(blackColor);
	levelCon->setCornerRadius(5, 0, 5, 0);
	pan3->addChild(levelCon);
	levelCon->setLocalPos(10.0, 10.0);

	scoreCon = new cConsol(120.0, 60.0, blackColor, font_pt, scoreFont_pt, "SCORE", false);
	scoreCon->setColor(blackColor);
	scoreCon->setCornerRadius(5, 0, 5, 0);
	pan3->addChild(scoreCon);
	scoreCon->setLocalPos(140.0, 10.0);

	highscoreCon = new cConsol(120.0, 60.0, blackColor, font_pt, scoreFont_pt, "HIGHSCORE");
	highscoreCon->setColor(blackColor);
	highscoreCon->setCornerRadius(0, 5, 0, 5);
	pan3->addChild(highscoreCon);
	highscoreCon->setLocalPos(270.0, 10.0);

	launchCon = new cConsol(100.0, 60.0, blackColor, font_pt, scoreFont_pt, "STRIKES");
	launchCon->setColor(blackColor);
	launchCon->setCornerRadius(0, 5, 0, 5);
	pan3->addChild(launchCon);
	launchCon->setLocalPos(420.0, 10.0);

	// create gauges to graphically represent variable values of the simulation
	cBitmap* icon1 = new cBitmap();
	fileload = loadImage(icon1, "masse.png");
	cBitmap* icon2 = new cBitmap();
	fileload = fileload && loadImage(icon2, "plume.png");
	if (!fileload) { return (-1); }
	masseGauge = new cGauge(font_pt, "Masse de la particule", icon1, icon2, 0.01, 0.35, 0.01, 0);
	masseGauge->setColor(panelSelectedColor);
	masseGauge->setCornerRadius(5, 5, 5, 5);
	masseGauge->setValue(sphere->getMass());
	pan2->addChild(masseGauge);

	icon1 = new cBitmap();
	fileload = loadImage(icon1, "plus.png");
	icon2 = new cBitmap();
	fileload = fileload && loadImage(icon2, "moins.png");
	if (!fileload) { return (-1); }
	chargeGauge = new cGauge(font_pt, "Charge de la particule", icon1, icon2, 0.0, 5.0, 0.5, 1, true);
	chargeGauge->setColor(panelColor);
	chargeGauge->setCornerRadius(5, 5, 5, 5);
	chargeGauge->setValue(sphere->getCharge());
	pan2->addChild(chargeGauge);

	icon1 = new cBitmap();
	fileload = loadImage(icon1, "aimantgros.png");
	icon2 = new cBitmap();
	fileload = fileload && loadImage(icon2, "aimantpetit.png");
	if (!fileload) { return (-1); }
	intensiteGauge = new cGauge(font_pt, "Intensite du champ magnetique", icon1, icon2, 0.5, 5.0, 0.5, 2);
	intensiteGauge->setColor(panelColor);
	intensiteGauge->setCornerRadius(5, 5, 5, 5);
	intensiteGauge->setValue(magnetField->getCurrentIntensity());
	pan2->addChild(intensiteGauge);

	icon1 = new cBitmap();
	fileload = loadImage(icon1, "opaque.png");
	icon2 = new cBitmap();
	fileload = fileload && loadImage(icon2, "transparent.png");
	if (!fileload) { return (-1); }
	opacityGauge = new cGauge(font_pt, "Opacite du champ magnetique", icon1, icon2, 0.0, 1.0, 0.05, 3);
	opacityGauge->setColor(panelColor);
	opacityGauge->setCornerRadius(5, 5, 5, 5);
	opacityGauge->setValue(magnetField->getTransparency());
	pan2->addChild(opacityGauge);

	icon1 = new cBitmap();
	fileload = loadImage(icon1, "cube2.png");
	icon2 = new cBitmap();
	fileload = fileload && loadImage(icon2, "cube1.png");
	if (!fileload) { return (-1); }
	viewGauge = new cGauge(font_pt, "Point de vue de la camera", icon1, icon2, -2.0, 2.0, 0.0, 4, false, true);
	viewGauge->setColor(panelColor);
	viewGauge->setCornerRadius(5, 5, 5, 5);
	viewGauge->setValue(1.0);
	pan2->addChild(viewGauge);


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

void reset()
{
	if (nbvisees == LAUNCH_NUMBER) {
		mode_j = 3 - mode_j;
		nbvisees = 0;
	}

	if (mode_j == 1) {
		posref = SPHERE_POS1;

		if (nbvisees == 0) {
			score = 0;
			cible->setLocalPos(0.00, 0.23, 0.0);
			tirpossible = true;//gdo
			bigMagnetFieldVector->setEnabled(true, true);

			// reinitialize parameters' values
			sphere->setMass(SPHERE_MASS);
			masseGauge->setValue(SPHERE_MASS);
			sphere->setCharge(SPHERE_CHARGE);
			chargeGauge->setValue(SPHERE_CHARGE);
			magnetField->setCurrentIntensity(CURRENT_INTENSITY);
			intensiteGauge->setValue(CURRENT_INTENSITY);
			magnetField->setTransparency(1.0);
			opacityGauge->setValue(1.0);
			if (camera->getState() == -1) {
				camera->setInMovement();
				viewGauge->setValue(1);
			}
		}
		else if (touche) {
			cible->setLocalPos(0.00, FRAND(0.0, 0.27), FRAND(-0.15, 0.15));
			touche = false;
		}
	}
	else if (mode_j == 2) {
		posref = SPHERE_POS2;

		if (nbvisees == 0) {
			cible->setLocalPos(-0.4, 0.0, 0.0);
			tirpossible = true;//gdo
			bigMagnetFieldVector->setEnabled(false, true);

			// prepare parameters' values for level 2
			if (magnetField->getTransparency() > 0.5) {
				magnetField->setTransparency(0.5);
				opacityGauge->setValue(0.5);
			}
			if (camera->getState() == 1) {
				camera->setInMovement();
				viewGauge->setValue(-1);
			}
		}
		else if (touche) {
			cible->setLocalPos(-0.4, FRAND(-0.084, 0.084), FRAND(-0.084, 0.084));
			touche = false;
		}

		aimXVector->setEnabled(true, true);
		aimXVector->setEnabled(false);
		aimXVector->setLocalPos(posref);

		aimYZVector->setEnabled(true, true);
		aimYZVector->setEnabled(false);
		aimYZVector->setLocalPos(posref);
	}

	sphere->setLocalPos(posref);
	sphere->setAcceleration(0.0, 0.0, 0.0);
	sphere->setSpeed(0.0, 0.0, 0.0);
	sphere->resetCourse();

	aimVector->setEnabled(true, true);
	aimVector->setEnabled(false);
	aimVector->setLocalPos(posref);
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

	// option d: toggle triaedra display
	if (key == 'd')
	{
		triaedraDisplay = !triaedraDisplay;
		magnetFieldVector->setEnabled(triaedraDisplay, true);
		speedVector->setEnabled(triaedraDisplay, true);
		forceVector->setEnabled(triaedraDisplay, true);
	}

	// option b: toggle bigMagnetFieldVector display
	if (key == 'b') { bigMagnetFieldVector->setEnabled(!bigMagnetFieldVector->getEnabled(), true); }

	// option e: toggle labelInfo display
	if (key == 'e') { labelInfo->setEnabled(!labelInfo->getEnabled()); }

	// option m: toggle mouse input mode
	if (key == 'm') { mouseMode = !mouseMode; }

	// option r: reset position of the sphere
	if (key == 'r') { reset(); }

	// option c: start camera movement
	if (key == 'c') { camera->setInMovement(); }

	// option 2: shift selection to the next parameter
	if (key == '2')
	{
		((cGauge*)pan2->getChild(selection))->setColor(panelColor);

		selection += 1;
		if (selection > 4) { selection = 0; }

		((cGauge*)pan2->getChild(selection))->setColor(panelSelectedColor);
	}

	// option 0: accept/launch
	if (key == '0')
	{
		if (tirpossible)
		{
			//cVector3d newVel(userVel + sphere->getSpeed() + timeInterval * sphere->getAcceleration());//grospb
			//sphere->setSpeed(newVel);
			sphere->setSpeed(userVel * 8);
			nbvisees += 1;

			aimVector->setEnabled(false, true);
			aimXVector->setEnabled(false, true);
			aimYZVector->setEnabled(false, true);
		}//gdo
	}

	// option +/-: increase/decrease the selected variable
	if (key == '+' || key == '-')
	{
		//printf("Salut,");//debug
		if (selection == 4) {
			camera->setInMovement();
			viewGauge->setValue(-camera->getState());
		}
		else {
			if (key == '+') { ((cGauge*)pan2->getChild(selection))->incr(); }
			else { ((cGauge*)pan2->getChild(selection))->decr(); }

			sphere->setMass(masseGauge->getValue());
			sphere->setCharge(chargeGauge->getValue());
			magnetField->setCurrentIntensity(intensiteGauge->getValue());
			magnetField->setTransparency(opacityGauge->getValue());
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
	/////////////////////////////////////////////////////////////////////
	// UPDATE PURELY GRAPHICAL ELEMENTS
	/////////////////////////////////////////////////////////////////////

	// some arrows are positioned and orientated on the sphere
	magnetFieldVector->setLocalPos(sphere->getLocalPos());

	speedVector->setLocalPos(sphere->getLocalPos());
	speedVector->updateArrow(sphere_speed, 0.08);//0.23);//pb1

	forceVector->setLocalPos(sphere->getLocalPos());
	forceVector->updateArrow(sphere_force, 1.0);//3.0);
	
	aimVector->updateArrow(devicePos, 1.0);
	aimXVector->updateArrow(cVector3d(devicePos.x(), 0.0, 0.0), 1.0);
	aimYZVector->updateArrow(cVector3d(0.0, devicePos.y(), devicePos.z()), 1.0);

	// move the camera
	if (camera->isInMovement())
	{
		camera->moveCam();
	}

	if (camera->isInMovement() || camera->getState() != 1)
	{
		// z-axis-rotate the GUI vectors, so they match the camera's new point of view
		sphere_force = camera->getRotation() * sphere_force;
		sphere_speed = camera->getRotation() * sphere_speed;
		magn_field = camera->getRotation() * cVector3d(1.0, 0.0, 0.0);
		//printf((magn_field.str() + "\n").c_str());//debug

		// update for the twBar
		MAGN_FIELD[2] = (float)(magn_field.x());
		MAGN_FIELD[0] = (float)(magn_field.y());
		MAGN_FIELD[1] = (float)(magn_field.z());
	}

	// updates for the twBar
	SPHERE_SPEED[2] = (float)(sphere_speed.x());
	SPHERE_SPEED[0] = (float)(sphere_speed.y());
	SPHERE_SPEED[1] = (float)(sphere_speed.z());

	SPHERE_FORCE[2] = (float)(sphere_force.x());
	SPHERE_FORCE[0] = (float)(sphere_force.y());
	SPHERE_FORCE[1] = (float)(sphere_force.z());

	// update for the sphere's course
	sphere->updateCourse();

	/////////////////////////////////////////////////////////////////////
	// UPDATE WIDGETS
	/////////////////////////////////////////////////////////////////////

	// update variable values
	labelInfo->setText(cStr(nbvisees)+" | field opacity : " + cStr(magnetField->getTransparency() * 100)
		+ "% | sphere mass : " + cStr(sphere->getMass()) 
		+ " kg | sphere charge : " + cStr(sphere->getCharge()) 
		+ " C | current intensity : " + cStr(magnetField->getCurrentIntensity()) + " A");
	//cStr(nbtouches)
	//cStr(sphere_force.length())//cStr(atan2(-devicePos.x(), devicePos.y()))
	//devicePos.str()+" "+userVel.str()

	// update position of info label
	labelInfo->setLocalPos(15, windowH - 25);
	// update position of message label
	labelMessage->setLocalPos(0.5 * (0.75 * windowW - labelMessage->getWidth()), 0.925 * windowH - 40);

	// set width and height of panels
	pan1->setSize(windowW * 0.7, windowH * 0.8);
	pan1->setCornerRadius(0, windowH * 0.1, windowH * 0.1, 0);
	pan2->setSize(windowW * 0.2, windowH);

	// assign a position (x,y) to panels
	pan1->setLocalPos(windowW * 0.05, windowH * 0.05);
	pan2->setLocalPos(windowW * 0.775, 0.0);
	pan3->setLocalPos(windowW * 0.4 - 265, windowH * 0.05 - 40);

	// update values displayed by consols
	levelCon->update(cStr(mode_j));
	scoreCon->update(cStr(score));
	highscoreCon->update(cStr(highscore));
	launchCon->update(cStr(LAUNCH_NUMBER - nbvisees));

	// update positions and dimensions of gauges
	masseGauge->update(windowW * 0.2, windowH, windowH * 0.35, 5);
	chargeGauge->update(windowW * 0.2, windowH, windowH * 0.35, 5);
	intensiteGauge->update(windowW * 0.2, windowH, windowH * 0.35, 5);
	opacityGauge->update(windowW * 0.2, windowH, windowH * 0.35, 5);
	viewGauge->update(windowW * 0.2, windowH, windowH * 0.35, 5);
	
	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE
	/////////////////////////////////////////////////////////////////////

	// update shadow maps (if any)
	world->updateShadowMaps();
	
	// render world
	camera->renderView_custom((int)(windowW * 0.05),
		(int)(windowH *0.05),
		(int)(windowW * 0.7),
		(int)(windowH * 0.8),
		windowW,
		windowH);

	// telling AntTweakBar the size of the graphic window
	TwWindowSize(windowW, windowH);

	// display bars
	barPosX = (int)(windowW*0.775);
	barPosY = (int)(windowH*0.65);
	barSizeW = (int)(windowW*0.2);
	barSizeH = (int)(windowH*0.35);
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
	
	// store the current position of the haptic device//jp
	//cVector3d devicePos(0.0, 0.0, 0.0);

	// store the force the user applies to the haptic device
	cVector3d userForce(0.0, 0.0, 0.0);//mq

	// store the desired position of the haptic device
	cVector3d deviceTargetPos(0.0, 0.0, 0.0);

	// store the force to apply to the device
	cVector3d forceToApply(0.0, 0.0, 0.0);

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

		// stop the simulation clock
		clock.stop();

		// read the time increment in seconds
		timeInterval = cMin(0.001, clock.getCurrentTimeSeconds());

		// restart the simulation clock
		clock.reset();
		clock.start();

		// update frequency counter
		frequencyCounter.signal(1);


		/////////////////////////////////////////////////////////////////////////
		// READ HAPTIC DEVICE
		/////////////////////////////////////////////////////////////////////////

		//parametres
		hapticDevice->getUserSwitch(1, minusButtonPressed);
		hapticDevice->getUserSwitch(3, plusButtonPressed);
		hapticDevice->getUserSwitch(2, shiftButtonPressed);
		hapticDevice->getUserSwitch(0, launchButtonPressed);

		if (!anyPreviouslyPressed)
		{
			if (shiftButtonPressed) { keySelect('2', 0, 0); }
			if (plusButtonPressed) { keySelect('+', 0, 0); }
			if (minusButtonPressed) { keySelect('-', 0, 0); }
			if (launchButtonPressed && sphere->getLocalPos().equals(posref, 0.005)) {
				keySelect('0', 0, 0);
			}//gdo
		}

		anyPreviouslyPressed = minusButtonPressed || plusButtonPressed || shiftButtonPressed || launchButtonPressed;

		// read device position
		if (mouseMode) {
			if (mode_j == 1) {
				devicePos = cVector3d(0.0, 0.0001 * (mousePosx - 0.5 * screenW), 0.0001 * (0.5 * screenH - mousePosy));
			}
			else {
				devicePos = cVector3d(-0.0001 * (0.5 * screenH - mousePosy), 0.0001 * (mousePosx - 0.5 * screenW), 0.0);
			}
		}
		else {
			hapticDevice->getPosition(devicePos);
		}

		// read sphere position
		spherePos = sphere->getLocalPos();

		// get sphere velocity from user
		//cVector3d userVel(userMovex, userMovey, userMovez);
		/*
		if (resetCount == 0 || resetCount % 10 != 0)
			userVel = userVel - userVel.dot(magnetField->getDirection()) * magnetField->getDirection();
		else
			userVel = userVel.dot(magnetField->getDirection()) * magnetField->getDirection();
		*/
		//userVel = userVel - userVel.dot(magnetField->getDirection()) * magnetField->getDirection();

		// get sphere velocity from user
		//userVel = cVector3d(devicePos.x()*0.001, devicePos.y()*0.001, devicePos.z()*0.001);//pb1//grospb
		userVel = devicePos;
		if (mode_j == 1) {
			userVel = (userVel - userVel.dot(magnetField->getDirection()) * magnetField->getDirection());
		}
		/*//grospb
		// limit the velocity maximal that the user can give to the sphere
		if (userVel.length() > 0.0008)
		{
			userVel.normalize();
			userVel *= 0.0008;
		}
		*/

		/////////////////////////////////////////////////////////////////////////
		// UPDATE SIMULATION
		/////////////////////////////////////////////////////////////////////////

		// clear forces for the sphere
		cVector3d sphereFce;
		sphereFce.zero();

		if (magnetField->contain(*sphere))
		{
			// compute magnetic forces
			sphereFce.add(magnetField->magnetForce(*sphere, sphere->getCharge(), sphere->getSpeed()));
			
			/*//mq
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
			*///mq
		}
		/*//mq
		else
		{
			userForce.zero();
			counterForceFactor = 0;
		}
		*///mq

		sphere_force = sphereFce;//fusionner les 2 vars?

		// compute acceleration
		sphere->setAcceleration(sphereFce / sphere->getMass());//sphere->setAcceleration((sphereFce + userForce) / sphere->getMass());//mqq

		// compute velocity (if the sphere has already been launched)
		if (!sphere->getLocalPos().equals(posref, 0.005)) {
			cVector3d newVel(sphere->getSpeed() + timeInterval * sphere->getAcceleration());
			sphere->setSpeed(newVel);
		}
		sphere_speed = sphere->getSpeed();

		// compute position
		spherePos = spherePos + timeInterval * sphere->getSpeed() + cSqr(timeInterval) * sphere->getAcceleration();

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

		/*if (mode_j > 2)//pb1
		{
			hapticDevice->setForce(forceToApply);
		}*/

		// check if the sphere hits the target
		if (sphere->getLocalPos().equals(cible->getLocalPos(), TARGET_RADIUS)) {
			touche = true;
			score += 1;
			if (score > highscore) {
				highscore = score;
			}
			reset();
		}

		// check if the sphere is out of the limited space
		if (sphere->getLocalPos().x() > 0.1 ||
			sphere->getLocalPos().x() < -0.4 ||
			sphere->getLocalPos().y() > 0.3 ||
			sphere->getLocalPos().y() < -0.3 ||
			sphere->getLocalPos().z() > 0.18 ||
			sphere->getLocalPos().z() < -0.18) {
			reset();
		}
		
	}

	// close  connection to haptic device
	hapticDevice->close();

	// exit haptics thread
	simulationFinished = true;
}

bool loadImage(cBitmap* a_image, const string a_filename)

{
	bool fileload = a_image->loadFromFile(RESOURCE_PATH("../resources/images/" + a_filename));
	if (!fileload)
	{
#if defined(_MSVC)
		fileload = a_image->loadFromFile("../../../bin/resources/images/" + a_filename);
#endif
	}
	if (!fileload)
	{
		cout << "Error - Image failed to load correctly." << endl;
		//printf(RESOURCE_PATH("../resources/images/spheremap-1.jpg"));//debug
		close();
	}
	return fileload;
}

//------------------------------------------------------------------------------