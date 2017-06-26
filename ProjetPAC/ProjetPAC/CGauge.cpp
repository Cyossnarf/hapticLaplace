#include "CGauge.h"
#include "widgets/CLevel.h"
#include "widgets/CLabel.h"
#include "widgets/CBitmap.h"

chai3d::cGauge::cGauge(cFontPtr a_font,
	const std::string a_text,
	cBitmap* a_icon,
	cBitmap* a_icon2,
	cColorf &a_col,
	const double a_minValue,
	const double a_maxValue,
	double a_step,
	const int a_id,
	const bool a_double,
	const bool a_toggle) : cPanel()

{
	// position of the gauge on screen
	this->m_id = a_id;
	// initial value of the gauge
	this->m_value;
	// min and max values of the gauge
	this->m_minValue = a_minValue;
	this->m_maxValue = a_maxValue;
	// incremential/decremential step value of the gauge
	this->m_step = a_step;
	// wether or not the gauge has two directions
	this->m_double = a_double;
	// wether or not the gauge is actually a toggle switch
	this->m_toggle = a_toggle;
	// color of the active part of the gauge
	this->m_col = a_col;

	// add a description text as a child
	cLabel* text = new cLabel(a_font);
	text->m_fontColor.setWhite();
	text->setText(a_text);
	this->addChild(text);

	// add 2 bitmap objects as children
	this->addChild(a_icon);
	this->addChild(a_icon2);

	if (!m_toggle)
	{
	//
	// add a level object as a child
	cLevel* level = new cLevel();
	level->setRange(0.0, a_maxValue);
	level->setWidth(20);
	level->rotateWidgetDeg(-90);
	this->addChild(level);

	if (m_double) {
		// add a second level object as a child
		level = new cLevel();
		level->setRange(0.0, a_maxValue);
		level->setWidth(20);
		level->setValue(0.0);
		level->rotateWidgetDeg(90);
		this->addChild(level);
	}
	//
	}
	else
	{
	//
	a_icon2->setEnabled(false);
	//
	}
}

void chai3d::cGauge::setValue(const double a_value)

{
	m_value = a_value;

	if (m_value > m_maxValue) {
		m_value = m_maxValue;
	}
	else if (m_double) {
		if (m_value < -m_maxValue) { m_value = -m_maxValue; }
	}
	else {
		if (m_value < m_minValue) { m_value = m_minValue; }
	}
}

void chai3d::cGauge::incr()

{
	setValue(m_value + m_step);
	if (m_double && m_value == 0.0) {
		m_value += m_step;
	}
}

void chai3d::cGauge::decr()

{
	setValue(m_value - m_step);
	if (m_double && m_value == 0.0) {
		m_value -= m_step;
	}
}

// Update the gauge for the rendering on screen
// ...
// All gauges on screen are children of a panel frame
// a_frameWidth is the width of the frame
// a_upBoundary is the up border of the frame
// a_downBoundary is the down border of the frame
// a_gaugesNumber is the nb of gauges in the frame
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

	cLabel* txt = (cLabel*)getChild(0);
	cBitmap* ico = (cBitmap*)getChild(1);
	cBitmap* ico_2 = (cBitmap*)getChild(2);
	//cLevel* lvl = (cLevel*)getChild(3);
	//cLevel* lvl_2 = (cLevel*) getChild(4);

	// set size and position of the gauge
	setLocalPos(0, a_upBoundary - m_id * frameHeight / a_gaugesNumber - gaugeHeight - 15);
	setSize(a_frameWidth, gaugeHeight);

	// set position of the text child
	txt->setLocalPos(15, gaugeHeight - 25);

	if (!m_toggle)
	{
	//
	cLevel* lvl = (cLevel*)getChild(3);
	lvl->m_colorActive = m_col;

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
		cLevel* lvl_2 = (cLevel*)getChild(4);

		// number of increments of each level child
		int levelNumIncrements = (int)((a_frameWidth / 2 - frameMarge - iconWidth) / 2);

		// set caracteristics of the right-directed level child
		lvl->setLocalPos(a_frameWidth / 2 + 1, gaugeHeight - 20 - levelHeight);
		lvl->setNumIncrements(levelNumIncrements);
		lvl->setValue(cMax(0.0, m_value));

		// set caracteristics of the left-directed level child
		lvl_2->setLocalPos(a_frameWidth / 2 - 1, gaugeHeight - 20 - 2 * levelHeight);
		lvl_2->setNumIncrements(levelNumIncrements);
		lvl_2->setValue(cMax(0.0, -m_value));
		lvl_2->m_colorActive = m_col;
	}
	//
	}
	else
	{
	//
	ico->setLocalPos((a_frameWidth - iconWidth) / 2, 30 - iconWidth / 2);
	ico_2->setLocalPos((a_frameWidth - iconWidth) / 2, 30 - iconWidth / 2);

	ico->setEnabled(m_value > 0);
	ico_2->setEnabled(m_value < 0);
	//
	}
}