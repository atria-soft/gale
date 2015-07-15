/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_KEY_TYPE_H__
#define __GALE_KEY_TYPE_H__

#include <gale/debug.h>

namespace gale {
	namespace key {
		/**
		 * @brief type of input : Note that the keyboard is not prevent due to the fact that data is too different
		 */
		enum type {
			type_unknow = 0, //!< Unknow input Type
			type_mouse,      //!< Mouse type
			type_finger,     //!< Finger type
			type_stylet,     //!< Stylet type
		};
	};
	/**
	 * @brief Debug operator To display the curent element in a Human redeable information
	 */
	std::ostream& operator <<(std::ostream& _os, const enum gale::key::type _obj);
};



#endif


