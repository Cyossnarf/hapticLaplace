#ifndef CGaugeh
#define CGaugeh

#include "widgets/CPanel.h"
#include "graphics/CFont.h"
#include "widgets/CBitmap.h"

namespace chai3d {

	class cGauge : public cPanel
	{

	public:

		cGauge(cFontPtr a_font,
			const std::string a_text,
			cBitmap* a_icon,
			cBitmap* a_icon2,
			cColorf &a_col,
			const double a_minValue,
			const double a_maxValue,
			const double a_step,
			const int a_id,
			const bool a_double = false,
			const bool a_toggle = false);

		void update(const double a_frameWidth,
			const double a_upBoundary,
			const double a_downBoundary,
			const int a_gaugesNumber);

		void setValue(const double a_value);
		inline double getValue() const { return m_value; }
		void incr();
		void decr();

	private:

		int m_id;
		double m_value;
		double m_maxValue;
		double m_minValue;
		double m_step;
		bool m_double;
		bool m_toggle;
		cColorf m_col;

	};


}

#endif