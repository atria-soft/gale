/** @file
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <gale/debug.h>

namespace gale {
	namespace context {
		enum cursor {
			cursor_arrow, //  this is the normal arrow ...
			cursor_leftArrow,
			cursor_info,
			cursor_destroy,
			cursor_help,
			cursor_cycle,
			cursor_spray,
			cursor_wait,
			cursor_text,
			cursor_crossHair,
			cursor_slideUpDown,      //!< change the position (slide) vertical
			cursor_slideLeftRight,   //!< change the position (slide) horizontal
			cursor_resizeUp,
			cursor_resizeDown,
			cursor_resizeLeft,
			cursor_resizeRight,
			cursor_cornerTopLeft,
			cursor_cornerTopRight,
			cursor_cornerButtomLeft,
			cursor_cornerButtomRight,
			cursor_none
		};
	};
	/**
	 * @brief Debug operator To display the curent element in a Human readable information
	 */
	std::ostream& operator <<(std::ostream& _os, const enum gale::context::cursor _obj);
}

