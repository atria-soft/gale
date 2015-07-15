/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/status.h>

static const char* statusDescriptionString[gale::key::statusCount+1] = {
	"statusUnknow",
	"statusDown",
	"statusMove",
	"statusSingle",
	"statusDouble",
	"statusTriple",
	"statusQuad",
	"statusQuinte",
	"statusUp",
	"statusUpAfter",
	"statusEnter",
	"statusLeave",
	"statusAbort",
	"statusTransfert",
	"statusCount"
};

std::ostream& gale::key::operator <<(std::ostream& _os, const enum gale::key::status _obj) {
	if (_obj >= 0 && _obj <gale::key::statusCount) {
		_os << statusDescriptionString[_obj];
	} else {
		_os << "[ERROR]";
	}
	return _os;
}


