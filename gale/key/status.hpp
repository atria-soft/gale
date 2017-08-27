/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <gale/debug.hpp>

namespace gale {
	namespace key {
		/**
		 * @brief Keybord event or joyestick event
		 */
		enum class status {
			unknow = 0,
			down, // availlable on Keyboard too
			downRepeate, // availlable on Keyboard too: the down event us in repeate cycle
			move,
			pressSingle,
			pressDouble,
			pressTriple,
			pressQuad,
			pressQuinte,
			up, // availlable on Keyboard too
			upRepeate, // availlable on Keyboard too: the up event us in repeate cycle
			upAfter, // mouse input & finger input this appear after the single event (depending on some case...)
			enter,
			leave,
			abort, // Appeare when an event is tranfert betwwen widgets (the widget which receive this has lost the events)
			transfert // Appeare when an event is tranfert betwwen widgets (the widget which receive this has receive the transfert of the event)
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		etk::Stream& operator <<(etk::Stream& _os, const enum gale::key::status _obj);
	}
}

