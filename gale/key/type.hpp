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
		 * @brief type of input : Note that the keyboard is not prevent due to the fact that data is too different
		 */
		enum class type {
			unknow = 0, //!< Unknow input Type
			mouse,      //!< Mouse type
			finger,     //!< Finger type
			stylet,     //!< Stylet type
		};
		/**
		 * @brief Debug operator To display the curent element in a Human redeable information
		 */
		std::ostream& operator <<(std::ostream& _os, enum gale::key::type _obj);
	}
}

