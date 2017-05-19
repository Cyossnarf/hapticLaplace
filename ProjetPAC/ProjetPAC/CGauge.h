#ifndef CGaugeh
#define CGaugeh

#include "widgets/CPanel.h"
#include "graphics/CFont.h"
#include "widgets/CBitmap.h"

namespace chai3d {

	class cGauge : public cPanel
	{

	public:

		cGauge(cFontPtr a_font, const std::string a_text, cBitmap* a_icon, cBitmap* a_icon2, const int a_id, const bool a_double = false);

		void update(const double a_frameWidth,
			const double a_upBoundary,
			const double a_downBoundary,
			const int a_gaugesNumber);

		inline void setValue(double a_value) { m_value = a_value; }

	private:

		int m_id;
		double m_value;
		bool m_double;

	};


}

#endif