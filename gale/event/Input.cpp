/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/widget/Widget.h>

#undef __class__
#define __class__ "event::Input"

std::ostream& gale::event::operator <<(std::ostream& _os, const gale::event::Input& _obj) {
	_os << "{type=" << _obj.getType();
	_os << " status=" << _obj.getStatus();
	_os << " id=" << etk::to_string(_obj.getId());
	_os << " pos=" << _obj.getPos();
	_os << "}";
	return _os;
}

std::ostream& gale::event::operator <<(std::ostream& _os, const gale::event::InputSystem& _obj) {
	_os << _obj.m_event;
	return _os;
}
