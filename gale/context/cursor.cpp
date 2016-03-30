/** @file
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/context/cursor.h>

static const char* cursorDescriptionString[] = {
	"cursor_arrow",
	"cursor_leftArrow",
	"cursor_info",
	"cursor_destroy",
	"cursor_help",
	"cursor_cycle",
	"cursor_spray",
	"cursor_wait",
	"cursor_text",
	"cursor_crossHair",
	"cursor_slideUpDown",
	"cursor_slideLeftRight",
	"cursor_resizeUp",
	"cursor_resizeDown",
	"cursor_resizeLeft",
	"cursor_resizeRight",
	"cursor_cornerTopLeft",
	"cursor_cornerTopRight",
	"cursor_cornerButtomLeft",
	"cursor_cornerButtomRight",
	"cursor_none"
};

std::ostream& gale::operator <<(std::ostream& _os, const enum gale::context::cursor _obj) {
	_os << cursorDescriptionString[_obj];
	return _os;
}


