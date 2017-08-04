/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/key/key.hpp>
#include <gale/context/commandLine.hpp>
#include <gale/context/clipBoard.hpp>
#include <etk/types.hpp>
#include <etk/os/FSNode.hpp>

#include <gale/resource/Manager.hpp>
#include <gale/context/Context.hpp>

#include <gale/context/MacOs/Interface.hpp>
#include <gale/context/MacOs/Context.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <sys/times.h>
#include <mach/clock.h>
#include <mach/mach.h>
#include <etk/etk.hpp>

#import <Cocoa/Cocoa.h>

class MacOSInterface : public gale::Context {
	private:
		gale::key::Special m_guiKeyBoardMode;
	public:
		MacOSInterface(gale::Application* _application, int _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv) {
			mm_main(_argc, _argv);
			start2ndThreadProcessing();
		}
		
		int32_t run() {
			return mm_run();
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
		void MAC_SetKeyboard(gale::key::Special _special, int32_t _unichar, bool _isDown, bool _isAReapeateKey) {
			if (char32_t(_unichar) == u32char::Delete) {
				_unichar = u32char::Suppress;
			} else if (char32_t(_unichar) == u32char::Suppress) {
				_unichar = u32char::Delete;
			}
			if (char32_t(_unichar) == u32char::CarrierReturn) {
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
				OS_setKeyboard(_special, move, (_isDown==false?gale::key::status::down:gale::key::status::up), _isAReapeateKey);
			} else {
				OS_setKeyboard(_special, gale::key::keyboard::character, (_isDown==false?gale::key::status::down:gale::key::status::up), _isAReapeateKey, _unichar);
			}
		}
		void MAC_SetKeyboardMove(gale::key::Special& _special,
								enum gale::key::keyboard _move,
								bool _isDown,
								bool _isAReapeateKey) {
			OS_setKeyboard(_special, _move, (_isDown==true?gale::key::status::down:gale::key::status::up), _isAReapeateKey);
		}
		void openURL(const std::string& _url) {
			std::string req = "open " + _url;
			system(req.c_str());
		}
		void MAC_Stop() {
			OS_Stop();
		}
		void stop() {
			mm_stopApplication();
		}
		void clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			if (_clipboardID == gale::context::clipBoard::clipboardStd) {
				NSPasteboard* myPasteboard = [NSPasteboard generalPasteboard];
				NSString* myString = [myPasteboard stringForType:NSPasteboardTypeString];
				std::string val([myString UTF8String]);
				gale::context::clipBoard::setSystem(_clipboardID, val);
				if (val.size() != 0) {
					OS_ClipBoardArrive(_clipboardID);
				}
			} else {
				gale::Context::clipBoardGet(_clipboardID);
			}
		}
		void clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			if (_clipboardID == gale::context::clipBoard::clipboardStd) {
				NSPasteboard* myPasteboard = [NSPasteboard generalPasteboard];
				[myPasteboard clearContents];
				//GALE_ERROR(" copy: " << gale::context::clipBoard::get(_clipboardID));
				NSString *text = [[NSString alloc] initWithUTF8String:gale::context::clipBoard::get(_clipboardID).c_str()];
				BOOL err = [myPasteboard setString:text forType:NSPasteboardTypeString];
				if (err == FALSE) {
					GALE_ERROR("copy to clipboard can not be done ..."); 
				}
			} else {
				gale::Context::clipBoardSet(_clipboardID);
			}
		}
	
};


MacOSInterface* interface = nullptr;



bool MacOs::draw(bool _displayEveryTime) {
    GALE_VERBOSE("draw request " << uint64_t(interface) << "  "  << _displayEveryTime);
    if (interface == nullptr) {
		return false;
	}
	return interface->MAC_Draw(_displayEveryTime);
}

void MacOs::resize(float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_Resize(_x, _y);
}


void MacOs::setMouseState(int32_t _id, bool _isDown, float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetMouseState(_id, _isDown, _x, _y);
}

void MacOs::setMouseMotion(int32_t _id, float _x, float _y) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetMouseMotion(_id, _x, _y);
}

void MacOs::setKeyboard(gale::key::Special _keyboardMode, int32_t _unichar, bool _isDown, bool _isAReapeateKey) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetKeyboard(_keyboardMode, _unichar, _isDown, _isAReapeateKey);
}

void MacOs::setKeyboardMove(gale::key::Special& _keyboardMode, enum gale::key::keyboard _move, bool _isDown, bool _isAReapeateKey) {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_SetKeyboardMove(_keyboardMode, _move, _isDown, _isAReapeateKey);
}

void MacOs::stopRequested() {
	if (interface == nullptr) {
		return;
	}
	interface->MAC_Stop();
}
/*
void MacOs::setRedrawCallback(const std::function<void()>& _func) {
	if (interface == nullptr) {
		return;
	}
	interface->getWidgetManager().setCallbackonRedrawNeeded(_func);
}
*/


bool gale::context::macos::isBackendPresent() {
	// TODO : Do it better...
	return true;
}

ememory::SharedPtr<gale::Context> gale::context::macos::createInstance(gale::Application* _application, int _argc, const char *_argv[]) {
	ememory::SharedPtr<MacOSInterface> tmp = ememory::makeShared<MacOSInterface>(_application, _argc, _argv);
    interface = tmp.get();
    return tmp;
}
