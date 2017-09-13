/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

extern "C" {
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
}

#include <etk/types.hpp>
#include <etk/os/FSNode.hpp>
#include <etk/tool.hpp>

#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/key/key.hpp>
#include <gale/context/commandLine.hpp>
#include <gale/resource/Manager.hpp>
#include <gale/context/Context.hpp>
#include <gale/Dimension.hpp>
#include <etk/etk.hpp>


#include <gale/renderer/openGL/openGL.hpp>
#include <gale/renderer/openGL/openGL-include.hpp>


static etk::Vector<etk::String> localSplit(const etk::String& _input) {
	etk::Vector<etk::String> out;
	char lastValue = '\0';
	etk::String tmpInput;
	for(auto &it : _input) {
		if (    lastValue == ':'
		     && it == ':') {
			tmpInput += "::";
			lastValue = '\0';
			continue;
		} else if (lastValue == ':') {
			out.pushBack(tmpInput);
			tmpInput = "";
			tmpInput += it;
		} else if (it == ':') {
			// nothing to do ...
		} else {
			tmpInput += it;
		}
		lastValue = it;
		
	}
	if (tmpInput!="") {
		out.pushBack(tmpInput);
	}
	return out;
}



class SimulationInterface : public gale::Context {
	private:
		gale::key::Special m_guiKeyBoardMode;
		bool m_run;
	public:
		SimulationInterface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv) {
			GALE_INFO("SIMU:INIT");
			m_run = true;
			start2ndThreadProcessing();
		}
		
		~SimulationInterface() {
			// TODO : ...
		}
		
		int32_t run() {
			// Try to open the file of simulation:
			if (m_simulationFile.fileOpenRead() == false) {
				GALE_ERROR("can not open the simulation file");
				return -1;
			}
			etk::String action;
			// main cycle
			while(m_run == true) {
				bool lineIsOk = m_simulationFile.fileGets(action);
				if (lineIsOk == false) {
					// reach end of simulation file;
					return 0;
				}
				if (action.size() == 0) {
					continue;
				}
				GALE_VERBOSE("SIM-EVENT : '" << action << "'");
				etk::Vector<etk::String> pads = localSplit(action);
				GALE_DEBUG("    " << pads);
				// The first is the time ...
				if (pads.size() < 2) {
					GALE_ERROR("Simu: Can not parse: '" << action << "'");
					continue;
				}
				uint64_t time = etk::string_to_uint64_t(pads[0]);
				etk::String localAction = pads[1];
				if (localAction == "INIT") {
					// nothing to do ...
				} else if (localAction == "RECALCULATE_SIZE") {
					//requestUpdateSize();
				} else if (localAction == "RESIZE") {
					// vec2
					OS_Resize(vec2(pads[2]));
				} else if (localAction == "INPUT") {
					// string type
					// string status
					// int32_t pointerID
					// vec2 pos
					enum gale::key::type type;
					etk::from_string(type, pads[2]);
					enum gale::key::status status;
					etk::from_string(status, pads[3]);
					int32_t pointerID = etk::string_to_int32_t(pads[4]);
					vec2 pos(pads[5]);
					OS_SetInput(type, status, pointerID, pos);
					
				} else if (localAction == "KEYBOARD") {
					// string special
					// string type
					// int32_t state
					// vec2 value
					
					gale::key::Special special;
					etk::from_string(special, pads[2]);
					enum gale::key::keyboard type;
					etk::from_string(type, pads[3]);
					enum gale::key::status status;
					etk::from_string(status, pads[4]);
					char32_t valChar = etk::string_to_uint64_t(pads[5]);
					
					OS_setKeyboard(special, type, status, false, valChar);
					
				} else if (localAction == "VIEW") {
					// bool ==> hide, show ...
					if (etk::string_to_bool(pads[2]) == true) {
						OS_Show();
					} else {
						OS_Hide();
					}
				} else if (localAction == "CLIPBOARD_ARRIVE") {
					// int32_t clipboard ID
					enum gale::context::clipBoard::clipboardListe clipboardId;
					etk::from_string(clipboardId, pads[2]);
					OS_ClipBoardArrive(clipboardId);
				} else if (localAction == "DRAW") {
					// bool ==> display every time ...
					OS_Draw(etk::string_to_bool(pads[2]));
				} else {
					GALE_ERROR("unknow event : '" << localAction << "'");
				}
			}
			m_simulationFile.fileClose();
			return 0;
		}
		/****************************************************************************************/
		virtual void stop() {
			GALE_INFO("SIMU-API: Stop");
			m_run = false;
		}
		/****************************************************************************************/
		virtual void setSize(const vec2& _size) {
			GALE_INFO("SIMU-API: changeSize=" << _size);
		}
		/****************************************************************************************/
		void setFullScreen(bool _status) {
			GALE_INFO("SIMU-API: changeFullscreen=" << _status);
		}
		/****************************************************************************************/
		virtual void grabKeyboardEvents(bool _status) {
			
		}
		/****************************************************************************************/
		virtual void setWindowsDecoration(bool _status) {
			GALE_INFO("SIMU-API: setWindows Decoration :" << _status);
		};
		/****************************************************************************************/
		virtual void setPos(const vec2& _pos) {
			GALE_INFO("SIMU-API: changePos=" << _pos);
		}
		/****************************************************************************************/
		virtual void setCursor(enum gale::context::cursor _newCursor) {
			GALE_INFO("SIMU-API: setCursor=" << _newCursor);
		}
		/****************************************************************************************/
		void grabPointerEvents(bool _status, const vec2& _forcedPosition) {
			GALE_DEBUG("SIMU-API: Grab Events");
		}
		/****************************************************************************************/
		void setIcon(const etk::String& _inputFile) {
			GALE_DEBUG("SIMU set icon " << _inputFile);
		}
		/****************************************************************************************/
		void setTitle(const etk::String& _title) {
			GALE_INFO("SIMU: set Title " << _title);
		}
		void openURL(const etk::String& _url) {
			GALE_INFO("SIMU: open URL " << _url);
		}
		/****************************************************************************************/
		void clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			GALE_INFO("SIMU: clipBoardGet " << _clipboardID);
		}
		/****************************************************************************************/
		void clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			GALE_INFO("SIMU: clipBoardSet " << _clipboardID);
		}
};


#include <gale/context/simulation/Context.hpp>

bool gale::context::simulation::isBackendPresent() {
	// TODO : Do it better...
	return true;
}

ememory::SharedPtr<gale::Context> gale::context::simulation::createInstance(gale::Application* _application, int _argc, const char *_argv[]) {
	// Disable openGL:
	gale::openGL::startSimulationMode();
	return ememory::makeShared<SimulationInterface>(_application, _argc, _argv);
}
