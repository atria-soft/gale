/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/key/Special.hpp>
#include <etk/stdTools.hpp>

#define GALE_FLAG_KEY_CAPS_LOCK   0x00000001
#define GALE_FLAG_KEY_SHIFT       0x00000030
#define GALE_FLAG_KEY_SHIFT_LEFT  0x00000010
#define GALE_FLAG_KEY_SHIFT_RIGHT 0x00000020
#define GALE_FLAG_KEY_CTRL        0x00000300
#define GALE_FLAG_KEY_CTRL_LEFT   0x00000100
#define GALE_FLAG_KEY_CTRL_RIGHT  0x00000200
#define GALE_FLAG_KEY_META        0x00003000
#define GALE_FLAG_KEY_META_LEFT   0x00001000
#define GALE_FLAG_KEY_META_RIGHT  0x00002000
#define GALE_FLAG_KEY_ALT         0x00030000
#define GALE_FLAG_KEY_ALT_LEFT    0x00010000
#define GALE_FLAG_KEY_ALT_RIGHT   0x00020000
#define GALE_FLAG_KEY_NUM_LOCK    0x00000002
#define GALE_FLAG_KEY_INSERT      0x00000003

// TODO : Update to support the Left and right of some key ...

gale::key::Special::Special() :
  m_value(0) {
	
}
void gale::key::Special::update(enum gale::key::keyboard _move, bool _isDown) {
	switch (_move) {
		case keyboard::insert:
			setInsert(_isDown);
			break;
		case keyboard::capLock:
			setCapsLock(_isDown);
			break;
		case keyboard::shiftLeft:
			setShiftLeft(_isDown);
			break;
		case keyboard::shiftRight:
			setShiftRight(_isDown);
			break;
		case keyboard::ctrlLeft:
			setCtrlLeft(_isDown);
			break;
		case keyboard::ctrlRight:
			setCtrlRight(_isDown);
			break;
		case keyboard::metaLeft:
			setMetaLeft(_isDown);
			break;
		case keyboard::metaRight:
			setMetaRight(_isDown);
			break;
		case keyboard::altLeft:
			setAltLeft(_isDown);
			break;
		case keyboard::altRight:
			setAltRight(_isDown);
			break;
		case keyboard::numLock:
			setNumLock(_isDown);
			break;
		default:
			break;
	}
}

bool gale::key::Special::get(enum gale::key::keyboard _move) {
	switch (_move) {
		case keyboard::insert:
			return getInsert();
		case keyboard::capLock:
			return getCapsLock();
		case keyboard::shiftLeft:
			return getShiftLeft();
		case keyboard::shiftRight:
			return getShiftRight();
		case keyboard::ctrlLeft:
			return getCtrlLeft();
		case keyboard::ctrlRight:
			return getCtrlRight();
		case keyboard::metaLeft:
			return getMetaLeft();
		case keyboard::metaRight:
			return getMetaRight();
		case keyboard::altLeft:
			return getAltLeft();
		case keyboard::altRight:
			return getAltRight();
		case keyboard::numLock:
			return getNumLock();
		default:
			break;
	}
	return false;
}


void gale::key::Special::setFlag(uint32_t _flag, bool _isDown) {
	if ((m_value & _flag) != 0) {
		if (_isDown == false) {
			m_value -= _flag;
		}
	} else {
		if (_isDown == true) {
			m_value += _flag;
		}
	}
}
bool gale::key::Special::getFlag(uint32_t _flag) const {
	if ((m_value & _flag) != 0) {
		return true;
	}
	return false;
};

bool gale::key::Special::getCapsLock() const {
	return getFlag(GALE_FLAG_KEY_CAPS_LOCK);
}
void gale::key::Special::setCapsLock(bool _value) {
	setFlag(GALE_FLAG_KEY_CAPS_LOCK, _value);
}

bool gale::key::Special::getShift() const {
	return getFlag(GALE_FLAG_KEY_SHIFT);
}
bool gale::key::Special::getShiftLeft() const {
	return getFlag(GALE_FLAG_KEY_SHIFT_LEFT);
}
bool gale::key::Special::getShiftRight() const {
	return getFlag(GALE_FLAG_KEY_SHIFT_RIGHT);
}
void gale::key::Special::setShiftLeft(bool _value) {
	setFlag(GALE_FLAG_KEY_SHIFT_LEFT, _value);
}
void gale::key::Special::setShiftRight(bool _value) {
	setFlag(GALE_FLAG_KEY_SHIFT_RIGHT, _value);
}

bool gale::key::Special::getCtrl() const {
	return getFlag(GALE_FLAG_KEY_CTRL);
}
bool gale::key::Special::getCtrlLeft() const {
	return getFlag(GALE_FLAG_KEY_CTRL_LEFT);
}
bool gale::key::Special::getCtrlRight() const {
	return getFlag(GALE_FLAG_KEY_CTRL_RIGHT);
}
void gale::key::Special::setCtrlLeft(bool _value) {
	setFlag(GALE_FLAG_KEY_CTRL_LEFT, _value);
}
void gale::key::Special::setCtrlRight(bool _value) {
	setFlag(GALE_FLAG_KEY_CTRL_RIGHT, _value);
}

bool gale::key::Special::getMeta() const {
	return getFlag(GALE_FLAG_KEY_META);
}
bool gale::key::Special::getMetaLeft() const {
	return getFlag(GALE_FLAG_KEY_META_LEFT);
}
bool gale::key::Special::getMetaRight() const {
	return getFlag(GALE_FLAG_KEY_META_RIGHT);
}
void gale::key::Special::setMetaLeft(bool _value) {
	setFlag(GALE_FLAG_KEY_META_LEFT, _value);
}
void gale::key::Special::setMetaRight(bool _value) {
	setFlag(GALE_FLAG_KEY_META_RIGHT, _value);
}

bool gale::key::Special::getAlt() const {
	return getFlag(GALE_FLAG_KEY_ALT);
}
bool gale::key::Special::getAltLeft() const {
	return getFlag(GALE_FLAG_KEY_ALT_LEFT);
}
bool gale::key::Special::getAltRight() const {
	return getFlag(GALE_FLAG_KEY_ALT_RIGHT);
}
void gale::key::Special::setAltLeft(bool _value) {
	setFlag(GALE_FLAG_KEY_ALT_LEFT, _value);
}
void gale::key::Special::setAltRight(bool _value) {
	setFlag(GALE_FLAG_KEY_ALT_RIGHT, _value);
}

bool gale::key::Special::getNumLock() const {
	return getFlag(GALE_FLAG_KEY_NUM_LOCK);
}
void gale::key::Special::setNumLock(bool _value) {
	setFlag(GALE_FLAG_KEY_NUM_LOCK, _value);
}

bool gale::key::Special::getInsert() const {
	return getFlag(GALE_FLAG_KEY_INSERT);
}
void gale::key::Special::setInsert(bool _value) {
	setFlag(GALE_FLAG_KEY_INSERT, _value);
}

etk::Stream& gale::key::operator <<(etk::Stream& _os, const gale::key::Special& _obj) {
	_os << " capLock=" << _obj.getCapsLock();
	_os << " shift=" << _obj.getShift();
	_os << " ctrl=" << _obj.getCtrl();
	_os << " meta=" << _obj.getMeta();
	_os << " alt=" << _obj.getAlt();
	_os << " verNum=" << _obj.getNumLock();
	_os << " insert=" << _obj.getInsert();
	return _os;
}

namespace etk {
	template<> etk::String toString<gale::key::Special>(const gale::key::Special& _obj) {
		etk::String out;
		if (_obj.getCapsLock() == true) {
			out += "CAPS";
		}
		if (_obj.getShift() == true) {
			if (out.size() > 0) {
				out += "|";
			}
			out += "SHIFT";
			if (_obj.getShiftLeft() == false) {
				out += "-RIGHT";
			} else if (_obj.getShiftRight() == false) {
				out += "-LEFT";
			}
		}
		if (_obj.getCtrl() == true) {
			if (out.size() > 0) {
				out += "|";
			}
			out += "CTRL";
			if (_obj.getCtrlLeft() == false) {
				out += "-RIGHT";
			} else if (_obj.getCtrlRight() == false) {
				out += "-LEFT";
			}
		}
		if (_obj.getMeta() == true) {
			if (out.size() > 0) {
				out += "|";
			}
			out += "META";
			if (_obj.getMetaLeft() == false) {
				out += "-RIGHT";
			} else if (_obj.getMetaRight() == false) {
				out += "-LEFT";
			}
		}
		if (_obj.getAlt() == true) {
			if (out.size() > 0) {
				out += "|";
			}
			out += "ALT";
			if (_obj.getAltLeft() == false) {
				out += "-RIGHT";
			} else if (_obj.getAltRight() == false) {
				out += "-LEFT";
			}
		}
		if (_obj.getNumLock() == true) {
			if (out.size() > 0) {
				out += "|";
			}
			out += "NUM_LOCK";
		}
		if (_obj.getInsert() == true) {
			if (out.size() > 0) {
				out += "|";
			}
			out += "INSERT";
		}
		return out;
	}
	template <> bool from_string<gale::key::Special>(gale::key::Special& _variableRet, const etk::String& _value) {
		gale::key::Special out;
		etk::Vector<etk::String> listElem = etk::split(_value, "|");
		for (auto &it : listElem) {
			if (it == "CAPS") {
				out.setCapsLock(true);
			} else if (it == "SHIFT") {
				out.setShiftLeft(true);
				out.setShiftRight(true);
			} else if (it == "SHIFT-LEFT") {
				out.setShiftLeft(true);
			} else if (it == "SHIFT-RIGHT") {
				out.setShiftRight(true);
			} else if (it == "CTRL") {
				out.setCtrlLeft(true);
				out.setCtrlRight(true);
			} else if (it == "CTRL-LEFT") {
				out.setCtrlLeft(true);
			} else if (it == "CTRL-RIGHT") {
				out.setCtrlRight(true);
			} else if (it == "META") {
				out.setMetaLeft(true);
				out.setMetaRight(true);
			} else if (it == "META-LEFT") {
				out.setMetaLeft(true);
			} else if (it == "META-RIGHT") {
				out.setMetaRight(true);
			} else if (it == "ALT") {
				out.setAltLeft(true);
				out.setAltRight(true);
			} else if (it == "ALT-LEFT") {
				out.setAltLeft(true);
			} else if (it == "ALT-RIGHT") {
				out.setAltRight(true);
			} else if (it == "NUM_LOCK") {
				out.setNumLock(true);
			} else if (it == "INSERT") {
				out.setInsert(true);
			} else {
				GALE_ERROR("unknow element '" << it << "'");
				return false;
			}
		}
		_variableRet = out;
		return true;
	}
}