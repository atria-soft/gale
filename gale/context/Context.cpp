/** @file
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
#include <ethread/tools.h>
#include <mutex>

#include <gale/gale.h>
#include <gale/Dimension.h>
#include <gale/debug.h>

#include <gale/renderer/openGL/openGL.h>
#include <gale/context/Context.h>
#include <gale/resource/Manager.h>
#include <map>




/**
 * @brief get the main gale mutex (event or periodic call mutex).
 * @note due ti the fact that the system can be called for multiple instance, for naw we just limit the acces to one process at a time.
 * @return the main inteface Mutex
 */
static std::mutex& mutexInterface() {
	static  std::mutex s_interfaceMutex;
	return s_interfaceMutex;
}


static std::mutex g_lockContextMap;
static std::map<std::thread::id, gale::Context*>& getContextList() {
	static std::map<std::thread::id, gale::Context*> g_val;
	return g_val;
}

gale::Context& gale::getContext() {
	std::map<std::thread::id, gale::Context*>& list = getContextList();
	g_lockContextMap.lock();
	std::map<std::thread::id, gale::Context*>::iterator it = list.find(std::this_thread::get_id());
	gale::Context* out = nullptr;
	if (it != list.end()) {
		out = it->second;
	}
	
	g_lockContextMap.unlock();
	#if DEBUG_LEVEL > 2
		if(out ==nullptr){
			GALE_CRITICAL("[CRITICAL] try acces at an empty interface");
		}
	#endif
	return *out;
}

void gale::setContext(gale::Context* _context) {
	std::map<std::thread::id, gale::Context*>& list = getContextList();
	//GALE_ERROR("Set context : " << std::this_thread::get_id() << " context pointer : " << uint64_t(_context));
	g_lockContextMap.lock();
	std::map<std::thread::id, gale::Context*>::iterator it = list.find(std::this_thread::get_id());
	if (it == list.end()) {
		list.insert(std::pair<std::thread::id, gale::Context*>(std::this_thread::get_id(), _context));
	} else {
		it->second = _context;
	}
	g_lockContextMap.unlock();
}

void gale::contextRegisterThread(std::thread* _thread) {
	if (_thread == nullptr) {
		return;
	}
	gale::Context* context = &gale::getContext();
	std::map<std::thread::id, gale::Context*>& list = getContextList();
	//GALE_ERROR("REGISTER Thread : " << _thread->get_id() << " context pointer : " << uint64_t(context));
	g_lockContextMap.lock();
	std::map<std::thread::id, gale::Context*>::iterator it = list.find(_thread->get_id());
	if (it == list.end()) {
		list.insert(std::pair<std::thread::id, gale::Context*>(_thread->get_id(), context));
	} else {
		it->second = context;
	}
	g_lockContextMap.unlock();
}

void gale::contextUnRegisterThread(std::thread* _thread) {
	if (_thread == nullptr) {
		return;
	}
	std::map<std::thread::id, gale::Context*>& list = getContextList();
	g_lockContextMap.lock();
	std::map<std::thread::id, gale::Context*>::iterator it = list.find(_thread->get_id());
	if (it != list.end()) {
		list.erase(it);
	}
	g_lockContextMap.unlock();
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
	setContext(this);
}

/**
 * @brief set the curent interface at nullptr.
 * @note this un-lock the main mutex
 */
void gale::Context::unLockContext() {
	setContext(nullptr);
	mutexInterface().unlock();
}

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
	while (m_msgSystem.count()>0) {
		nbEvent++;
		std::function<void(gale::Context& _context)> func;
		m_msgSystem.wait(func);
		if (func == nullptr) {
			continue;
		}
		func(*this);
	}
}

void gale::Context::setArchiveDir(int _mode, const char* _str, const char* _applName) {
	switch(_mode) {
		case 0:
			GALE_DEBUG("Directory APK : path=" << _str);
			etk::setBaseFolderData(_str, _applName);
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
  m_application(_application),
  m_imulationActive(false),
  m_simulationFile("gale.gsim"),
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
  m_windowsSize(320,480) {
	// set a basic 
	ethread::setName("galeThread");
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
			m_simulationFile.setName(std::string(m_commandLine.get(iii).begin()+23, m_commandLine.get(iii).end()) );
		} else if (etk::start_with(m_commandLine.get(iii), "--gale-simulation-mode=") == true) {
			m_imulationActive = true;
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
			GALE_PRINT("                Enable the simulation mode of the gale IO, parameter: file (default:simulation gale.gsim)");
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
	// TODO : remove this ...
	etk::initDefaultFolder("galeApplNoName");
	// request the init of the application in the main context of openGL ...
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":INIT");
		m_simulationFile.filePuts("\n");
	}
	m_msgSystem.post([](gale::Context& _context){
		std::shared_ptr<gale::Application> appl = _context.getApplication();
		if (appl == nullptr) {
			return;
		}
		appl->onCreate(_context);
		appl->onStart(_context);
		appl->onResume(_context);
	});
	
	// force a recalculation
	requestUpdateSize();
	#if defined(__GALE_ANDROID_ORIENTATION_LANDSCAPE__)
		forceOrientation(gale::orientation_screenLandscape);
	#elif defined(__GALE_ANDROID_ORIENTATION_PORTRAIT__)
		forceOrientation(gale::orientation_screenPortrait);
	#else
		forceOrientation(gale::orientation_screenAuto);
	#endif
	if (m_imulationActive == true) {
		// in simulation case:
		m_simulationFile.fileOpenWrite();
	}
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
	if (m_imulationActive == true) {
		// in simulation case:
		m_simulationFile.fileClose();
	}
}

void gale::Context::requestUpdateSize() {
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":RECALCULATE_SIZE\n");
	}
	m_msgSystem.post([](gale::Context& _context){
		//GALE_DEBUG("Receive MSG : THREAD_RESIZE");
		_context.forceRedrawAll();
	});
}

void gale::Context::OS_Resize(const vec2& _size) {
	// TODO : Better in the thread ...  ==> but generate some init error ...
	gale::Dimension::setPixelWindowsSize(_size);
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":RESIZE:");
		m_simulationFile.filePuts(etk::to_string(_size));
		m_simulationFile.filePuts("\n");
	}
	m_msgSystem.post([_size](gale::Context& _context){
		//GALE_DEBUG("Receive MSG : THREAD_RESIZE");
		_context.m_windowsSize = _size;
		gale::Dimension::setPixelWindowsSize(_context.m_windowsSize);
		_context.forceRedrawAll();
	});
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

void gale::Context::OS_SetInput(enum gale::key::type _type,
                                enum gale::key::status _status,
                                int32_t _pointerID,
                                const vec2& _pos ) {
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":INPUT:");
		m_simulationFile.filePuts(etk::to_string(_type));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::to_string(_status));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::to_string(_pointerID));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::to_string(_pos));
		m_simulationFile.filePuts("\n");
	}
	m_msgSystem.post([_type, _status, _pointerID, _pos](gale::Context& _context){
		std::shared_ptr<gale::Application> appl = _context.getApplication();
		if (appl == nullptr) {
			return;
		}
		appl->onPointer(_type,
		                _pointerID,
		                _pos,
		                _status);
	});
}

void gale::Context::OS_setKeyboard(const gale::key::Special& _special,
                                   enum gale::key::keyboard _type,
                                   enum gale::key::status _state,
                                   bool _isARepeateKey,
                                   char32_t _char) {
	if (_isARepeateKey == true) {
		if (_state == gale::key::status_down) {
			_state = gale::key::status_downRepeate;
		} else {
			_state = gale::key::status_upRepeate;
		}
	}
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":KEYBOARD:");
		m_simulationFile.filePuts(etk::to_string(_special));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::to_string(_type));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::to_string(_state));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::to_string(uint64_t(_char)));
		m_simulationFile.filePuts("\n");
	}
	m_msgSystem.post([_special, _type, _state, _char](gale::Context& _context){
		std::shared_ptr<gale::Application> appl = _context.getApplication();
		if (appl == nullptr) {
			return;
		}
		appl->onKeyboard(_special,
		                 _type,
		                 _char,
		                 _state);
	});
}

void gale::Context::OS_Hide() {
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":VIEW:false\n");
	}
	m_msgSystem.post([](gale::Context& _context){
		/*
		std::shared_ptr<gale::Application> appl = _context.getApplication();
		if (appl == nullptr) {
			return;
		}
		appl->onKeyboard(_special,
		                 _type,
		                 _char,
		                 _state);
		*/
		GALE_TODO("HIDE ... ");
	});
}

void gale::Context::OS_Show() {
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":VIEW:true\n");
	}
	m_msgSystem.post([](gale::Context& _context){
		/*
		std::shared_ptr<gale::Application> appl = _context.getApplication();
		if (appl == nullptr) {
			return;
		}
		appl->onKeyboard(_special,
		                 _type,
		                 _char,
		                 _state);
		*/
		GALE_TODO("SHOW ... ");
	});
}


void gale::Context::OS_ClipBoardArrive(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	if (m_imulationActive == true) {
		m_simulationFile.filePuts(etk::to_string(gale::getTime()));
		m_simulationFile.filePuts(":CLIPBOARD_ARRIVE:");
		m_simulationFile.filePuts(etk::to_string(_clipboardID));
		m_simulationFile.filePuts("\n");
	}
	m_msgSystem.post([_clipboardID](gale::Context& _context){
		std::shared_ptr<gale::Application> appl = _context.getApplication();
		if (appl != nullptr) {
			appl->onClipboardEvent(_clipboardID);
		}
	});
}

void gale::Context::clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	// just transmit an event , we have the data in the system
	OS_ClipBoardArrive(_clipboardID);
}

void gale::Context::clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	// nothing to do, data is already copyed in the GALE clipborad center
}

bool gale::Context::OS_Draw(bool _displayEveryTime) {
	gale::openGL::threadHasContext();
	int64_t currentTime = gale::getTime();
	// this is to prevent the multiple display at the a high frequency ...
	#if (!defined(__TARGET_OS__Android) && !defined(__TARGET_OS__Windows))
	if(currentTime - m_previousDisplayTime < 1000000/120) {
		usleep(1000);
		gale::openGL::threadHasNoMoreContext();
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
			lockContext();
			m_resourceManager.updateContext();
			unLockContext();
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
	gale::openGL::threadHasNoMoreContext();
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
	if (m_application == nullptr) {
		stop();
		return;
	}
	m_application->onKillDemand(*this);
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

