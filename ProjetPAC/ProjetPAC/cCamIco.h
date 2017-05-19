#ifndef cCamIcoh
#define cCamIcoh

#include "widgets/CPanel.h"
#include "graphics/CFont.h"
#include "widgets/CBitmap.h"

namespace chai3d {

	class cCamIco : public cPanel
	{

	public:

		cCamIco(cFont* a_font, const std::string a_text, cBitmap* a_icon, cBitmap* a_icon2, const int a_id, const bool a_double = false);

		void update(const double a_frameWidth,
			const double a_upBoundary,
			const double a_downBoundary,
			const int a_gaugesNumber);

		void setValue(bool valeur);
		bool getValue();
		bool getBool();

	private:
		bool m_value;
		int m_id;
		bool m_bool;
		cBitmap* m_icon1;
		cBitmap* m_icon2;

	};


}

#endif