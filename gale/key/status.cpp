/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/key/status.hpp>
#include <etk/stdTools.hpp>

static const char* statusDescriptionString[] = {
	"status::unknow",
	"status::down",
	"status::downRepeate",
	"status::move",
	"status::pressSingle",
	"status::pressDouble",
	"status::pressTriple",
	"status::pressQuad",
	"status::pressQuinte",
	"status::up",
	"status::upRepeate",
	"status::upAfter",
	"status::enter",
	"status::leave",
	"status::abort",
	"status::transfert"
};

etk::Stream& gale::key::operator <<(etk::Stream& _os, const enum gale::key::status _obj) {
	_os << statusDescriptionString[int32_t(_obj)];
	return _os;
}

namespace etk {
	template<> etk::String toString<enum gale::key::status>(const enum gale::key::status& _obj) {
		return statusDescriptionString[int32_t(_obj)];
	}
	
	template <> bool from_string<enum gale::key::status>(enum gale::key::status& _variableRet, const etk::String& _value) {
		for (size_t iii=0; iii< sizeof(statusDescriptionString); ++iii) {
			if (statusDescriptionString[iii] == _value) {
				_variableRet = (enum gale::key::status)iii;
				return true;
			}
		}
		return false;
	}
}
