/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_KEY_STATUS_H__
#define __GALE_KEY_STATUS_H__

#include <gale/debug.h>

namespace gale {
	namespace key {
		/**
		 * @brief Keybord event or joyestick event
		 */
		enum status {
			statusUnknow = 0,
			statusDown, // availlable on Keyboard too
			statusMove,
			statusSingle,
			statusDouble,
			statusTriple,
			statusQuad,
			statusQuinte,
			statusUp, // availlable on Keyboard too
			statusUpAfter, // mouse input & finger input this appear after the single event (depending on some case...)
			statusEnter,
			statusLeave,
			statusAbort, // Appeare when an event is tranfert betwwen widgets (the widget which receive this has lost the events)
			statusTransfert, // Appeare when an event is tranfert betwwen widgets (the widget which receive this has receive the transfert of the event)
			statusCount, // number max of imput possible
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		std::ostream& operator <<(std::ostream& _os, const enum gale::key::status _obj);
	};
};



#endif


