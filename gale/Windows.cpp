/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <etk/types.h>
#include <gale/gale.h>
#include <gale/openGL/openGL.h>
#include <gale/context/Context.h>
#include <gale/widget/Widget.h>
#include <gale/widget/Windows.h>
#include <gale/widget/Manager.h>
#include <gale/widget/meta/StdPopUp.h>

#undef __class__
#define __class__ "Windows"

//list of local events : 
extern const char * const galeEventWindowsHideKeyboard   = "gale Windows hideKeyboard";


gale::widget::Windows::Windows() :
  m_colorProperty(nullptr),
  m_colorBg(-1) {
	addResourceType("gale::widget::Windows");
	setCanHaveFocus(true);
	m_colorProperty = gale::resource::ColorFile::create("THEME:COLOR:Windows.json");
	if (m_colorProperty != nullptr) {
		m_colorBg = m_colorProperty->request("background");
	}
	//KeyboardShow(KEYBOARD_MODE_CODE);
}

gale::widget::Windows::~Windows() {
	m_subWidget.reset();
	m_popUpWidgetList.clear();
}

void gale::widget::Windows::calculateSize(const vec2& _availlable) {
	GALE_DEBUG(" _availlable : " << _availlable);
	m_size = _availlable;
	if (m_subWidget != nullptr) {
		m_subWidget->calculateMinMaxSize();
		// TODO : Check if min size is possible ...
		// TODO : Herited from MinSize .. and expand ???
		m_subWidget->calculateSize(m_size);
	}
	for (auto &it : m_popUpWidgetList) {
		if(it != nullptr) {
			it->calculateMinMaxSize();
			it->calculateSize(m_size);
		}
	}
}

std::shared_ptr<gale::Widget> gale::widget::Windows::getWidgetAtPos(const vec2& _pos) {
	// calculate relative position
	vec2 relativePos = relativePosition(_pos);
	// event go directly on the pop-up
	if (0 < m_popUpWidgetList.size()) {
		return m_popUpWidgetList.back()->getWidgetAtPos(_pos);
	// otherwise in the normal windows
	} else if (nullptr != m_subWidget) {
		return m_subWidget->getWidgetAtPos(_pos);
	}
	// otherwise the event go to this widget ...
	return std::dynamic_pointer_cast<gale::Widget>(shared_from_this());
}

void gale::widget::Windows::sysDraw() {
	//GALE_DEBUG("Drow on (" << m_size.x << "," << m_size.y << ")");
	// set the size of the open GL system
	glViewport(0,0,m_size.x(),m_size.y());
	
	gale::openGL::disable(gale::openGL::FLAG_DITHER);
	//gale::openGL::disable(gale::openGL::FLAG_BLEND);
	gale::openGL::disable(gale::openGL::FLAG_STENCIL_TEST);
	gale::openGL::disable(gale::openGL::FLAG_ALPHA_TEST);
	gale::openGL::disable(gale::openGL::FLAG_FOG);
	#if (!defined(__TARGET_OS__Android) && !defined(__TARGET_OS__IOs))
		glPixelZoom(1.0,1.0);
	#endif
	gale::openGL::disable(gale::openGL::FLAG_TEXTURE_2D);
	gale::openGL::disable(gale::openGL::FLAG_DEPTH_TEST);
	
	gale::openGL::enable(gale::openGL::FLAG_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// clear the matrix system :
	mat4 newOne;
	gale::openGL::setBasicMatrix(newOne);
	
	gale::DrawProperty displayProp;
	displayProp.m_windowsSize = m_size;
	displayProp.m_origin.setValue(0,0);
	displayProp.m_size = m_size;
	systemDraw(displayProp);
	gale::openGL::disable(gale::openGL::FLAG_BLEND);
	return;
}

void gale::widget::Windows::onRegenerateDisplay() {
	if (nullptr != m_subWidget) {
		m_subWidget->onRegenerateDisplay();
	}
	for (auto &it : m_popUpWidgetList) {
		if (it != nullptr) {
			it->onRegenerateDisplay();
		}
	}
}

//#define TEST_PERFO_WINDOWS

void gale::widget::Windows::systemDraw(const gale::DrawProperty& _displayProp) {
	gale::Widget::systemDraw(_displayProp);
	#ifdef TEST_PERFO_WINDOWS
	int64_t ___startTime0 = gale::getTime();
	#endif
	
	// clear the screen with transparency ...
	etk::Color<float> colorBg(0.5, 0.5, 0.5, 0.5);
	if (m_colorProperty != nullptr) {
		colorBg = m_colorProperty->get(m_colorBg);
	}
	glClearColor(colorBg.r(), colorBg.g(), colorBg.b(), colorBg.a());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	#ifdef TEST_PERFO_WINDOWS
	float ___localTime0 = (float)(gale::getTime() - ___startTime0) / 1000.0f;
	GALE_ERROR("      Windows000  : " << ___localTime0 << "ms ");
	
	int64_t ___startTime1 = gale::getTime();
	#endif
	//GALE_WARNING(" WINDOWS draw on " << m_currentDrawId);
	// first display the windows on the display
	if (nullptr != m_subWidget) {
		m_subWidget->systemDraw(_displayProp);
		//GALE_DEBUG("Draw Windows");
	}
	#ifdef TEST_PERFO_WINDOWS
	float ___localTime1 = (float)(gale::getTime() - ___startTime1) / 1000.0f;
	GALE_ERROR("      Windows111  : " << ___localTime1 << "ms ");
	
	int64_t ___startTime2 = gale::getTime();
	#endif
	// second display the pop-up
	for (auto &it : m_popUpWidgetList) {
		if (it != nullptr) {
			it->systemDraw(_displayProp);
			//GALE_DEBUG("Draw Pop-up");
		}
	}
	#ifdef TEST_PERFO_WINDOWS
	float ___localTime2 = (float)(gale::getTime() - ___startTime2) / 1000.0f;
	GALE_ERROR("      Windows222  : " << ___localTime2 << "ms ");
	#endif
}

void gale::widget::Windows::setSubWidget(std::shared_ptr<gale::Widget> _widget) {
	if (m_subWidget != nullptr) {
		GALE_INFO("Remove current main windows Widget...");
		m_subWidget->removeParent();
		m_subWidget.reset();
	}
	if (_widget != nullptr) {
		m_subWidget = _widget;
		m_subWidget->setParent(shared_from_this());
	}
	
	// Regenerate the size calculation :
	calculateSize(m_size);
}

void gale::widget::Windows::popUpWidgetPush(std::shared_ptr<gale::Widget> _widget) {
	if (_widget == nullptr) {
		// nothing to do an error appear :
		GALE_ERROR("can not set widget pop-up (null pointer)");
		return;
	}
	m_popUpWidgetList.push_back(_widget);
	_widget->setParent(shared_from_this());
	// force the focus on the basic widget ==> this remove many time the virual keyboard area
	_widget->keepFocus();
	// Regenerate the size calculation :
	calculateSize(m_size);
	// TODO : it is dangerous to access directly to the system ...
	getContext().resetIOEvent();
}

void gale::widget::Windows::popUpWidgetPop() {
	if (m_popUpWidgetList.size() == 0) {
		return;
	}
	m_popUpWidgetList.pop_back();
}

void gale::widget::Windows::setBackgroundColor(const etk::Color<float>& _color) {
	if (m_backgroundColor != _color) {
		m_backgroundColor = _color;
		markToRedraw();
	}
}

void gale::widget::Windows::setTitle(const std::string& _title) {
	// TODO : remove this ...
	std::string title = _title;
	getContext().setTitle(title);
}


void gale::widget::Windows::createPopUpMessage(enum popUpMessageType _type, const std::string& _message) {
	std::shared_ptr<gale::widget::StdPopUp> tmpPopUp = widget::StdPopUp::create();
	if (tmpPopUp == nullptr) {
		GALE_ERROR("Can not create a simple pop-up");
		return;
	}
	switch(_type) {
		case messageTypeInfo:
			tmpPopUp->setTitle("<bold>Info</bold>");
			break;
		case messageTypeWarning:
			tmpPopUp->setTitle("<bold><font color=\"orange\">Warning</font></bold>");
			break;
		case messageTypeError:
			tmpPopUp->setTitle("<bold><font color=\"red\">Error</font></bold>");
			break;
		case messageTypeCritical:
			tmpPopUp->setTitle("<bold><font colorBg=\"red\">Critical</font></bold>");
			break;
	}
	tmpPopUp->setComment(_message);
	tmpPopUp->addButton("close", true);
	tmpPopUp->setRemoveOnExternClick(true);
	popUpWidgetPush(tmpPopUp);
}

void gale::widget::Windows::requestDestroyFromChild(const std::shared_ptr<Object>& _child) {
	auto it = m_popUpWidgetList.begin();
	while (it != m_popUpWidgetList.end()) {
		if (*it == _child) {
			if (*it == nullptr) {
				m_popUpWidgetList.erase(it);
				it = m_popUpWidgetList.begin();
				continue;
			}
			(*it)->removeParent();
			(*it).reset();
			m_popUpWidgetList.erase(it);
			it = m_popUpWidgetList.begin();
			markToRedraw();
			continue;
		}
		++it;
	}
	if (m_subWidget == _child) {
		if (m_subWidget == nullptr) {
			return;
		}
		m_subWidget->removeParent();
		m_subWidget.reset();
		markToRedraw();
	}
}

std::shared_ptr<gale::Object> gale::widget::Windows::getSubObjectNamed(const std::string& _objectName) {
	std::shared_ptr<gale::Object> tmpObject = gale::Widget::getSubObjectNamed(_objectName);
	if (tmpObject != nullptr) {
		return tmpObject;
	}
	// check direct subwidget
	if (m_subWidget != nullptr) {
		tmpObject = m_subWidget->getSubObjectNamed(_objectName);
		if (tmpObject != nullptr) {
			return tmpObject;
		}
	}
	// get all subwidget "pop-up"
	for (auto &it : m_popUpWidgetList) {
		if (it != nullptr) {
			tmpObject = it->getSubObjectNamed(_objectName);
			if (tmpObject != nullptr) {
				return tmpObject;
			}
		}
	}
	// not find ...
	return nullptr;
}

void gale::widget::Windows::sysOnKill() {
	if (onKill() == true) {
		getContext().stop();
	}
}

