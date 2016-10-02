/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <gale/debug.hpp>

namespace gale {
	namespace key {
		/**
		 * @brief Keybord event or joystick event
		 * @warning If you modify Id of these element check the java interface file of constant : GaleConstant.java
		 */
		enum class keyboard {
			unknow = 0,    //!< Unknown keyboard key
			character,     //!< Char input is arrived ...
			left,          //!< Left key <--
			right,         //!< Right key -->
			up,            //!< Up key ^
			down,          //!< Down key \/
			pageUp,        //!< Page Up key
			pageDown,      //!< page down key
			start,         //!< Start key
			end,           //!< End key
			print,         //!< print screen key.
			stopDefil,     //!< Stop display key.
			wait,          //!< Wait key.
			insert,        //!< insert key.
			f1,            //!< F1 key.
			f2,            //!< F2 key.
			f3,            //!< F3 key.
			f4,            //!< F4 key.
			f5,            //!< F5 key.
			f6,            //!< F6 key.
			f7,            //!< F7 key.
			f8,            //!< F8 key.
			f9,            //!< F9 key.
			f10,           //!< F10 key.
			f11,           //!< F11 key.
			f12,           //!< F12 key.
			capLock,       //!< Capital Letter Lock key.
			shiftLeft,     //!< Shift left key.
			shiftRight,    //!< Shift right key.
			ctrlLeft,      //!< Control left key.
			ctrlRight,     //!< Control right key.
			metaLeft,      //!< Meta left key (apple key or windows key).
			metaRight,     //!< Meta right key (apple key or windows key).
			alt,           //!< Alt key.
			altGr,         //!< Alt ground key.
			contextMenu,   //!< Contextual menu key.
			numLock,       //!< Numerical Lock key
			// harware section:
			volumeUp,      //!< Hardware volume UP key
			volumeDown,    //!< Hardware volume DOWN key
			menu,          //!< Hardware Menu key
			camera,        //!< Hardware Camera key
			home,          //!< Hardware Home key
			power,         //!< Hardware Power key
			back           //!< Hardware Back key
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		std::ostream& operator <<(std::ostream& _os, const enum gale::key::keyboard _obj);
	}
}

