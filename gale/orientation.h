/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

namespace gale {
	enum orientation {
		orientation_screenAuto = 0,
		orientation_screenLandscape,
		orientation_screenPortrait
	};
	std::ostream& operator <<(std::ostream& _os, enum gale::orientation _obj);
}
