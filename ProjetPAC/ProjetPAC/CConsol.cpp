#include "CConsol.h"
#include "widgets/CLabel.h"

chai3d::cConsol::cConsol(double a_width,
	double a_height,
	const cColorf &a_colBg,
	cFontPtr a_fontLbl,
	cFontPtr a_fontVal,
	const std::string a_textLbl,
	const bool a_leftAligned) : cPanel()

{
	this->m_width = a_width;
	this->m_leftAligned = a_leftAligned;

	// set dimensions of the frame
	this->setSize(a_width, a_height);

	// add a description text as a child
	cLabel* textLbl = new cLabel(a_fontLbl);
	textLbl->m_fontColor.setRed();
	textLbl->setText(a_textLbl);
	this->addChild(textLbl);

	// add a value text as a child
	cLabel* textVal = new cLabel(a_fontVal);
	textVal->m_fontColor.setRed();
	this->addChild(textVal);

	if (m_leftAligned) {
		textLbl->setLocalPos(5, 34);
	}
	else {
		textLbl->setLocalPos(a_width - 5 - textLbl->getWidth(), 34);
	}
}

void chai3d::cConsol::update(const std::string a_textVal)

{
	cLabel* textVal = (cLabel*)getChild(1);
	
	// set a new value text
	textVal->setText(a_textVal);

	// set position of the new value text
	if (m_leftAligned) {
		textVal->setLocalPos(5, -1);
	}
	else {
		textVal->setLocalPos(m_width - 5 - textVal->getWidth(), -1);
	}
}