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
			keyboardUnknow = 0,    //!< Unknown keyboard key
			keyboardChar,          //!< Char input is arrived ...
			keyboardLeft,          //!< Left key <--
			keyboardRight,         //!< Right key -->
			keyboardUp,            //!< Up key ^
			keyboardDown,          //!< Down key \/
			keyboardPageUp,        //!< Page Up key
			keyboardPageDown,      //!< page down key
			keyboardStart,         //!< Start key
			keyboardEnd,           //!< End key
			keyboardPrint,         //!< print screen key.
			keyboardStopDefil,     //!< Stop display key.
			keyboardWait,          //!< Wait key.
			keyboardInsert,        //!< insert key.
			keyboardF1,            //!< F1 key.
			keyboardF2,            //!< F2 key.
			keyboardF3,            //!< F3 key.
			keyboardF4,            //!< F4 key.
			keyboardF5,            //!< F5 key.
			keyboardF6,            //!< F6 key.
			keyboardF7,            //!< F7 key.
			keyboardF8,            //!< F8 key.
			keyboardF9,            //!< F9 key.
			keyboardF10,           //!< F10 key.
			keyboardF11,           //!< F11 key.
			keyboardF12,           //!< F12 key.
			keyboardCapLock,       //!< Capital Letter Lock key.
			keyboardShiftLeft,     //!< Shift left key.
			keyboardShiftRight,    //!< Shift right key.
			keyboardCtrlLeft,      //!< Control left key.
			keyboardCtrlRight,     //!< Control right key.
			keyboardMetaLeft,      //!< Meta left key (apple key or windows key).
			keyboardMetaRight,     //!< Meta right key (apple key or windows key).
			keyboardAlt,           //!< Alt key.
			keyboardAltGr,         //!< Alt ground key.
			keyboardContextMenu,   //!< Contextual menu key.
			keyboardNumLock,       //!< Numerical Lock key.
			keyboardCount          //!< number of posible key
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		std::ostream& operator <<(std::ostream& _os, const enum gale::key::keyboard _obj);
		
		enum keyboardSystem {
			keyboardSystemUnknow = 0, //!< Unknown keyboard system key
			keyboardSystemVolumeUp,   //!< Hardware volume UP key
			keyboardSystemVolumeDown, //!< Hardware volume DOWN key
			keyboardSystemMenu,       //!< Hardware Menu key
			keyboardSystemCamera,     //!< Hardware Camera key
			keyboardSystemHome,       //!< Hardware Home key
			keyboardSystemPower,      //!< Hardware Power key
			keyboardSystemBack,       //!< Hardware Back key
			keyboardSystemCount       //!< number of posible System key
		};
		std::ostream& operator <<(std::ostream& _os, const enum gale::key::keyboardSystem _obj);
	};
};



#endif


