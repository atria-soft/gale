/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

namespace gale {
	enum class orientation {
		screenAuto = 0,
		screenLandscape,
		screenPortrait
	};
	etk::Stream& operator <<(etk::Stream& _os, enum gale::orientation _obj);
}
