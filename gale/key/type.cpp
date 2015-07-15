/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/type.h>


static const char* typeDescriptionString[gale::key::typeCount+1] = {
	"typeUnknow",
	"typeMouse",
	"typeFinger",
	"typeStylet",
	"typeCount"
};

std::ostream& gale::operator <<(std::ostream& _os, const enum gale::key::type _obj) {
	if (_obj >= 0 && _obj < gale::key::typeCount) {
		_os << typeDescriptionString[_obj];
	} else {
		_os << "[ERROR]";
	}
	return _os;
}

