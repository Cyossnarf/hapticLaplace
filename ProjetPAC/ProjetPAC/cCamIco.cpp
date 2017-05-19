#include "cCamIco.h"
#include "widgets/CLevel.h"
#include "widgets/CLabel.h"
#include "widgets/CBitmap.h"

chai3d::cCamIco::cCamIco(cFont* a_font, const std::string a_text, cBitmap* a_icon, cBitmap* a_icon2, const int a_id, const bool a_bool) : cPanel()

{
	// position of the gauge on screen
	this->m_id = a_id;

	// which icon to show
	this->m_bool = a_bool;
	this->m_value = false;

	//store the icons
	this->m_icon1 = a_icon;
	this->m_icon2 = a_icon2;

	// add a description text as a child
	cLabel* text = new cLabel(a_font->create());
	text->m_fontColor.setWhite();
	text->setText(a_text);
	this->addChild(text);

	

	// add 2 bitmap objects as children
	this->addChild(m_icon1);
	this->addChild(m_icon2);
	
	
}

void chai3d::cCamIco::setValue(bool valeur){
	this->m_bool = true;
	this->m_value = valeur;
}

bool chai3d::cCamIco::getValue(){
	return(this->m_value);
}

bool chai3d::cCamIco::getBool(){
	return(this->m_bool);
}


void chai3d::cCamIco::update(const double a_frameWidth,
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
	cBitmap* icon2 = (cBitmap*)getChild(2);
	if (this->m_bool){
		if (ico->getEnabled()){
			ico->setEnabled(false);
			icon2->setEnabled(true);
		}
		else{
			ico->setEnabled(true);
			icon2->setEnabled(false);
		}
		m_bool = false;
	}

	// set size and position of the gauge
	setLocalPos(0, a_upBoundary - m_id * frameHeight / a_gaugesNumber - gaugeHeight - 15);
	setSize(a_frameWidth, gaugeHeight);

	// set position of the text child
	txt->setLocalPos(15, gaugeHeight - 20);

	// set positions of the icons children
	ico->setLocalPos((a_frameWidth - iconWidth) / 2, 30 - iconWidth / 2);
	icon2->setLocalPos((a_frameWidth - iconWidth) / 2, 30 - iconWidth / 2);
}