/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <gale/orientation.hpp>

static const char* listValues[] = {
	"orientation::screenAuto",
	"orientation::screenLandscape",
	"orientation::screenPortrait"
};

std::ostream& gale::operator <<(std::ostream& _os, const enum gale::orientation _obj) {
	_os << listValues[int32_t(_obj)];
	return _os;
}


