/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/status.h>

static const char* statusDescriptionString[] = {
	"status_unknow",
	"status_down",
	"status_downRepeate",
	"status_move",
	"status_single",
	"status_double",
	"status_triple",
	"status_quad",
	"status_quinte",
	"status_up",
	"status_upRepeate",
	"status_upAfter",
	"status_enter",
	"status_leave",
	"status_abort",
	"status_transfert"
};

std::ostream& gale::key::operator <<(std::ostream& _os, const enum gale::key::status _obj) {
	_os << statusDescriptionString[_obj];
	return _os;
}

namespace etk {
	template<> std::string to_string<enum gale::key::status>(const enum gale::key::status& _obj) {
		return statusDescriptionString[_obj];
	}
}

