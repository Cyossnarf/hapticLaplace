#include "CGauge.h"
#include "widgets/CLevel.h"
#include "widgets/CLabel.h"

chai3d::cGauge::cGauge(cFont* a_font, const std::string a_text, const int a_id) : cPanel()

{
	this->id = a_id;
	
	// add a description text
	cLabel* text = new cLabel(a_font);
	text->m_fontColor.setWhite();
	text->setText(a_text);
	this->addChild(text);
	
	// add a level
	cLevel* level = new cLevel();
	level->setRange(0.0, 1.0);
	level->setSize(20);
	//level->setSingleIncrementDisplay(true);
	level->setValue(0.5);
	level->rotateWidgetDeg(-90);
	//level->m_colorActive.setRed();
	this->addChild(level);
}

void chai3d::cGauge::update(const double a_frameWidth,
	const double a_upBoundary,
	const double a_downBoundary,
	const double a_gaugesNumber)

{
	double frameHeight = a_upBoundary - a_downBoundary;
	double levelHeight = 20;//frameHeight * 0.045;
	double gaugeHeight = levelHeight + 60;
	cLabel* txt = (cLabel*) getChild(0);
	cLevel* lvl = (cLevel*) getChild(1);

	setLocalPos(0, a_upBoundary - id * frameHeight / a_gaugesNumber - gaugeHeight - 15);
	setSize(a_frameWidth, gaugeHeight);

	txt->setLocalPos(15, gaugeHeight - 20);
	lvl->setLocalPos(15, gaugeHeight - 40);

	//lev->setSize(levelHeight);
	//lev->setValue(0.5);
	//lev->m_colorActive.setRed();
}