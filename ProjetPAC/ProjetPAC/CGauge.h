#ifndef CGaugeh
#define CGaugeh

#include "widgets/CPanel.h"
#include "graphics/CFont.h"

namespace chai3d {

	class cGauge : public cPanel
	{

	public:

		cGauge(cFont* a_font, const std::string a_text, const int a_id);

		void update(const double a_frameWidth,
			const double a_upBoundary,
			const double a_downBoundary,
			const double a_gaugesNumber);

	private:

		int id;

	};


}

#endif