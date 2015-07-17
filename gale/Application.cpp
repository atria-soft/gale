/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <gale/Application.h>
#include <gale/context/Context.h>


gale::Application::Application() :
  m_needRedraw(true) {
	GALE_VERBOSE("Constructor Gale Application");
}

gale::Application::~Application() {
	GALE_VERBOSE("destructor of Gale Application");
}

void gale::Application::onCreate(gale::Context& _context) {
	GALE_VERBOSE("Create Gale Application");
}

void gale::Application::onStart(gale::Context& _context) {
	GALE_VERBOSE("Start Gale Application");
}

void gale::Application::onResume(gale::Context& _context) {
	GALE_VERBOSE("Resume Gale Application");
}

void gale::Application::onRegenerateDisplay(gale::Context& _context) {
	GALE_VERBOSE("Regenerate Gale Application");
	markDrawingIsNeeded();
}

void gale::Application::markDrawingIsNeeded() {
	m_needRedraw = true;
}

bool gale::Application::isDrawingNeeded() {
	bool tmp = m_needRedraw;
	m_needRedraw = false;
	return tmp;
}
void gale::Application::onDraw(gale::Context& _context) {
	GALE_VERBOSE("draw Gale Application");
}

void gale::Application::onPause(gale::Context& _context) {
	GALE_VERBOSE("Pause Gale Application");
}

void gale::Application::onStop(gale::Context& _context) {
	GALE_VERBOSE("Stop Gale Application");
}

void gale::Application::onDestroy(gale::Context& _context) {
	GALE_VERBOSE("Destroy Gale Application");
}

void gale::Application::exit(int32_t _value) {
	GALE_VERBOSE("Exit Requested");
}

void gale::Application::onPointer(enum gale::key::type _type, int32_t _pointerID, const vec2& _pos, gale::key::status _state) {
	
}

void gale::Application::onKeyboard(gale::key::Special& _special,
                                   enum gale::key::keyboard _type,
                                   char32_t _value,
                                   gale::key::status _state) {
	
}

void gale::Application::keyboardShow() {
	
}

void gale::Application::keyboardHide() {
	
}

void gale::Application::onResize(const vec2& _size) {
	
}

void gale::Application::setSize(const vec2& _size) {
	gale::Context& context = gale::getContext();
	context.setSize(_size);
}

vec2 gale::Application::getSize() const {
	gale::Context& context = gale::getContext();
	return context.getSize();
}

void gale::Application::onMovePosition(const vec2& _size) {
	
}

void gale::Application::setPosition(const vec2& _size) {
	
}

vec2 gale::Application::getPosition() const {
	return vec2(0,0);
}

void gale::Application::setTitle(const std::string& _title) {
	
}

std::string gale::Application::getTitle() {
	return "";
}

void gale::Application::setIcon(const std::string& _iconFile) {
	
}

std::string gale::Application::getIcon() {
	return "";
}

void gale::Application::setCursor(enum gale::context::cursor _newCursor) {
	
}

enum gale::context::cursor gale::Application::getCursor() {
	return gale::context::cursor_arrow;
}

void gale::Application::openURL(const std::string& _url) {
	
}

void gale::Application::setOrientation(enum gale::orientation _orientation) {
	
}

enum gale::orientation gale::Application::getOrientation() {
	return gale::orientation_screenAuto;
}
