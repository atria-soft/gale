/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/keyboard.h>
#include <etk/stdTools.h>

static const char* keyboardDescriptionString[] = {
	"keyboard::unknow",
	"keyboard::character",
	"keyboard::left",
	"keyboard::right",
	"keyboard::up",
	"keyboard::down",
	"keyboard::pageUp",
	"keyboard::pageDown",
	"keyboard::start",
	"keyboard::end",
	"keyboard::print",
	"keyboard::stopDefil",
	"keyboard::wait",
	"keyboard::insert",
	"keyboard::f1",
	"keyboard::f2",
	"keyboard::f3",
	"keyboard::f4",
	"keyboard::f5",
	"keyboard::f6",
	"keyboard::f7",
	"keyboard::f8",
	"keyboard::f9",
	"keyboard::f10",
	"keyboard::f11",
	"keyboard::f12",
	"keyboard::capLock",
	"keyboard::shiftLeft",
	"keyboard::shiftRight",
	"keyboard::ctrlLeft",
	"keyboard::ctrlRight",
	"keyboard::metaLeft",
	"keyboard::metaRight",
	"keyboard::alt",
	"keyboard::altGr",
	"keyboard::contextMenu",
	"keyboard::numLock",
	// harware section:
	"keyboard::volumeUp",
	"keyboard::volumeDown",
	"keyboard::menu",
	"keyboard::camera",
	"keyboard::home",
	"keyboard::power",
	"keyboard::back",
};

std::ostream& gale::key::operator <<(std::ostream& _os, const enum gale::key::keyboard _obj) {
	_os << keyboardDescriptionString[int32_t(_obj)];
	return _os;
}

namespace etk {
	template<> std::string to_string<enum gale::key::keyboard>(const enum gale::key::keyboard& _obj) {
		return keyboardDescriptionString[int32_t(_obj)];
	}
}

