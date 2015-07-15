/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/keyboard.h>

static const char* keyboardDescriptionString[gale::key::keyboardCount+1] = {
	"keyboardUnknow",
	"keyboardChar",
	"keyboardLeft",
	"keyboardRight",
	"keyboardUp",
	"keyboardDown",
	"keyboardPageUp",
	"keyboardPageDown",
	"keyboardStart",
	"keyboardEnd",
	"keyboardPrint",
	"keyboardStopDefil",
	"keyboardWait",
	"keyboardInsert",
	"keyboardF1",
	"keyboardF2",
	"keyboardF3",
	"keyboardF4",
	"keyboardF5",
	"keyboardF6",
	"keyboardF7",
	"keyboardF8",
	"keyboardF9",
	"keyboardF10",
	"keyboardF11",
	"keyboardF12",
	"keyboardCapLock",
	"keyboardShiftLeft",
	"keyboardShiftRight",
	"keyboardCtrlLeft",
	"keyboardCtrlRight",
	"keyboardMetaLeft",
	"keyboardMetaRight",
	"keyboardAlt",
	"keyboardAltGr",
	"keyboardContextMenu",
	"keyboardNumLock",
	"keyboardCount"
};

std::ostream& gale::key::operator <<(std::ostream& _os, const enum gale::key::keyboard _obj) {
	if (_obj >= 0 && _obj <gale::key::keyboardCount) {
		_os << keyboardDescriptionString[_obj];
	} else {
		_os << "[ERROR]";
	}
	return _os;
}

static const char* keyboardSystemDescriptionString[gale::key::keyboardSystemCount+1] = {
	"keyboardSystemUnknow",
	"keyboardSystemVolumeUp",
	"keyboardSystemVolumeDown",
	"keyboardSystemMenu",
	"keyboardSystemCamera",
	"keyboardSystemHome",
	"keyboardSystemPower",
	"keyboardSystemBack",
	"keyboardSystemCount"
};

std::ostream& gale::key::operator <<(std::ostream& _os, const enum gale::key::keyboardSystem _obj) {
	if (_obj >= 0 && _obj <gale::key::keyboardSystemCount) {
		_os << keyboardSystemDescriptionString[_obj];
	} else {
		_os << "[ERROR]";
	}
	return _os;
}