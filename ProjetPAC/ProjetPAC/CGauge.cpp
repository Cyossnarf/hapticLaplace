#include "CGauge.h"
#include "widgets/CLevel.h"
#include "widgets/CLabel.h"
#include "widgets/CBitmap.h"

chai3d::cGauge::cGauge(cFontPtr a_font, const std::string a_text, cBitmap* a_icon, cBitmap* a_icon2, const int a_id, const bool a_double) : cPanel()

{
	// position of the gauge on screen
	this->m_id = a_id;
	// initial value of the gauge
	this->m_value = 0.5;
	// wether the gauge has two directions
	this->m_double = a_double;
	
	// add a description text as a child
	cLabel* text = new cLabel(a_font);
	text->m_fontColor.setWhite();
	text->setText(a_text);
	this->addChild(text);
	
	// add a level object as a child
	cLevel* level = new cLevel();
	level->setRange(0.0, 1.0);
	level->setWidth(20);
	//level->setSingleIncrementDisplay(true);
	level->setValue(0.5);
	level->rotateWidgetDeg(-90);
	//level->m_colorActive.setRed();
	this->addChild(level);

	// add 2 bitmap objects as children
	this->addChild(a_icon);
	this->addChild(a_icon2);

	if (m_double) {
		// add a second level object as a child
		level = new cLevel();
		level->setRange(0.0, 1.0);
		level->setWidth(20);
		level->setValue(0.0);
		level->rotateWidgetDeg(90);
		this->addChild(level);
	}
}

void chai3d::cGauge::update(const double a_frameWidth,
	const double a_upBoundary,
	const double a_downBoundary,
	const int a_gaugesNumber)

{
	// height of the parent frame of the gauges
	double frameHeight = a_upBoundary - a_downBoundary;
	// height of the cLevel  child(ren)
	double levelHeight = 20;//frameHeight * 0.045;
	// global height of the gauge
	double gaugeHeight = levelHeight + 60;
	// marge of the gauge's borders
	int frameMarge = 10;
	// width/height of the icon images
	int iconWidth = 40;

	cLabel* txt = (cLabel*) getChild(0);
	cLevel* lvl = (cLevel*) getChild(1);
	cBitmap* ico = (cBitmap*) getChild(2);
	cBitmap* ico_2 = (cBitmap*)getChild(3);

	// set size and position of the gauge
	setLocalPos(0, a_upBoundary - m_id * frameHeight / a_gaugesNumber - gaugeHeight - 15);
	setSize(a_frameWidth, gaugeHeight);

	// set position of the text child
	txt->setLocalPos(15, gaugeHeight - 20);

	// set positions of the icons children
	ico->setLocalPos(a_frameWidth - frameMarge - iconWidth + 5, 30 - iconWidth / 2);
	ico_2->setLocalPos(frameMarge - 5, 30 - iconWidth / 2);

	if (!m_double) {
		// number of increments (~width/2) of the level child
		int levelNumIncrements = (int)(a_frameWidth / 2 - frameMarge - iconWidth);

		// set caracteristics of the level child
		lvl->setLocalPos(frameMarge + iconWidth, gaugeHeight - 20 - levelHeight);
		lvl->setNumIncrements(levelNumIncrements);
		lvl->setValue(m_value);
	}
	else {
		cLevel* lvl_2 = (cLevel*) getChild(4);

		// number of increments of each level child
		int levelNumIncrements = (int)((a_frameWidth / 2 - frameMarge - iconWidth) / 2);

		// set caracteristics of the right-directed level child
		lvl->setLocalPos(a_frameWidth / 2 + 1, gaugeHeight - 20 - levelHeight);
		lvl->setNumIncrements(levelNumIncrements);
		lvl->setValue(m_value);

		// set caracteristics of the left-directed level child
		lvl_2->setLocalPos(a_frameWidth / 2 - 1, gaugeHeight - 20 - 2 * levelHeight);
		lvl_2->setNumIncrements(levelNumIncrements);
		lvl_2->setValue(0.0);
	}

	
	//lev->setSize(levelHeight);
	//lev->m_colorActive.setRed();
}