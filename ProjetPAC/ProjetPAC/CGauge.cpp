#include "CGauge.h"
#include "widgets/CLevel.h"
#include "widgets/CLabel.h"
#include "widgets/CBitmap.h"

chai3d::cGauge::cGauge(cFont* a_font, const std::string a_text, cBitmap* a_icon, const int a_id, cBitmap* a_icon2) : cPanel()

{
	this->m_id = a_id;
	this->m_value = 0.5;
	this->m_double = (a_icon2 != nullptr);
	
	// add a description text as a child
	cLabel* text = new cLabel(a_font);
	text->m_fontColor.setWhite();
	text->setText(a_text);
	this->addChild(text);
	
	// add a level object as a child
	cLevel* level = new cLevel();
	level->setRange(0.0, 1.0);
	level->setSize(20);
	//level->setSingleIncrementDisplay(true);
	level->setValue(0.5);
	level->rotateWidgetDeg(-90);
	//level->m_colorActive.setRed();
	this->addChild(level);

	// add a bitmap object as a child
	this->addChild(a_icon);

	if (m_double) {
		// add a second level object as a child
		level = new cLevel();
		level->setRange(0.0, 1.0);
		level->setSize(20);
		level->setValue(0.0);
		level->rotateWidgetDeg(90);
		this->addChild(level);

		// add a second bitmap object as a child
		this->addChild(a_icon2);
	}
}

void chai3d::cGauge::update(const double a_frameWidth,
	const double a_upBoundary,
	const double a_downBoundary,
	const int a_gaugesNumber)

{
	double frameHeight = a_upBoundary - a_downBoundary;
	double levelHeight = 20;//frameHeight * 0.045;
	double gaugeHeight = levelHeight + 60;
	int frameMarge = 15;
	int iconWidth = 40;
	cLabel* txt = (cLabel*) getChild(0);
	cLevel* lvl = (cLevel*) getChild(1);
	cBitmap* ico = (cBitmap*) getChild(2);

	setLocalPos(0, a_upBoundary - m_id * frameHeight / a_gaugesNumber - gaugeHeight - 15);
	setSize(a_frameWidth, gaugeHeight);

	txt->setLocalPos(15, gaugeHeight - 20);

	ico->setLocalPos(a_frameWidth - frameMarge - iconWidth, 30 - iconWidth / 2);

	if (!m_double) {
		int levelNumIncrements = (int)((a_frameWidth- iconWidth) / 2 - frameMarge);

		lvl->setLocalPos(frameMarge, gaugeHeight - 20 - levelHeight);
		lvl->setNumIncrements(levelNumIncrements);
		lvl->setValue(m_value);
	}
	else {
		cLevel* lvl_2 = (cLevel*) getChild(3);
		cBitmap* ico_2 = (cBitmap*) getChild(4);
		int levelNumIncrements = (int)((a_frameWidth / 2 - frameMarge - iconWidth) / 2);

		lvl->setLocalPos(a_frameWidth / 2 + 1, gaugeHeight - 20 - levelHeight);
		lvl->setNumIncrements(levelNumIncrements);
		lvl->setValue(m_value);

		lvl_2->setLocalPos(a_frameWidth / 2 - 1, gaugeHeight - 20 - levelHeight);
		lvl_2->setNumIncrements(levelNumIncrements);
		lvl_2->setValue(0.0);

		ico_2->setLocalPos(frameMarge, 30 - iconWidth / 2);
	}

	
	//lev->setSize(levelHeight);
	//lev->m_colorActive.setRed();
}