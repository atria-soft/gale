/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_KEY_KEYBOARD_H__
#define __GALE_KEY_KEYBOARD_H__

#include <gale/debug.h>

namespace gale {
	namespace key {
		/**
		 * @brief Keybord event or joystick event
		 * @warning If you modify Id of these element check the java interface file of constant : GaleConstant.java
		 */
		enum keyboard {
			keyboard_unknow = 0,    //!< Unknown keyboard key
			keyboard_char,          //!< Char input is arrived ...
			keyboard_left,          //!< Left key <--
			keyboard_right,         //!< Right key -->
			keyboard_up,            //!< Up key ^
			keyboard_down,          //!< Down key \/
			keyboard_pageUp,        //!< Page Up key
			keyboard_pageDown,      //!< page down key
			keyboard_start,         //!< Start key
			keyboard_end,           //!< End key
			keyboard_print,         //!< print screen key.
			keyboard_stopDefil,     //!< Stop display key.
			keyboard_wait,          //!< Wait key.
			keyboard_insert,        //!< insert key.
			keyboard_f1,            //!< F1 key.
			keyboard_f2,            //!< F2 key.
			keyboard_f3,            //!< F3 key.
			keyboard_f4,            //!< F4 key.
			keyboard_f5,            //!< F5 key.
			keyboard_f6,            //!< F6 key.
			keyboard_f7,            //!< F7 key.
			keyboard_f8,            //!< F8 key.
			keyboard_f9,            //!< F9 key.
			keyboard_f10,           //!< F10 key.
			keyboard_f11,           //!< F11 key.
			keyboard_f12,           //!< F12 key.
			keyboard_capLock,       //!< Capital Letter Lock key.
			keyboard_shiftLeft,     //!< Shift left key.
			keyboard_shiftRight,    //!< Shift right key.
			keyboard_ctrlLeft,      //!< Control left key.
			keyboard_ctrlRight,     //!< Control right key.
			keyboard_metaLeft,      //!< Meta left key (apple key or windows key).
			keyboard_metaRight,     //!< Meta right key (apple key or windows key).
			keyboard_alt,           //!< Alt key.
			keyboard_altGr,         //!< Alt ground key.
			keyboard_contextMenu,   //!< Contextual menu key.
			keyboard_numLock,       //!< Numerical Lock key
			// harware section:
			keyboard_volumeUp,      //!< Hardware volume UP key
			keyboard_volumeDown,    //!< Hardware volume DOWN key
			keyboard_menu,          //!< Hardware Menu key
			keyboard_camera,        //!< Hardware Camera key
			keyboard_home,          //!< Hardware Home key
			keyboard_power,         //!< Hardware Power key
			keyboard_back           //!< Hardware Back key
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		std::ostream& operator <<(std::ostream& _os, const enum gale::key::keyboard _obj);
	};
};



#endif


