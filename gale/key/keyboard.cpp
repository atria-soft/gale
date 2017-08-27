/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/key/keyboard.hpp>
#include <etk/stdTools.hpp>

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
	"keyboard::altLeft",
	"keyboard::altRight",
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

etk::Stream& gale::key::operator <<(etk::Stream& _os, const enum gale::key::keyboard _obj) {
	_os << keyboardDescriptionString[int32_t(_obj)];
	return _os;
}

namespace etk {
	template<> etk::String toString<enum gale::key::keyboard>(const enum gale::key::keyboard& _obj) {
		return keyboardDescriptionString[int32_t(_obj)];
	}
	
	template <> bool from_string<enum gale::key::keyboard>(enum gale::key::keyboard& _variableRet, const etk::String& _value) {
		for (size_t iii=0; iii< sizeof(keyboardDescriptionString); ++iii) {
			if (keyboardDescriptionString[iii] == _value) {
				_variableRet = (enum gale::key::keyboard)iii;
				return true;
			}
		}
		return false;
	}
}

