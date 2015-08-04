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
		public:
			enum theadMessage {
				msgNone,
				msgInit,
				msgRecalculateSize,
				msgResize,
				msgHide,
				msgShow,
				
				msgInputMotion,
				msgInputState,
				
				msgKeyboardKey,
				msgKeyboardMove,
				
				msgClipboardArrive
			};
		public :
			// specify the message type
			enum theadMessage TypeMessage;
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
				TypeMessage(msgNone),
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
			case eSystemMessage::msgInit:
				// this is due to the openGL context
				/*bool returnVal = */
				m_application->onCreate(*this);
				m_application->onStart(*this);
				m_application->onResume(*this);
				break;
			case eSystemMessage::msgRecalculateSize:
				forceRedrawAll();
				break;
			case eSystemMessage::msgResize:
				//GALE_DEBUG("Receive MSG : THREAD_RESIZE");
				m_windowsSize = data->dimention;
				gale::Dimension::setPixelWindowsSize(m_windowsSize);
				forceRedrawAll();
				break;
			case eSystemMessage::msgInputMotion:
				//GALE_DEBUG("Receive MSG : THREAD_INPUT_MOTION");
				// TODO : m_input.motion(data->inputType, data->inputId, data->dimention);
				break;
			case eSystemMessage::msgInputState:
				//GALE_DEBUG("Receive MSG : THREAD_INPUT_STATE");
				// TODO : m_input.state(data->inputType, data->inputId, data->stateIsDown, data->dimention);
				break;
			case eSystemMessage::msgKeyboardKey:
			case eSystemMessage::msgKeyboardMove:
				//GALE_DEBUG("Receive MSG : THREAD_KEYBORAD_KEY");
				// store the keyboard special key status for mouse event...
				// TODO : m_input.setLastKeyboardSpecial(data->keyboardSpecial);
				#if 0
				if (nullptr != m_windowsCurrent) {
					if (false == m_windowsCurrent->onEventShortCut(data->keyboardSpecial,
					                                             data->keyboardChar,
					                                             data->keyboardMove,
					                                             data->stateIsDown) ) {
						// get the current focused Widget :
						std::shared_ptr<gale::Widget> tmpWidget = m_widgetManager.focusGet();
						if (nullptr != tmpWidget) {
							// check if the widget allow repeating key events.
							//GALE_DEBUG("repeating test :" << data->repeateKey << " widget=" << tmpWidget->getKeyboardRepeate() << " state=" << data->stateIsDown);
							if(    false == data->repeateKey
							    || (    true == data->repeateKey
							         && true == tmpWidget->getKeyboardRepeate()) ) {
								// check Widget shortcut
								if (false == tmpWidget->onEventShortCut(data->keyboardSpecial,
								                                      data->keyboardChar,
								                                      data->keyboardMove,
								                                      data->stateIsDown) ) {
									// generate the direct event ...
									if (data->TypeMessage == eSystemMessage::msgKeyboardKey) {
										gale::event::EntrySystem tmpEntryEvent(gale::key::keyboardChar,
										                                     gale::key::statusUp,
										                                     data->keyboardSpecial,
										                                     data->keyboardChar);
										if(true == data->stateIsDown) {
											tmpEntryEvent.m_event.setStatus(gale::key::statusDown);
										}
										tmpWidget->systemEventEntry(tmpEntryEvent);
									} else { // THREAD_KEYBORAD_MOVE
										GALE_DEBUG("THREAD_KEYBORAD_MOVE" << data->keyboardMove << " " << data->stateIsDown);
										gale::event::EntrySystem tmpEntryEvent(data->keyboardMove,
										                                     gale::key::statusUp,
										                                     data->keyboardSpecial,
										                                     0);
										if(true == data->stateIsDown) {
											tmpEntryEvent.m_event.setStatus(gale::key::statusDown);
										}
										tmpWidget->systemEventEntry(tmpEntryEvent);
									}
								} else {
									GALE_DEBUG("remove Repeate key ...");
								}
							}
						}
					}
				}
				#endif
				break;
			case eSystemMessage::msgClipboardArrive:
				{
					std::shared_ptr<gale::Application> appl = m_application;
					if (appl != nullptr) {
						appl->onClipboardEvent(data->clipboardID);
					}
				}
				break;
			case eSystemMessage::msgHide:
				GALE_DEBUG("Receive MSG : msgHide");
				//guiAbstraction::SendKeyboardEventMove(tmpData->isDown, tmpData->move);
				//gui_uniqueWindows->SysOnHide();
				break;
			case eSystemMessage::msgShow:
				GALE_DEBUG("Receive MSG : msgShow");
				//guiAbstraction::SendKeyboardEventMove(tmpData->isDown, tmpData->move);
				//gui_uniqueWindows->SysOnShow();
				break;
			default:
				GALE_DEBUG("Receive MSG : UNKNOW");
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
	etk::thread::setName("gale");
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
	for(int32_t iii = 0; iii < m_commandLine.size() ; ++iii) {
		if (m_commandLine.get(iii) == "--gale-fps") {
			m_displayFps=true;
		} else if (    m_commandLine.get(iii) == "-h"
		            || m_commandLine.get(iii) == "--help") {
			GALE_PRINT("gale - help : ");
			GALE_PRINT("    " << etk::getApplicationName() << " [options]");
			GALE_PRINT("        --gale-fps:   Display the current fps of the display");
			GALE_PRINT("        -h/--help:    Display this help");
			GALE_PRINT("    example:");
			GALE_PRINT("        " << etk::getApplicationName() << " --gale-fps");
			// this is a global help system does not remove it
			continue;
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
	{
		gale::eSystemMessage *data = new gale::eSystemMessage();
		if (data == nullptr) {
			GALE_ERROR("allocationerror of message");
		} else {
			data->TypeMessage = eSystemMessage::msgInit;
			m_msgSystem.post(data);
		}
	}
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
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgRecalculateSize;
	m_msgSystem.post(data);
}

void gale::Context::OS_Resize(const vec2& _size) {
	// TODO : Better in the thread ...  == > but generate some init error ...
	gale::Dimension::setPixelWindowsSize(_size);
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgResize;
	data->dimention = _size;
	m_msgSystem.post(data);
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
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgInputMotion;
	data->inputType = gale::key::type_finger;
	data->inputId = _pointerID;
	data->dimention = _pos;
	m_msgSystem.post(data);
}

void gale::Context::OS_SetInputState(int _pointerID, bool _isDown, const vec2& _pos ) {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgInputState;
	data->inputType = gale::key::type_finger;
	data->inputId = _pointerID;
	data->stateIsDown = _isDown;
	data->dimention = _pos;
	m_msgSystem.post(data);
}

void gale::Context::OS_SetMouseMotion(int _pointerID, const vec2& _pos ) {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgInputMotion;
	data->inputType = gale::key::type_mouse;
	data->inputId = _pointerID;
	data->dimention = _pos;
	m_msgSystem.post(data);
}

void gale::Context::OS_SetMouseState(int _pointerID, bool _isDown, const vec2& _pos ) {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgInputState;
	data->inputType = gale::key::type_mouse;
	data->inputId = _pointerID;
	data->stateIsDown = _isDown;
	data->dimention = _pos;
	m_msgSystem.post(data);
}

void gale::Context::OS_SetKeyboard(gale::key::Special& _special,
                                   char32_t _myChar,
                                   bool _isDown,
                                   bool _isARepeateKey) {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgKeyboardKey;
	data->stateIsDown = _isDown;
	data->keyboardChar = _myChar;
	data->keyboardSpecial = _special;
	data->repeateKey = _isARepeateKey;
	m_msgSystem.post(data);
}

void gale::Context::OS_SetKeyboardMove(gale::key::Special& _special,
                                        enum gale::key::keyboard _move,
                                        bool _isDown,
                                        bool _isARepeateKey) {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgKeyboardMove;
	data->stateIsDown = _isDown;
	data->keyboardMove = _move;
	data->keyboardSpecial = _special;
	data->repeateKey = _isARepeateKey;
	m_msgSystem.post(data);
}

void gale::Context::OS_Hide() {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgHide;
	m_msgSystem.post(data);
}

void gale::Context::OS_Show() {
	gale::eSystemMessage *data = new gale::eSystemMessage();
	if (data == nullptr) {
		GALE_ERROR("allocationerror of message");
		return;
	}
	data->TypeMessage = eSystemMessage::msgShow;
	m_msgSystem.post(data);
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
	// TODO : Remove this force at true ...
	_displayEveryTime = true;
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
		// call all the widget that neded to do something periodicly
		// TODO : m_objectManager.timeCall(currentTime);
		// check if the user selected a windows
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
	#if 0
	if (m_windowsCurrent == nullptr) {
		return;
	}
	m_windowsCurrent->calculateSize(vec2(m_windowsSize.x(), m_windowsSize.y()));
	#endif
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
