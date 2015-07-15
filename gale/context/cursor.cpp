/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/context/cursor.h>

static const char* cursorDescriptionString[gale::context::cursorCount+1] = {
	"cursorArrow",
	"cursorLeftArrow",
	"cursorInfo",
	"cursorDestroy",
	"cursorHelp",
	"cursorCycle",
	"cursorSpray",
	"cursorWait",
	"cursorText",
	"cursorCrossHair",
	"cursorSlideUpDown",
	"cursorSlideLeftRight",
	"cursorResizeUp",
	"cursorResizeDown",
	"cursorResizeLeft",
	"cursorResizeRight",
	"cursorCornerTopLeft",
	"cursorCornerTopRight",
	"cursorCornerButtomLeft",
	"cursorCornerButtomRight",
	"cursorNone",
	"cursorCount"
};

std::ostream& gale::operator <<(std::ostream& _os, const enum gale::context::cursorDisplay _obj) {
	if (_obj >= 0 && _obj <gale::context::cursorCount) {
		_os << cursorDescriptionString[_obj];
	} else {
		_os << "[ERROR]";
	}
	return _os;
}


