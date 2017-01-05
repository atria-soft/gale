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

std::ostream& gale::key::operator <<(std::ostream& _os, const enum gale::key::type _obj) {
	_os << typeDescriptionString[int32_t(_obj)];
	return _os;
}

namespace etk {
	template<> std::string to_string<enum gale::key::type>(const enum gale::key::type& _obj) {
		return typeDescriptionString[int32_t(_obj)];
	}
}
