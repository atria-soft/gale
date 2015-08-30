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
#include <etk/os/FSNode.h>
#include <etk/thread/tools.h>
#include <mutex>

#include <date/date.h>

#include <gale/gale.h>
#include <gale/Dimension.h>
#include <gale/debug.h>

#include <gale/renderer/openGL/openGL.h>
#include <gale/context/Context.h>
#include <gale/resource/Manager.h>




/**
 * @brief get the main gale mutex (event or periodic call mutex).
 * @note due ti the fact that the system can be called for multiple instance, for naw we just limit the acces to one process at a time.
 * @return the main inteface Mutex
 */
static std::mutex& mutexInterface() {
	static  std::mutex s_interfaceMutex;
	return s_interfaceMutex;
}


static gale::Context* l_curentInterface=nullptr;
gale::Context& gale::getContext() {
	#if DEBUG_LEVEL > 2
		if(nullptr == l_curentInterface){
			GALE_CRITICAL("[CRITICAL] try acces at an empty interface");
		}
	#endif
	return *l_curentInterface;
}



void gale::Context::setInitImage(const std::string& _fileName) {
	//m_initDisplayImageName = _fileName;
}



/**
 * @brief set the curent interface.
 * @note this lock the main mutex
 */
void gale::Context::lockContext() {
	mutexInterface().lock();
	l_curentInterface = this;
}

/**
 * @brief set the curent interface at nullptr.
 * @note this un-lock the main mutex
 */
void gale::Context::unLockContext() {
	l_curentInterface = nullptr;
	mutexInterface().unlock();
}


namespace gale {
	class eSystemMessage {
		public :
			// can not set a union ...
			enum gale::context::clipBoard::clipboardListe clipboardID;
			// InputId
			enum gale::key::type inputType;
			int32_t inputId;
			// generic dimentions
			vec2 dimention;
			// keyboard events :
			bool repeateKey;  //!< special flag for the repeating key on the PC interface
			bool stateIsDown;
			char32_t keyboardChar;
			enum gale::key::keyboard keyboardMove;
			gale::key::Special keyboardSpecial;
			
			eSystemMessage() :
				clipboardID(gale::context::clipBoard::clipboardStd),
				inputType(gale::key::type_unknow),
				inputId(-1),
				dimention(0,0),
				repeateKey(false),
				stateIsDown(false),
				keyboardChar(0),
				keyboardMove(gale::key::keyboard_unknow)
			{
				
			}
	};
};

#if 0
void gale::Context::inputEventTransfertWidget(std::shared_ptr<gale::Widget> _source,
                                              std::shared_ptr<gale::Widget> _destination) {
	m_input.transfertEvent(_source, _destination);
}


void gale::Context::inputEventGrabPointer(std::shared_ptr<gale::Widget> _widget) {
	m_input.grabPointer(_widget);
}

void gale::Context::inputEventUnGrabPointer() {
	m_input.unGrabPointer();
}
#endif

void gale::Context::processEvents() {
	int32_t nbEvent = 0;
	//GALE_DEBUG(" ********  Event");
	gale::eSystemMessage* data = nullptr;
	while (m_msgSystem.count()>0) {
		nbEvent++;
		if (data != nullptr) {
			delete(data);
			data = nullptr;
		}
		m_msgSystem.wait(data);
		//GALE_DEBUG("EVENT");
		switch (data->TypeMessage) {
			case eSystemMessage::msgClipboardArrive:
				{
					std::shared_ptr<gale::Application> appl = m_application;
					if (appl != nullptr) {
						appl->onClipboardEvent(data->clipboardID);
					}
				}
				break;
		}
	}
}

void gale::Context::setArchiveDir(int _mode, const char* _str) {
	switch(_mode) {
		case 0:
			GALE_DEBUG("Directory APK : path=" << _str);
			etk::setBaseFolderData(_str);
			break;
		case 1:
			GALE_DEBUG("Directory mode=FILE path=" << _str);
			etk::setBaseFolderDataUser(_str);
			break;
		case 2:
			GALE_DEBUG("Directory mode=CACHE path=" << _str);
			etk::setBaseFolderCache(_str);
			break;
		case 3:
			GALE_DEBUG("Directory mode=EXTERNAL_CACHE path=" << _str);
			break;
		default:
			GALE_DEBUG("Directory mode=???? path=" << _str);
			break;
	}
}



gale::Context::Context(gale::Application* _application, int32_t _argc, const char* _argv[]) :
  //m_application(std::make_shared<gale::Application>(_application)),
  m_application(_application),
  //m_objectManager(*this),
  m_previousDisplayTime(0),
  // TODO : m_input(*this),
#if (defined(__TARGET_OS__Android) || defined(__TARGET_OS__IOs))
  m_displayFps(true),
#else
  m_displayFps(false),
#endif
  m_FpsSystemEvent(  "Event     ", false),
  m_FpsSystemContext("Context   ", false),
  m_FpsSystem(       "Draw      ", true),
  m_FpsFlush(        "Flush     ", false),
  m_windowsSize(320,480),
  m_initStepId(0) {
	// set a basic 
	etk::thread::setName("galeThread");
	if (m_application == nullptr) {
		GALE_CRITICAL("Can not start context with no Application ==> rtfm ...");
	}
	m_commandLine.parse(_argc, _argv);
	GALE_INFO(" == > Gale system init (BEGIN)");
	// Reset the random system to be sure have real random values...
	etk::tool::resetRandom();
	// set the curent interface :
	lockContext();
	// By default we set 2 themes (1 color and 1 shape ...) :
	etk::theme::setNameDefault("GUI", "shape/square/");
	etk::theme::setNameDefault("COLOR", "color/black/");
	// parse the debug level:
	for(int32_t iii=0; iii<m_commandLine.size(); ++iii) {
		if (m_commandLine.get(iii) == "--gale-fps") {
			m_displayFps=true;
		} else if (etk::start_with(m_commandLine.get(iii), "--gale-simulation-file=") == true) {
			
		} else if (etk::start_with(m_commandLine.get(iii), "--gale-simulation-mode=") == true) {
			
		} else if (m_commandLine.get(iii) == "--gale-simulation-stop") {
			
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		} else if (m_commandLine.get(iii) == "--gale-disable-opengl") {
			gale::openGL::startSimulationMode();
		#endif
		} else if (    m_commandLine.get(iii) == "-h"
		            || m_commandLine.get(iii) == "--help"
		            || etk::start_with(m_commandLine.get(iii), "--gale") == true) {
			GALE_PRINT("gale - help : ");
			GALE_PRINT("    " << etk::getApplicationName() << " [options]");
			GALE_PRINT("        --gale-simulation-file=XXX.gsim");
			GALE_PRINT("                Enable the simulation mode of the gale IO, parameter: file (default:simulation Gale.gsim)");
			GALE_PRINT("        --gale-simulation-mode=XXX");
			GALE_PRINT("                Mode of the simulation");
			GALE_PRINT("                    - record   Record all input of the playing element (default)");
			GALE_PRINT("                    - play     Play all the sequence write in the simulation file");
			GALE_PRINT("        --gale-simulation-stop");
			GALE_PRINT("                Stop at the end of the simulation");
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				GALE_PRINT("        --gale-disable-opengl");
				GALE_PRINT("                Disable openGL access (availlable in SIMULATION mode)");
			#endif
			GALE_PRINT("        --gale-fps");
			GALE_PRINT("                Display the current fps of the display");
			GALE_PRINT("        -h/--help");
			GALE_PRINT("                Display this help");
			GALE_PRINT("    example:");
			GALE_PRINT("        " << etk::getApplicationName() << " --gale-fps");
			if (etk::start_with(m_commandLine.get(iii), "--gale") == true) {
				GALE_ERROR("gale unknow element in parameter: '" << m_commandLine.get(iii) << "'");
				// remove parameter ...
			} else {
				// this is a global help system does not remove it
				continue;
			}
		} else {
			continue;
		}
		m_commandLine.remove(iii);
		--iii;
	}
	//etk::cout.setOutputFile(true);
	
	GALE_INFO("GALE v:" << gale::getVersion());
	GALE_INFO("Build Date: " << date::getYear() << "/" << date::getMonth() << "/" << date::getDay() << " " << date::getHour() << "h" << date::getMinute());
	// TODO : remove this ...
	etk::initDefaultFolder("galeApplNoName");
	// request the init of the application in the main context of openGL ...
	m_msgSystem.post(std::make_shared<gale::context::LoopActionInit>());
	// force a recalculation
	requestUpdateSize();
	#if defined(__GALE_ANDROID_ORIENTATION_LANDSCAPE__)
		forceOrientation(gale::orientation_screenLandscape);
	#elif defined(__GALE_ANDROID_ORIENTATION_PORTRAIT__)
		forceOrientation(gale::orientation_screenPortrait);
	#else
		forceOrientation(gale::orientation_screenAuto);
	#endif
	// release the curent interface :
	unLockContext();
	GALE_INFO(" == > Gale system init (END)");
}

gale::Context::~Context() {
	GALE_INFO(" == > Gale system Un-Init (BEGIN)");
	// TODO : Clean the message list ...
	// set the curent interface :
	lockContext();
	// clean all widget and sub widget with their resources:
	//m_objectManager.cleanInternalRemoved();
	// call application to uninit
	m_application->onPause(*this);
	m_application->onStop(*this);
	m_application->onDestroy(*this);
	m_application.reset();
	// clean all messages
	m_msgSystem.clean();
	// internal clean elements
	//m_objectManager.cleanInternalRemoved();
	m_resourceManager.cleanInternalRemoved();
	
	GALE_INFO("List of all widget of this context must be equal at 0 ==> otherwise some remove is missing");
	//m_objectManager.displayListObject();
	// Resource is an lower element as objects ...
	m_resourceManager.unInit();
	// now All must be removed !!!
	//m_objectManager.unInit();
	// release the curent interface :
	unLockContext();
	GALE_INFO(" == > Gale system Un-Init (END)");
}

void gale::Context::requestUpdateSize() {
	m_msgSystem.post(std::make_shared<gale::context::LoopActionRecalculateSize>());
}

void gale::Context::OS_Resize(const vec2& _size) {
	// TODO : Better in the thread ...  == > but generate some init error ...
	gale::Dimension::setPixelWindowsSize(_size);
	m_msgSystem.post(std::make_shared<gale::context::LoopActionResize>(_size));
}
void gale::Context::OS_Move(const vec2& _pos) {
	/*
	gale::eSystemMessage *data = new gale::eSystemMessage();
	data->TypeMessage = eSystemMessage::msgResize;
	data->resize.w = w;
	data->resize.h = h;
	m_msgSystem.Post(data);
	*/
}

void gale::Context::OS_SetInputMotion(int _pointerID, const vec2& _pos ) {
	m_msgSystem.post(std::make_shared<gale::context::LoopActionInput>(gale::key::type_finger,
	                                                                  gale::key::status_move,
	                                                                  _pointerID,
	                                                                  _pos));
}

void gale::Context::OS_SetInputState(int _pointerID, bool _isDown, const vec2& _pos ) {
	m_msgSystem.post(std::make_shared<gale::context::LoopActionInput>(gale::key::type_finger,
	                                                                  (_isDown==true?gale::key::status_down:gale::key::status_up),
	                                                                  _pointerID,
	                                                                  _pos));
}

void gale::Context::OS_SetMouseMotion(int _pointerID, const vec2& _pos ) {
	m_msgSystem.post(std::make_shared<gale::context::LoopActionInput>(gale::key::type_mouse,
	                                                                  gale::key::status_move,
	                                                                  _pointerID,
	                                                                  _pos));
}

void gale::Context::OS_SetMouseState(int _pointerID, bool _isDown, const vec2& _pos ) {
	m_msgSystem.post(std::make_shared<gale::context::LoopActionInput>(gale::key::type_mouse,
	                                                                  (_isDown==true?gale::key::status_down:gale::key::status_up),
	                                                                  _pointerID,
	                                                                  _pos));
}

void gale::Context::OS_SetKeyboard(gale::key::Special& _special,
                                   char32_t _myChar,
                                   bool _isDown,
                                   bool _isARepeateKey) {
	enum gale::key::status state = _isDown==true?gale::key::status_down:gale::key::status_up;
	if (_isARepeateKey == true) {
		if (state == gale::key::status_down) {
			state = gale::key::status_downRepeate;
		} else {
			state = gale::key::status_upRepeate;
		}
	}
	m_msgSystem.post(std::make_shared<gale::context::LoopActionKeyboard>(_special
	                                                                     gale::key::keyboard_char,
	                                                                     state,
	                                                                     _myChar));
}

void gale::Context::OS_SetKeyboardMove(gale::key::Special& _special,
                                       enum gale::key::keyboard _move,
                                       bool _isDown,
                                       bool _isARepeateKey) {
	gale::key::status state = _isDown==true?gale::key::status_down:gale::key::status_up;
	if (_isARepeateKey == true) {
		if (state == gale::key::status_down) {
			state = gale::key::status_downRepeate;
		} else {
			state = gale::key::status_upRepeate;
		}
	}
	m_msgSystem.post(std::make_shared<gale::context::LoopActionKeyboard>(_special
	                                                                     _move,
	                                                                     state));
}

void gale::Context::OS_Hide() {
	m_msgSystem.post(std::make_shared<gale::context::LoopActionView>(false));
}

void gale::Context::OS_Show() {
	m_msgSystem.post(std::make_shared<gale::context::LoopActionView>(true));
}


void gale::Context::OS_ClipBoardArrive(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgClipboardArrive;
	data->clipboardID = _clipboardID;
	m_msgSystem.post(data);
}

void gale::Context::clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	// just transmit an event , we have the data in the system
	OS_ClipBoardArrive(_clipboardID);
}

void gale::Context::clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	// nothing to do, data is already copyed in the GALE clipborad center
}

bool gale::Context::OS_Draw(bool _displayEveryTime) {
	int64_t currentTime = gale::getTime();
	// this is to prevent the multiple display at the a high frequency ...
	#if (!defined(__TARGET_OS__Android) && !defined(__TARGET_OS__Windows))
	if(currentTime - m_previousDisplayTime < 1000000/120) {
		usleep(1000);
		return false;
	}
	#endif
	m_previousDisplayTime = currentTime;
	
	// process the events
	if (m_displayFps == true) {
		m_FpsSystemEvent.tic();
	}
	bool needRedraw = false;
	//! Event management section ...
	{
		// set the curent interface :
		lockContext();
		processEvents();
		// call all the application for periodic request (the application manage multiple instance ...
		if (m_application != nullptr) {
			m_application->onPeriod(currentTime);
		}
		if (m_application != nullptr) {
			// Redraw all needed elements
			m_application->onRegenerateDisplay(*this);
			needRedraw = m_application->isDrawingNeeded();
		}
		if (m_displayFps == true) {
			m_FpsSystemEvent.incrementCounter();
			m_FpsSystemEvent.toc();
		}
		// release the curent interface :
		unLockContext();
	}
	bool hasDisplayDone = false;
	//! drawing section:
	{
		// Lock openGl context:
		gale::openGL::lock();
		if (m_displayFps == true) {
			m_FpsSystemContext.tic();
		}
		if(    needRedraw == true
		    || _displayEveryTime == true) {
			m_resourceManager.updateContext();
			if (m_displayFps == true) {
				m_FpsSystemContext.incrementCounter();
			}
		}
		if (m_displayFps == true) {
			m_FpsSystemContext.toc();
			m_FpsSystem.tic();
		}
		if (m_application != nullptr) {
			if(    true == needRedraw
			    || true == _displayEveryTime) {
				m_FpsSystem.incrementCounter();
				// set the curent interface :
				lockContext();
				m_application->onDraw(*this);
				unLockContext();
				hasDisplayDone = true;
			}
		}
		if (m_displayFps == true) {
			m_FpsSystem.toc();
			m_FpsFlush.tic();
		}
		if (hasDisplayDone == true) {
			if (m_displayFps == true) {
				m_FpsFlush.incrementCounter();
			}
			gale::openGL::flush();
		}
		if (m_displayFps == true) {
			m_FpsFlush.toc();
		}
		// release open GL Context
		gale::openGL::unLock();
	}
	if (m_displayFps == true) {
		m_FpsSystemEvent.draw();
		m_FpsSystemContext.draw();
		m_FpsSystem.draw();
		m_FpsFlush.draw();
	}
	{
		// set the curent interface :
		lockContext();
		// release open GL Context
		gale::openGL::lock();
		// while The Gui is drawing in OpenGl, we do some not realTime things
		m_resourceManager.updateContext();
		// release open GL Context
		gale::openGL::unLock();
		// TODO : m_objectManager.cleanInternalRemoved();
		m_resourceManager.cleanInternalRemoved();
		// release the curent interface :
		unLockContext();
	}
	return hasDisplayDone;
}

void gale::Context::resetIOEvent() {
	// TODO : m_input.newLayerSet();
}

void gale::Context::OS_OpenGlContextDestroy() {
	m_resourceManager.contextHasBeenDestroyed();
}

void gale::Context::forceRedrawAll() {
	if (m_application == nullptr) {
		return;
	}
	m_application->onResize(m_windowsSize);
}

void gale::Context::OS_Stop() {
	// set the curent interface :
	lockContext();
	GALE_INFO("OS_Stop...");
	#if 0
	if (m_windowsCurrent != nullptr) {
		m_windowsCurrent->sysOnKill();
	}
	#endif
	// release the curent interface :
	unLockContext();
}

void gale::Context::OS_Suspend() {
	// set the curent interface :
	lockContext();
	GALE_INFO("OS_Suspend...");
	m_previousDisplayTime = -1;
	#if 0
	if (m_windowsCurrent != nullptr) {
		m_windowsCurrent->onStateSuspend();
	}
	#endif
	// release the curent interface :
	unLockContext();
}

void gale::Context::OS_Resume() {
	// set the curent interface :
	lockContext();
	GALE_INFO("OS_Resume...");
	m_previousDisplayTime = gale::getTime();
	// TODO : m_objectManager.timeCallResume(m_previousDisplayTime);
	#if 0
	if (m_windowsCurrent != nullptr) {
		m_windowsCurrent->onStateResume();
	}
	#endif
	// release the curent interface :
	unLockContext();
}
void gale::Context::OS_Foreground() {
	// set the curent interface :
	lockContext();
	GALE_INFO("OS_Foreground...");
	#if 0
	if (m_windowsCurrent != nullptr) {
		m_windowsCurrent->onStateForeground();
	}
	#endif
	// release the curent interface :
	unLockContext();
}

void gale::Context::OS_Background() {
	// set the curent interface :
	lockContext();
	GALE_INFO("OS_Background...");
	#if 0
	if (m_windowsCurrent != nullptr) {
		m_windowsCurrent->onStateBackground();
	}
	#endif
	// release the curent interface :
	unLockContext();
}


void gale::Context::stop() {
	
}

void gale::Context::setSize(const vec2& _size) {
	GALE_INFO("setSize: NOT implemented ...");
};

void gale::Context::setPos(const vec2& _pos) {
	GALE_INFO("setPos: NOT implemented ...");
}

void gale::Context::hide() {
	GALE_INFO("hide: NOT implemented ...");
};

void gale::Context::show() {
	GALE_INFO("show: NOT implemented ...");
}

void gale::Context::setTitle(const std::string& _title) {
	GALE_INFO("setTitle: NOT implemented ...");
}

void gale::Context::keyboardShow() {
	GALE_INFO("keyboardShow: NOT implemented ...");
}


void gale::Context::keyboardHide() {
	GALE_INFO("keyboardHide: NOT implemented ...");
}

#if 0
bool gale::Context::systemKeyboradEvent(enum gale::key::keyboardSystem _key, bool _down) {
	if (m_windowsCurrent == nullptr) {
		return false;
	}
	lockContext();
	bool ret = m_windowsCurrent->onEventHardwareInput(_key, _down);
	unLockContext();
	return ret;
}
#endif
