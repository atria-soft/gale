/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <gale/debug.hpp>

namespace gale {
	namespace context {
		enum class cursor {
			arrow, //  this is the normal arrow ...
			leftArrow,
			info,
			destroy,
			help,
			cycle,
			spray,
			wait,
			text,
			crossHair,
			slideUpDown,      //!< change the position (slide) vertical
			slideLeftRight,   //!< change the position (slide) horizontal
			resizeUp,
			resizeDown,
			resizeLeft,
			resizeRight,
			cornerTopLeft,
			cornerTopRight,
			cornerButtomLeft,
			cornerButtomRight,
			none
		};
		/**
		 * @brief Debug operator To display the curent element in a Human readable information
		 */
		etk::Stream& operator <<(etk::Stream& _os, enum gale::context::cursor _obj);
	}
}

