#ifndef CConsolh
#define CConsolh

#include "widgets/CPanel.h"
#include "graphics/CFont.h"

namespace chai3d {

	class cConsol : public cPanel
	{

	public:

		cConsol(double a_width,
			double a_height,
			const cColorf &a_txtCol,
			cFontPtr a_fontLbl,
			cFontPtr a_fontVal,
			const std::string a_textLbl,
			const bool a_leftAlignt = true);

		void update(const std::string a_textVal);

	private:

		double m_width;
		bool m_leftAligned;

	};


}

#endif