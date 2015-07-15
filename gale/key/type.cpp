/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/type.h>


static const char* typeDescriptionString[] = {
	"type_unknow",
	"type_mouse",
	"type_finger",
	"type_stylet"
};

std::ostream& gale::operator <<(std::ostream& _os, const enum gale::key::type _obj) {
	_os << typeDescriptionString[_obj];
	return _os;
}

