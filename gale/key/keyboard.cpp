/** @file
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/keyboard.h>
#include <etk/stdTools.h>

static const char* keyboardDescriptionString[] = {
	"keyboard_unknow",
	"keyboard_char",
	"keyboard_left",
	"keyboard_right",
	"keyboard_up",
	"keyboard_down",
	"keyboard_pageUp",
	"keyboard_pageDown",
	"keyboard_start",
	"keyboard_end",
	"keyboard_print",
	"keyboard_stopDefil",
	"keyboard_wait",
	"keyboard_insert",
	"keyboard_f1",
	"keyboard_f2",
	"keyboard_f3",
	"keyboard_f4",
	"keyboard_f5",
	"keyboard_f6",
	"keyboard_f7",
	"keyboard_f8",
	"keyboard_f9",
	"keyboard_f10",
	"keyboard_f11",
	"keyboard_f12",
	"keyboard_capLock",
	"keyboard_shiftLeft",
	"keyboard_shiftRight",
	"keyboard_ctrlLeft",
	"keyboard_ctrlRight",
	"keyboard_metaLeft",
	"keyboard_metaRight",
	"keyboard_alt",
	"keyboard_altGr",
	"keyboard_contextMenu",
	"keyboard_numLock",
	// harware section:
	"keyboard_volumeUp",
	"keyboard_volumeDown",
	"keyboard_menu",
	"keyboard_camera",
	"keyboard_home",
	"keyboard_power",
	"keyboard_back",
};

std::ostream& gale::key::operator <<(std::ostream& _os, const enum gale::key::keyboard _obj) {
	_os << keyboardDescriptionString[_obj];
	return _os;
}

namespace etk {
	template<> std::string to_string<enum gale::key::keyboard>(const enum gale::key::keyboard& _obj) {
		return keyboardDescriptionString[_obj];
	}
}

