#ifndef CGaugeh
#define CGaugeh

#include "widgets/CPanel.h"
#include "graphics/CFont.h"
#include "widgets/CBitmap.h"

namespace chai3d {

	class cGauge : public cPanel
	{

	public:

		cGauge(cFont* a_font, const std::string a_text, cBitmap* a_icon, const int a_id, cBitmap* a_icon2 = nullptr);

		void update(const double a_frameWidth,
			const double a_upBoundary,
			const double a_downBoundary,
			const int a_gaugesNumber);

	private:

		int m_id;
		double m_value;
		bool m_double;

	};


}

#endif