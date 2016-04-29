/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

namespace gale {
	enum class orientation {
		screenAuto = 0,
		screenLandscape,
		screenPortrait
	};
	std::ostream& operator <<(std::ostream& _os, enum gale::orientation _obj);
}
