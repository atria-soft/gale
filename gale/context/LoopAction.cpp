/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

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
	std::string nameLower = etk::tolower(_name);
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



void gale::context::LoopActionInit::doAction(gale::Context& _context) {
	// this is due to the openGL context
	if (_context.m_application == nullptr) {
		return;
	}
	_context.m_application->onCreate(*this);
	_context.m_application->onStart(*this);
	_context.m_application->onResume(*this);
}

gale::context::LoopActionResize::LoopActionResize(const vec2& _size) :
  m_size(_size) {
	
}

void gale::context::LoopActionResize::doAction(gale::Context& _context) {
	//GALE_DEBUG("Receive MSG : THREAD_RESIZE");
	_context.m_windowsSize = m_size;
	gale::Dimension::setPixelWindowsSize(_context.m_windowsSize);
	_context.forceRedrawAll();
}

void gale::context::LoopActionRecalculateSize::doAction(gale::Context& _context) {
	_context.forceRedrawAll();
}

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
	if (_context.m_application == nullptr) {
		return;
	}
	_context.m_application->onPointer(m_type,
	                                  m_pointerID,
	                                  m_pos,
	                                  m_status);
}

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
	if (_context.m_application == nullptr) {
		return;
	}
	_context.m_application->onKeyboard(m_special,
	                                   m_type,
	                                   m_char,
	                                   m_state);
}
