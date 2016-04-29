/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <gale/context/cursor.h>

static const char* cursorDescriptionString[] = {
	"cursor::arrow",
	"cursor::leftArrow",
	"cursor::info",
	"cursor::destroy",
	"cursor::help",
	"cursor::cycle",
	"cursor::spray",
	"cursor::wait",
	"cursor::text",
	"cursor::crossHair",
	"cursor::slideUpDown",
	"cursor::slideLeftRight",
	"cursor::resizeUp",
	"cursor::resizeDown",
	"cursor::resizeLeft",
	"cursor::resizeRight",
	"cursor::cornerTopLeft",
	"cursor::cornerTopRight",
	"cursor::cornerButtomLeft",
	"cursor::cornerButtomRight",
	"cursor::none"
};

std::ostream& gale::context::operator <<(std::ostream& _os, enum gale::context::cursor _obj) {
	_os << cursorDescriptionString[int32_t(_obj)];
	return _os;
}


