/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/widget/Widget.h>

#undef __class__
#define __class__ "event::Entry"

std::ostream& gale::event::operator <<(std::ostream& _os, const gale::event::Entry& _obj) {
	_os << "{type=" << _obj.getType();
	_os << " status=" << _obj.getStatus();
	if (_obj.getType() == gale::key::keyboardChar) {
		_os << " char=" << _obj.getChar();
	}
	_os << "}";
	return _os;
}

std::ostream& gale::event::operator <<(std::ostream& _os, const gale::event::EntrySystem& _obj) {
	_os << _obj.m_event;
	return _os;
}
