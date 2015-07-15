/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_CURSOR_H__
#define __GALE_CURSOR_H__

#include <gale/debug.h>

namespace gale {
	namespace context {
		enum cursorDisplay {
			cursorArrow, //  this is the normal arrow ...
			cursorLeftArrow,
			cursorInfo,
			cursorDestroy,
			cursorHelp,
			cursorCycle,
			cursorSpray,
			cursorWait,
			cursorText,
			cursorCrossHair,
			cursorSlideUpDown,      //!< change the position (slide) vertical
			cursorSlideLeftRight,   //!< change the position (slide) horizontal
			cursorResizeUp,
			cursorResizeDown,
			cursorResizeLeft,
			cursorResizeRight,
			cursorCornerTopLeft,
			cursorCornerTopRight,
			cursorCornerButtomLeft,
			cursorCornerButtomRight,
			cursorNone,
			// just for the count:
			cursorCount
		};
	};
	/**
	 * @brief Debug operator To display the curent element in a Human readable information
	 */
	std::ostream& operator <<(std::ostream& _os, const enum gale::context::cursorDisplay _obj);
};

#endif


