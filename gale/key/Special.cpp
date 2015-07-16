/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/key/Special.h>

#define GALE_FLAG_KEY_CAPS_LOCK 0x00000001
#define GALE_FLAG_KEY_SHIFT     0x00000002
#define GALE_FLAG_KEY_CTRL      0x00000004
#define GALE_FLAG_KEY_META      0x00000008
#define GALE_FLAG_KEY_ALT       0x00000010
#define GALE_FLAG_KEY_ALTGR     0x00000020
#define GALE_FLAG_KEY_NUM_LOCK  0x00000040
#define GALE_FLAG_KEY_INSERT    0x00000080

// TODO : Update to support the Left and right of some key ...

gale::key::Special::Special() :
  m_value(0) {
	
}
void gale::key::Special::update(enum gale::key::keyboard _move, bool _isDown) {
	switch (_move) {
		case keyboard_insert:
			setInsert(_isDown);
			break;
		case keyboard_capLock:
			setCapsLock(_isDown);
			break;
		case keyboard_shiftLeft:
		case keyboard_shiftRight:
			setShift(_isDown);
			break;
		case keyboard_ctrlLeft:
		case keyboard_ctrlRight:
			setCtrl(_isDown);
			break;
		case keyboard_metaLeft:
		case keyboard_metaRight:
			setMeta(_isDown);
			break;
		case keyboard_alt:
			setAlt(_isDown);
			break;
		case keyboard_altGr:
			setAltGr(_isDown);
			break;
		case keyboard_numLock:
			setNumLock(_isDown);
			break;
		default:
			break;
	}
}

bool gale::key::Special::getCapsLock() const {
	if ((m_value & GALE_FLAG_KEY_CAPS_LOCK) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setCapsLock(bool _value) {
	if ((m_value & GALE_FLAG_KEY_CAPS_LOCK) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_CAPS_LOCK;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_CAPS_LOCK;
		}
	}
}

bool gale::key::Special::getShift() const {
	if ((m_value & GALE_FLAG_KEY_SHIFT) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setShift(bool _value) {
	if ((m_value & GALE_FLAG_KEY_SHIFT) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_SHIFT;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_SHIFT;
		}
	}
}

bool gale::key::Special::getCtrl() const {
	if ((m_value & GALE_FLAG_KEY_CTRL) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setCtrl(bool _value) {
	if ((m_value & GALE_FLAG_KEY_CTRL) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_CTRL;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_CTRL;
		}
	}
}

bool gale::key::Special::getMeta() const {
	if ((m_value & GALE_FLAG_KEY_META) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setMeta(bool _value) {
	if ((m_value & GALE_FLAG_KEY_META) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_META;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_META;
		}
	}
}

bool gale::key::Special::getAlt() const {
	if ((m_value & GALE_FLAG_KEY_ALT) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setAlt(bool _value) {
	if ((m_value & GALE_FLAG_KEY_ALT) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_ALT;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_ALT;
		}
	}
}

bool gale::key::Special::getAltGr() const {
	if ((m_value & GALE_FLAG_KEY_ALTGR) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setAltGr(bool _value) {
	if ((m_value & GALE_FLAG_KEY_ALTGR) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_ALTGR;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_ALTGR;
		}
	}
}

bool gale::key::Special::getNumLock() const {
	if ((m_value & GALE_FLAG_KEY_NUM_LOCK) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setNumLock(bool _value) {
	if ((m_value & GALE_FLAG_KEY_NUM_LOCK) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_NUM_LOCK;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_NUM_LOCK;
		}
	}
}

bool gale::key::Special::getInsert() const {
	if ((m_value & GALE_FLAG_KEY_INSERT) != 0) {
		return true;
	}
	return false;
}
void gale::key::Special::setInsert(bool _value) {
	if ((m_value & GALE_FLAG_KEY_INSERT) != 0) {
		if (_value == false) {
			m_value -= GALE_FLAG_KEY_INSERT;
		}
	} else {
		if (_value == true) {
			m_value += GALE_FLAG_KEY_INSERT;
		}
	}
}

std::ostream& gale::key::operator <<(std::ostream& _os, const gale::key::Special _obj) {
	_os << " capLock=" << _obj.getCapsLock();
	_os << " shift=" << _obj.getShift();
	_os << " ctrl=" << _obj.getCtrl();
	_os << " meta=" << _obj.getMeta();
	_os << " alt=" << _obj.getAlt();
	_os << " altGr=" << _obj.getAltGr();
	_os << " verNum=" << _obj.getNumLock();
	_os << " insert=" << _obj.getInsert();
	return _os;
}
