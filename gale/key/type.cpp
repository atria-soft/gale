/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/key/type.hpp>
#include <etk/stdTools.hpp>


static const char* typeDescriptionString[] = {
	"type::unknow",
	"type::mouse",
	"type::finger",
	"type::stylet"
};

etk::Stream& gale::key::operator <<(etk::Stream& _os, const enum gale::key::type _obj) {
	_os << typeDescriptionString[int32_t(_obj)];
	return _os;
}

namespace etk {
	template<> etk::String toString<enum gale::key::type>(const enum gale::key::type& _obj) {
		return typeDescriptionString[int32_t(_obj)];
	}
	
	template <> bool from_string<enum gale::key::type>(enum gale::key::type& _variableRet, const etk::String& _value) {
		for (size_t iii=0; iii< sizeof(typeDescriptionString); ++iii) {
			if (typeDescriptionString[iii] == _value) {
				_variableRet = (enum gale::key::type)iii;
				return true;
			}
		}
		return false;
	}
}
