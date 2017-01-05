/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/key/key.hpp>
#include <gale/context/commandLine.hpp>
#include <etk/types.hpp>
#include <etk/os/FSNode.hpp>

#include <gale/resource/Manager.hpp>
#include <gale/context/Context.hpp>

#include <gale/context/IOs/Interface.h>
#include <gale/context/IOs/Context.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>


#include <sys/times.h>
#include <mach/clock.h>
#include <mach/mach.h>
#include <etk/etk.hpp>

class MacOSInterface : public gale::Context {
private:
	gale::key::Special m_guiKeyBoardMode;
public:
	MacOSInterface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
	gale::Context(_application, _argc, _argv) {
		// nothing to do ...
	}
	
	int32_t Run() {
		
		return 0;
	}
	virtual void stop() {
		mm_exit();
	}
public:
	//interface MacOS :
	
	bool MAC_Draw(bool _displayEveryTime) {
		return OS_Draw(_displayEveryTime);
	}
	void MAC_Resize(float _x, float _y) {
		OS_Resize(vec2(_x,_y));
	}
	void MAC_SetMouseState(int32_t _id, bool _isDown, float _x, float _y) {
		OS_SetInput(gale::key::type::mouse,
		            (_isDown==true?gale::key::status::down:gale::key::status::up),
		            _id,
		            vec2(_x, _y));
	}
	void MAC_SetMouseMotion(int32_t _id, float _x, float _y) {
		OS_SetInput(gale::key::type::mouse,
		            gale::key::status::move,
		            _id,
		            vec2(_x, _y));
	}
	void MAC_SetInputState(int32_t _id, bool _isDown, float _x, float _y) {
		OS_SetInput(gale::key::type::finger,
		            (_isDown==true?gale::key::status::down:gale::key::status::up),
		            _id,
		            vec2(_x, _y));
	}
	void MAC_SetInputMotion(int32_t _id, float _x, float _y) {
		OS_SetInput(gale::key::type::finger,
		            gale::key::status::move,
		            _id,
		            vec2(_x, _y));
	}
	void MAC_SetKeyboard(gale::key::Special _special, int32_t _unichar, bool _isDown, bool _isARepeateKey) {
		if (_unichar == u32char::Delete) {
			_unichar = u32char::Suppress;
		} else if (_unichar == u32char::Suppress) {
			_unichar = u32char::Delete;
		}
		if (_unichar == u32char::CarrierReturn) {
			_unichar = u32char::Return;
		}
		//GALE_DEBUG("key: " << _unichar << " up=" << !_isDown);
		if (_unichar <= 4) {
			enum gale::key::keyboard move;
			switch(_unichar) {
				case 0:
					move = gale::key::keyboard::up;
					break;
				case 1:
					move = gale::key::keyboard::down;
					break;
				case 2:
					move = gale::key::keyboard::left;
					break;
				case 3:
					move = gale::key::keyboard::right;
					break;
			}
			OS_setKeyboard(_special,
			               move,
			               (_isDown==false?gale::key::status::down:gale::key::status::up),
			               _isARepeateKey);
		} else {
			OS_setKeyboard(_special,
			               gale::key::keyboard::character,
			               (_isDown==false?gale::key::status::down:gale::key::status::up),
			               _isARepeateKey,
			               _unichar);
		}
	}
	void MAC_SetKeyboardMove(gale::key::Special& _special,
							 enum gale::key::keyboard _move,
							 bool _isDown) {
		OS_setKeyboard(_special,
		               _move,
		               (_isDown==true?gale::key::status::down:gale::key::status::up));
	}
	void openURL(const std::string& _url) {
		mm_openURL(_url.c_str());
	}
};


MacOSInterface* interface = nullptr;



bool IOs::draw(bool _displayEveryTime) {
	if (interface == nullptr) {
		return false;
	}
	return interface->MAC_Draw(_displayEveryTime);
}

void IOs::resize(float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_Resize(_x, _y);
}


void IOs::setMouseState(int32_t _id, bool _isDown, float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetMouseState(_id, _isDown, _x, _y);
}

void IOs::setMouseMotion(int32_t _id, float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetMouseMotion(_id, _x, _y);
}

void IOs::setInputState(int32_t _id, bool _isDown, float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetInputState(_id, _isDown, _x, _y);
}

void IOs::setInputMotion(int32_t _id, float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetInputMotion(_id, _x, _y);
}

void IOs::setKeyboard(gale::key::Special _special, int32_t _unichar, bool _isDown, bool _isARepeateKey) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetKeyboard(_special, _unichar, _isDown, _isARepeateKey);
}

void IOs::setKeyboardMove(gale::key::Special& _special, enum gale::key::keyboard _move, bool _isDown) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetKeyboardMove(_special, _move, _isDown);
}

void IOs::start() {
	if (interface == nullptr) {
		return;
	}
	//interface->OS_Start();
}

void IOs::resume() {
	if (interface == nullptr) {
		return;
	}
	interface->OS_Resume();
}

void IOs::suspend() {
	if (interface == nullptr) {
		return;
	}
	interface->OS_Suspend();
}

void IOs::stop() {
	if (interface == nullptr) {
		return;
	}
	interface->OS_Stop();
}
void IOs::background() {
	if (interface == nullptr) {
		return;
	}
	interface->OS_Background();
}
void IOs::foreground() {
	if (interface == nullptr) {
		return;
	}
	interface->OS_Foreground();
}


static int l_argc = 0;
static const char **l_argv = nullptr;
static gale::Application* l_application;
/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	l_argc = _argc;
	l_argv = _argv;
	l_application = _application;
	return mm_main(_argc, _argv);
}

// Creat and relaese gale::Context interface:
void IOs::createInterface() {
	etk::init(l_argc, l_argv);
	GALE_INFO("Create new interface");
	interface = new MacOSInterface(l_application, l_argc, l_argv);
	l_application = nullptr;
	if (nullptr == interface) {
		GALE_CRITICAL("Can not create the X11 interface ... MEMORY allocation error");
		return;
	}
}

void IOs::releaseInterface() {
	if (interface != nullptr) {
		GALE_INFO("Remove interface");
	}
	delete(interface);
	interface = nullptr;
}






