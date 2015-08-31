/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <unistd.h>

#include <etk/types.h>
#include <etk/etk.h>

#include <etk/tool.h>
#include <etk/os/Fifo.h>
#include <etk/Hash.h>
#include <etk/thread/tools.h>
#include <mutex>
#include <date/date.h>
#include <gale/gale.h>
#include <gale/Dimension.h>
#include <gale/debug.h>

#include <gale/context/LoopAction.h>
#include <gale/context/Context.h>

etk::Hash<std::function<std::shared_ptr<gale::context::LoopAction>(const std::string&)> >& getList() {
	static etk::Hash<std::function<std::shared_ptr<gale::context::LoopAction>(const std::string&)> > list;
	return list;
}

std::shared_ptr<gale::context::LoopAction> gale::context::createAction(const std::string& _lineToParse) {
	// TODO: parse line ...
	std::string _name = "lkjlkjlkjlk";
	std::string nameLower = etk::tolower(_name);
	if (getList().exist(nameLower) == true) {
		std::function<std::shared_ptr<gale::context::LoopAction>(const std::string&)> func = getList()[nameLower];
		if (func != nullptr) {
			return func(_lineToParse);
		}
	}
	GALE_WARNING("try to create an UnExistant widget : " << nameLower);
	return nullptr;
}

void gale::context::addFactory(const std::string& _type, const std::function<std::shared_ptr<gale::context::LoopAction>(const std::string&)>& _func) {
	if (_func == nullptr) {
		return;
	}
	//Keep name in lower case :
	std::string nameLower = etk::tolower(_type);
	if (true == getList().exist(nameLower)) {
		GALE_WARNING("Replace Creator of a loop action : " << nameLower);
		getList()[nameLower] = _func;
		return;
	}
	GALE_INFO("Add Creator of a specify loop action : " << nameLower);
	getList().add(nameLower, _func);
}

gale::context::LoopAction::LoopAction() {
	m_timestamp = gale::getTime();
}

gale::context::LoopAction::~LoopAction() {
	
}

/////////////////////////////////////////////////////////////////////////////////////////

void gale::context::LoopActionInit::doAction(gale::Context& _context) {
	std::shared_ptr<gale::Application> appl = _context.getApplication();
	// this is due to the openGL context
	if (appl == nullptr) {
		return;
	}
	appl->onCreate(_context);
	appl->onStart(_context);
	appl->onResume(_context);
}

std::string gale::context::LoopActionInit::createString() {
	return etk::to_string(m_timestamp) + ":INIT";
}

/////////////////////////////////////////////////////////////////////////////////////////
gale::context::LoopActionResize::LoopActionResize(const vec2& _size) :
  m_size(_size) {
	
}

void gale::context::LoopActionResize::doAction(gale::Context& _context) {
	//GALE_DEBUG("Receive MSG : THREAD_RESIZE");
	_context.m_windowsSize = m_size;
	gale::Dimension::setPixelWindowsSize(_context.m_windowsSize);
	_context.forceRedrawAll();
}

std::string gale::context::LoopActionResize::createString() {
	return etk::to_string(m_timestamp) + ":RESIZE:" + etk::to_string(m_size);
}

/////////////////////////////////////////////////////////////////////////////////////////
gale::context::LoopActionView::LoopActionView(bool _show) :
  m_show(_show) {
	
}

void gale::context::LoopActionView::doAction(gale::Context& _context) {
	GALE_TODO("kjhkjhkhkjh");
}

std::string gale::context::LoopActionView::createString() {
	return etk::to_string(m_timestamp) + ":VIEW:" + etk::to_string(m_show);
}

/////////////////////////////////////////////////////////////////////////////////////////

void gale::context::LoopActionRecalculateSize::doAction(gale::Context& _context) {
	_context.forceRedrawAll();
}

std::string gale::context::LoopActionRecalculateSize::createString() {
	return etk::to_string(m_timestamp) + ":RECALCULATE_SIZE";
}

/////////////////////////////////////////////////////////////////////////////////////////
gale::context::LoopActionInput::LoopActionInput(enum gale::key::type _type,
                                                enum gale::key::status _status,
                                                int32_t _pointerID,
                                                const vec2& _pos) :
  m_type(_type),
  m_status(_status),
  m_pointerID(_pointerID),
  m_pos(_pos) {
	
}

void gale::context::LoopActionInput::doAction(gale::Context& _context) {
	std::shared_ptr<gale::Application> appl = _context.getApplication();
	if (appl == nullptr) {
		return;
	}
	appl->onPointer(m_type,
	                m_pointerID,
	                m_pos,
	                m_status);
}

std::string gale::context::LoopActionInput::createString() {
	return etk::to_string(m_timestamp) + ":INPUT:" + etk::to_string(m_type) + ":" + etk::to_string(m_status) + ":" + etk::to_string(m_pointerID) + ":" + etk::to_string(m_pos);
}

/////////////////////////////////////////////////////////////////////////////////////////

gale::context::LoopActionKeyboard::LoopActionKeyboard(const gale::key::Special& _special,
                                                      enum gale::key::keyboard _type,
                                                      enum gale::key::status _state,
                                                      char32_t _char) :
  m_special(_special),
  m_type(_type),
  m_state(_state),
  m_char(_char) {
	
}

void gale::context::LoopActionKeyboard::doAction(gale::Context& _context) {
	std::shared_ptr<gale::Application> appl = _context.getApplication();
	if (appl == nullptr) {
		return;
	}
	appl->onKeyboard(m_special,
	                 m_type,
	                 m_char,
	                 m_state);
}

std::string gale::context::LoopActionKeyboard::createString() {
	return etk::to_string(m_timestamp) + ":KEYBOARD:" + etk::to_string(m_special) + ":" + etk::to_string(m_type) + ":" + etk::to_string(m_state) + ":" + etk::to_string(uint64_t(m_char));
}

/////////////////////////////////////////////////////////////////////////////////////////

gale::context::LoopActionClipboardArrive::LoopActionClipboardArrive(enum gale::context::clipBoard::clipboardListe _id) :
  m_id(_id) {
	
}

void gale::context::LoopActionClipboardArrive::doAction(gale::Context& _context) {
	std::shared_ptr<gale::Application> appl = _context.getApplication();
	if (appl != nullptr) {
		appl->onClipboardEvent(m_id);
	}
}

std::string gale::context::LoopActionClipboardArrive::createString() {
	return etk::to_string(m_timestamp) + ":CLIPBOARD_ARRIVE:" + etk::to_string(m_id);
}

/////////////////////////////////////////////////////////////////////////////////////////


