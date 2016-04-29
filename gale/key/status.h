/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <gale/debug.h>

namespace gale {
	namespace key {
		/**
		 * @brief Keybord event or joyestick event
		 */
		enum status {
			status_unknow = 0,
			status_down, // availlable on Keyboard too
			status_downRepeate, // availlable on Keyboard too: the down event us in repeate cycle
			status_move,
			status_single,
			status_double,
			status_triple,
			status_quad,
			status_quinte,
			status_up, // availlable on Keyboard too
			status_upRepeate, // availlable on Keyboard too: the up event us in repeate cycle
			status_upAfter, // mouse input & finger input this appear after the single event (depending on some case...)
			status_enter,
			status_leave,
			status_abort, // Appeare when an event is tranfert betwwen widgets (the widget which receive this has lost the events)
			status_transfert // Appeare when an event is tranfert betwwen widgets (the widget which receive this has receive the transfert of the event)
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		std::ostream& operator <<(std::ostream& _os, const enum gale::key::status _obj);
	}
}

