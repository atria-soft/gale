/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */



#include <etk/types.hpp>
#include <etk/etk.hpp>

#include <etk/tool.hpp>
#include <etk/os/FSNode.hpp>
#include <ethread/tools.hpp>
#include <ethread/Mutex.hpp>

#include <gale/gale.hpp>
#include <gale/Dimension.hpp>
#include <gale/debug.hpp>

#include <gale/renderer/openGL/openGL.hpp>
#include <gale/context/Context.hpp>
#include <gale/resource/Manager.hpp>
#include <etk/Map.hpp>
#include <echrono/Steady.hpp>


/**
 * @brief get the main gale mutex (event or periodic call mutex).
 * @note due ti the fact that the system can be called for multiple instance, for naw we just limit the acces to one process at a time.
 * @return the main inteface Mutex
 */
static ethread::Mutex& mutexInterface() {
	static  ethread::Mutex s_interfaceMutex;
	return s_interfaceMutex;
}


static ethread::Mutex g_lockContextMap;
static etk::Map<ethread::Thread::id, gale::Context*>& getContextList() {
	static etk::Map<ethread::Thread::id, gale::Context*> g_val;
	return g_val;
}

static gale::Context* lastContextSet = nullptr;

gale::Context& gale::getContext() {
	etk::Map<ethread::Thread::id, gale::Context*>& list = getContextList();
	g_lockContextMap.lock();
	etk::Map<ethread::Thread::id, gale::Context*>::Iterator it = list.find(std::this_thread::get_id());
	gale::Context* out = nullptr;
	if (it != list.end()) {
		out = it->second;
	}
	
	g_lockContextMap.unlock();
	if (out == nullptr) {
		for (auto &it2 : list) {
			if (out == nullptr) {
				if (it2.second != nullptr) {
					out = it2.second;
				}
			} else {
				if (it2.second == nullptr) {
					continue;
				} else if (it2.second == out) {
					continue;
				} else {
					GALE_CRITICAL("[CRITICAL] try acces at an empty context interface ==> bad use of library gale ...");
				}
			}
		}
		if (out == nullptr) {
			if (lastContextSet != nullptr) {
				GALE_ERROR("[CRITICAL] try acces at an empty context interface && fallback on the last context SET ==> you must correct yout implementation");
				out = lastContextSet;
			} else {
				GALE_CRITICAL("[CRITICAL] try acces at an empty context interface && No context availlable ...");
			}
		} else {
			GALE_ERROR("[CRITICAL] try acces at an empty context interface && fallback on the only context that exit ==> you must correct yout implementation");
		}
	}
	return *out;
}

void gale::setContext(gale::Context* _context) {
	etk::Map<ethread::Thread::id, gale::Context*>& list = getContextList();
	//GALE_ERROR("Set context : " << std::this_thread::get_id() << " context pointer : " << uint64_t(_context));
	g_lockContextMap.lock();
	if (_context != nullptr) {
		lastContextSet = _context;
	}
	list.set(std::this_thread::get_id(), _context);
	g_lockContextMap.unlock();
}

void gale::contextRegisterThread(ethread::Thread* _thread) {
	if (_thread == nullptr) {
		return;
	}
	gale::Context* context = &gale::getContext();
	etk::Map<ethread::Thread::id, gale::Context*>& list = getContextList();
	//GALE_ERROR("REGISTER Thread : " << _thread->get_id() << " context pointer : " << uint64_t(context));
	g_lockContextMap.lock();
	list.set(_thread->get_id(), context);
	g_lockContextMap.unlock();
}

void gale::contextUnRegisterThread(ethread::Thread* _thread) {
	if (_thread == nullptr) {
		return;
	}
	etk::Map<ethread::Thread::id, gale::Context*>& list = getContextList();
	g_lockContextMap.lock();
	etk::Map<ethread::Thread::id, gale::Context*>::Iterator it = list.find(_thread->get_id());
	if (it != list.end()) {
		list.erase(it);
	}
	g_lockContextMap.unlock();
}

void gale::Context::setInitImage(const etk::String& _fileName) {
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

void gale::Context::processEvents() {
	int32_t nbEvent = 0;
	//GALE_DEBUG(" ********  Event");
	while (m_msgSystem.count()>0) {
		nbEvent++;
		etk::Function<void(gale::Context& _context)> func;
		{
			std::unique_lock<std::recursive_mutex> lock(m_mutex);
			m_msgSystem.wait(func);
		}
		if (func == nullptr) {
			continue;
		}
		func(*this);
	}
}

void gale::Context::setArchiveDir(int _mode, const char* _str, const char* _applName) {
	GALE_INFO("Set archive DIR : " << _mode << " " << _str << " " << _applName << "[BEGIN]");
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
	GALE_INFO("Set archive DIR : " << _mode << " " << _str << " " << _applName << "[ END ]");
}

namespace gale {
	class PeriodicThread : public gale::Thread {
		private:
			gale::Context* m_context;
		public:
			PeriodicThread(gale::Context* _context):
			  m_context(_context) {
				setName("GaleThread 2");
			}
			bool onThreadCall() override {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				#if 0
					m_context->lockContext();
				#else
					mutexInterface().lock();
				#endif
				m_context->processEvents();
				// call all the application for periodic request (the application manage multiple instance )...
				if (m_context->m_application != nullptr) {
					m_context->m_application->onPeriod(echrono::Steady::now());
				}
				#if 0
					m_context->unLockContext();
				#else
					mutexInterface().unlock();
				#endif
				return false;
			}
	};
}




gale::Context::Context(gale::Application* _application, int32_t _argc, const char* _argv[]) :
  m_application(_application),
  m_simulationActive(false),
  m_simulationFile("gale.gsim"),
  //m_objectManager(*this),
  m_previousDisplayTime(),
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
	// set the curent interface:
	lockContext();
	// create thread to manage real periodic event
	m_periodicThread = ememory::makeShared<PeriodicThread>(this);
	
	// By default we set 2 themes (1 color and 1 shape ...) :
	etk::theme::setNameDefault("GUI", "shape/square/");
	etk::theme::setNameDefault("COLOR", "color/black/");
	// parse the debug level:
	for(int32_t iii=0; iii<m_commandLine.size(); ++iii) {
		if (m_commandLine.get(iii) == "--gale-fps") {
			m_displayFps=true;
		} else if (etk::start_with(m_commandLine.get(iii), "--gale-simulation-file=") == true) {
			m_simulationFile.setName(etk::String(m_commandLine.get(iii).begin()+23, m_commandLine.get(iii).end()) );
		} else if (m_commandLine.get(iii) == "--gale-simulation-record") {
			m_simulationActive = true;
		} else if (etk::start_with(m_commandLine.get(iii), "--gale-backend=") == true) {
			// nothing to do ==> parse in the buttom of the current file ...
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
			GALE_PRINT("        --gale-simulation-record");
			GALE_PRINT("                Record the IO in the simulation file");
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				GALE_PRINT("        --gale-disable-opengl");
				GALE_PRINT("                Disable openGL access (availlable in SIMULATION mode)");
			#endif
			GALE_PRINT("        --gale-fps");
			GALE_PRINT("                Display the current fps of the display");
			#if defined(__TARGET_OS__Linux)
				GALE_PRINT("        --gale-backend=XXX");
				GALE_PRINT("                'X11'          For X11 backend (default)");
				GALE_PRINT("                'wayland'      For wayland backend");
				#ifdef GALE_BUILD_SIMULATION
				GALE_PRINT("                'simulation'   For simulation backend");
				#endif
				GALE_PRINT("                can be set with environement variable 'export EWOL_BACKEND=xxx'");
			#endif
			#if defined(__TARGET_OS__Windows)
				GALE_PRINT("        --gale-backend=XXX");
				GALE_PRINT("                'windows'      For windows backend (default)");
				#ifdef GALE_BUILD_SIMULATION
				GALE_PRINT("                'simulation'   For simulation backend");
				#endif
				GALE_PRINT("                can be set with environement variable 'export EWOL_BACKEND=xxx'");
			#endif
			#if defined(__TARGET_OS__MacOs)
				GALE_PRINT("        --gale-backend=XXX");
				GALE_PRINT("                'macos'          For MacOs backend (default)");
				#ifdef GALE_BUILD_SIMULATION
				GALE_PRINT("                'simulation'   For simulation backend");
				#endif
				GALE_PRINT("                can be set with environement variable 'export EWOL_BACKEND=xxx'");
			#endif
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
	// request the init of the application in the main context of openGL ...
	if (m_simulationActive == true) {
		// in simulation case:
		if (m_simulationFile.fileOpenWrite() == false) {
			GALE_CRITICAL("Can not create Simulation file : " << m_simulationFile);
			m_simulationActive = false;
		} else {
			m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
			m_simulationFile.filePuts(":INIT");
			m_simulationFile.filePuts("\n");
		}
	}
	#if defined(__GALE_ANDROID_ORIENTATION_LANDSCAPE__)
		forceOrientation(gale::orientation::screenLandscape);
	#elif defined(__GALE_ANDROID_ORIENTATION_PORTRAIT__)
		forceOrientation(gale::orientation::screenPortrait);
	#else
		forceOrientation(gale::orientation::screenAuto);
	#endif
	
	m_msgSystem.post([](gale::Context& _context){
		ememory::SharedPtr<gale::Application> appl = _context.getApplication();
		if (appl == nullptr) {
			return;
		}
		appl->onCreate(_context);
		appl->onStart(_context);
		appl->onResume(_context);
		appl->m_canDraw = true;
	});
	
	// force a recalculation
	requestUpdateSize();
	// release the curent interface :
	unLockContext();
	GALE_INFO(" == > Gale system init (END)");
}

void gale::Context::start2ndThreadProcessing() {
	// set the curent interface:
	lockContext();
	m_periodicThread->start();
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	// release the curent interface:
	unLockContext();
}

void gale::Context::postAction(etk::Function<void(gale::Context& _context)> _action) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post(_action);
}

gale::Context::~Context() {
	GALE_INFO(" == > Gale system Un-Init (BEGIN)");
	m_periodicThread->stop();
	getResourcesManager().applicationExiting();
	// TODO : Clean the message list ...
	// set the curent interface:
	lockContext();
	// clean all widget and sub widget with their resources:
	//m_objectManager.cleanInternalRemoved();
	// call application to uninit
	m_application->m_canDraw = false;
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
	if (m_simulationActive == true) {
		// in simulation case:
		m_simulationFile.fileClose();
	}
}

void gale::Context::requestUpdateSize() {
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":RECALCULATE_SIZE\n");
	}
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post([](gale::Context& _context){
		//GALE_DEBUG("Receive MSG : THREAD_RESIZE");
		_context.forceRedrawAll();
	});
}

void gale::Context::OS_Resize(const vec2& _size) {
	if (m_windowsSize == _size) {
		return;
	}
	// TODO : Better in the thread ...  ==> but generate some init error ...
	gale::Dimension::setPixelWindowsSize(_size);
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":RESIZE:");
		m_simulationFile.filePuts(etk::toString(_size));
		m_simulationFile.filePuts("\n");
	}
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post([_size](gale::Context& _context){
		GALE_DEBUG("Receive MSG : THREAD_RESIZE : " << _context.m_windowsSize << " ==> " << _size);
		_context.m_windowsSize = _size;
		gale::Dimension::setPixelWindowsSize(_context.m_windowsSize);
		// call application inside ..
		_context.forceRedrawAll();
	});
}

void gale::Context::setSize(const vec2& _size) {
	GALE_INFO("setSize: NOT implemented ...");
};

void gale::Context::setFullScreen(bool _status) {
	GALE_INFO("setFullScreen: NOT implemented ...");
};

void gale::Context::OS_Move(const vec2& _pos) {
	if (m_windowsPos == _pos) {
		return;
	}
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post([_pos](gale::Context& _context){
		GALE_DEBUG("Receive MSG : THREAD_MOVE : " << _context.m_windowsPos << " ==> " << _pos);
		_context.m_windowsPos = _pos;
		ememory::SharedPtr<gale::Application> appl = _context.getApplication();
		if (appl == nullptr) {
			return;
		}
		appl->onMovePosition(_context.m_windowsPos);
	});
}

void gale::Context::setPos(const vec2& _pos) {
	GALE_INFO("setPos: NOT implemented ...");
}

vec2 gale::Context::getPos() {
	return m_windowsPos;
}


void gale::Context::OS_SetInput(enum gale::key::type _type,
                                enum gale::key::status _status,
                                int32_t _pointerID,
                                const vec2& _pos) {
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":INPUT:");
		m_simulationFile.filePuts(etk::toString(_type));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::toString(_status));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::toString(_pointerID));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::toString(_pos));
		m_simulationFile.filePuts("\n");
	}
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post([_type, _status, _pointerID, _pos](gale::Context& _context){
		ememory::SharedPtr<gale::Application> appl = _context.getApplication();
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
		if (_state == gale::key::status::down) {
			_state = gale::key::status::downRepeate;
		} else {
			_state = gale::key::status::upRepeate;
		}
	}
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":KEYBOARD:");
		m_simulationFile.filePuts(etk::toString(_special));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::toString(_type));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::toString(_state));
		m_simulationFile.filePuts(":");
		m_simulationFile.filePuts(etk::toString(uint64_t(_char)));
		m_simulationFile.filePuts("\n");
	}
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post([_special, _type, _state, _char](gale::Context& _context){
		ememory::SharedPtr<gale::Application> appl = _context.getApplication();
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
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":VIEW:false\n");
	}
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post([](gale::Context& _context){
		/*
		ememory::SharedPtr<gale::Application> appl = _context.getApplication();
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
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":VIEW:true\n");
	}
	m_msgSystem.post([](gale::Context& _context){
		/*
		ememory::SharedPtr<gale::Application> appl = _context.getApplication();
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
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":CLIPBOARD_ARRIVE:");
		m_simulationFile.filePuts(etk::toString(_clipboardID));
		m_simulationFile.filePuts("\n");
	}
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_msgSystem.post([_clipboardID](gale::Context& _context){
		ememory::SharedPtr<gale::Application> appl = _context.getApplication();
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
	if (m_simulationActive == true) {
		m_simulationFile.filePuts(etk::toString(echrono::Steady::now()));
		m_simulationFile.filePuts(":DRAW:");
		m_simulationFile.filePuts(etk::toString(_displayEveryTime));
		m_simulationFile.filePuts("\n");
	}
	echrono::Steady currentTime = echrono::Steady::now();
	// TODO : Review this ...
	// this is to prevent the multiple display at the a high frequency ...
	#if (    !defined(__TARGET_OS__Windows) \
	      && !defined(__TARGET_OS__Android))
	if(currentTime - m_previousDisplayTime < echrono::milliseconds(8)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		return false;
	}
	#endif
	m_previousDisplayTime = currentTime;
	
	gale::openGL::threadHasContext();
	
	// process the events
	if (m_displayFps == true) {
		m_FpsSystemEvent.tic();
	}
	bool needRedraw = false;
	//! Event management section ...
	{
		// set the current interface:
		lockContext();
		/*
		Lock the event processing
		
		Wait end of current processing
		
		Display ...
		
		Release the event processing
		
		*/
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
			if(    needRedraw == true
			    || _displayEveryTime == true) {
				m_FpsSystem.incrementCounter();
				// set the curent interface :
				lockContext();
				if (m_application->m_canDraw == true) {
					m_application->onDraw(*this);
				}
				unLockContext();
				hasDisplayDone = true;
			}
		}
		if (m_displayFps == true) {
			m_FpsSystem.toc();
			m_FpsFlush.tic();
		}
		if (hasDisplayDone == true) {
			//GALE_INFO("lklklklklk " << _displayEveryTime);
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
		// set the curent interface:
		lockContext();
		// release open GL Context
		gale::openGL::lock();
		// while The Gui is drawing in OpenGl, we do some not realTime things
		m_resourceManager.updateContext();
		// release open GL Context
		gale::openGL::unLock();
		// TODO : m_objectManager.cleanInternalRemoved();
		m_resourceManager.cleanInternalRemoved();
		// release the curent interface:
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
	m_previousDisplayTime = echrono::Steady();
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
	m_previousDisplayTime = echrono::Steady::now();
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
	GALE_WARNING("stop: NOT implemented for this platform...");
}

void gale::Context::hide() {
	GALE_INFO("hide: NOT implemented ...");
};

void gale::Context::show() {
	GALE_INFO("show: NOT implemented ...");
}

void gale::Context::setTitle(const etk::String& _title) {
	GALE_INFO("setTitle: NOT implemented ...");
}

void gale::Context::keyboardShow() {
	GALE_INFO("keyboardShow: NOT implemented ...");
}


void gale::Context::keyboardHide() {
	GALE_INFO("keyboardHide: NOT implemented ...");
}

// for IOs and Android, this is embended system ==> it is too complex tro wrap it ...
#if    !defined(__TARGET_OS__Android) \
    && !defined(__TARGET_OS__IOs)

#if defined(__TARGET_OS__Linux)
	#ifdef GALE_BUILD_X11
		#include <gale/context/X11/Context.hpp>
	#endif
	#ifdef GALE_BUILD_WAYLAND
		#include <gale/context/wayland/Context.hpp>
	#endif
#elif defined(__TARGET_OS__Windows)
	#include <gale/context/Windows/Context.hpp>
#elif defined(__TARGET_OS__Web)
	#include <gale/context/SDL/Context.hpp>
#elif defined(__TARGET_OS__Android)
	//#include <gale/context/Android/Context.hpp>
#elif defined(__TARGET_OS__IOs)
	//#include <gale/context/IOs/Context.hpp>
#elif defined(__TARGET_OS__MacOs)
	#include <gale/context/MacOs/Context.hpp>
#endif

#ifdef GALE_BUILD_SIMULATION
	#include <gale/context/simulation/Context.hpp>
#endif
/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	ememory::SharedPtr<gale::Context> context;
	etk::String request = "";
	
	// get the environement variable:
	char * basicEnv = getenv("EWOL_BACKEND");
	if (nullptr != basicEnv) {
		etk::String tmpVal = basicEnv;
		//TODO : Check if it leak ...
		#if defined(__TARGET_OS__Linux)
			if (false) { }
			#ifdef GALE_BUILD_X11
			 else if (tmpVal == "X11") {
				request = tmpVal;
			}
			#endif
			#ifdef GALE_BUILD_WAYLAND
			 else if (tmpVal == "wayland") {
				request = tmpVal;
			}
			#endif
			#ifdef GALE_BUILD_SIMULATION
			 else if (tmpVal == "simulation") {
				request = tmpVal;
			}
			#endif
			 else {
				GALE_ERROR("Unsupported environement variable : '" << tmpVal << "' only: ["
				#ifdef GALE_BUILD_X11
					<< "X11"
				#endif
				#ifdef GALE_BUILD_WAYLAND
					<< ",wayland"
				#endif
				#ifdef GALE_BUILD_SIMULATION
					<< ",simulation"
				#endif
					<< "]");
			}
		#elif defined(__TARGET_OS__Windows)
			if (    tmpVal != "windows"
			#ifdef GALE_BUILD_SIMULATION
			     || tmpVal != "simulation"
			#endif
			   ) {
				GALE_ERROR("Unsupported environement variable : '" << tmpVal << "' only: [windows,"
				#ifdef GALE_BUILD_SIMULATION
					<< ",simulation"
				#endif
					<< "]");
			} else {
				request = tmpVal;
			}
		#elif defined(__TARGET_OS__MacOs)
			if (    tmpVal != "macos"
			#ifdef GALE_BUILD_SIMULATION
			     || tmpVal != "simulation"
			#endif
			   ) {
				GALE_ERROR("Unsupported environement variable : '" << tmpVal << "' only: [macos,"
				#ifdef GALE_BUILD_SIMULATION
					<< ",simulation"
				#endif
					<< "]");
			} else {
				request = tmpVal;
			}
		#else
			GALE_ERROR("Unsupported environement variable 'EWOL_BACKEND' in this mode");
		#endif
	}
	for(int32_t iii=0; iii<_argc; ++iii) {
		if (etk::start_with(_argv[iii], "--gale-backend=") == true) {
			etk::String tmpVal = &(_argv[iii][15]);
			#if defined(__TARGET_OS__Linux)
				if (false) { }
				#ifdef GALE_BUILD_X11
				 else if (tmpVal == "X11") {
					request = tmpVal;
				}
				#endif
				#ifdef GALE_BUILD_WAYLAND
				 else if (tmpVal == "wayland") {
					request = tmpVal;
				}
				#endif
				#ifdef GALE_BUILD_SIMULATION
				 else if (tmpVal == "simulation") {
					request = tmpVal;
				}
				#endif
				 else {
					GALE_ERROR("Unsupported environement variable : '" << tmpVal << "' only: ["
					#ifdef GALE_BUILD_X11
						<< "X11"
					#endif
					#ifdef GALE_BUILD_WAYLAND
						<< ",wayland"
					#endif
					#ifdef GALE_BUILD_SIMULATION
						<< ",simulation"
					#endif
						<< "]");
				}
			#elif defined(__TARGET_OS__Windows)
				if (    tmpVal != "windows"
				#ifdef GALE_BUILD_SIMULATION
				     || tmpVal != "simulation"
				#endif
				   ) {
					GALE_ERROR("Unsupported command line input --gale-backend='" << tmpVal << "' only: [windows,"
				#ifdef GALE_BUILD_SIMULATION
					<< ",simulation"
				#endif
					<< "]");
				} else {
					request = tmpVal;
				}
			#elif defined(__TARGET_OS__MacOs)
				if (    tmpVal != "macos"
				#ifdef GALE_BUILD_SIMULATION
				     || tmpVal != "simulation"
				#endif
				   ) {
					GALE_ERROR("Unsupported command line input --gale-backend='" << tmpVal << "' only: [macos,"
				#ifdef GALE_BUILD_SIMULATION
					<< ",simulation"
				#endif
					<< "]");
				} else {
					request = tmpVal;
				}
			#else
				GALE_ERROR("Unsupported environement variable 'EWOL_BACKEND' in this mode");
			#endif
		}
	}
	#if defined(__TARGET_OS__Linux)
		if (request == "") {
			if (false) {}
			#ifdef GALE_BUILD_X11
			 else if (gale::context::x11::isBackendPresent() == true) {
				context = gale::context::x11::createInstance(_application, _argc, _argv);
			}
			#endif
			#ifdef GALE_BUILD_WAYLAND
			 else if (gale::context::wayland::isBackendPresent() == true) {
				context = gale::context::wayland::createInstance(_application, _argc, _argv);
			}
			#endif
			#ifdef GALE_BUILD_SIMULATION
			 else if (gale::context::simulation::isBackendPresent() == true) {
				context = gale::context::simulation::createInstance(_application, _argc, _argv);
			}
			#endif
			 else {
				GALE_CRITICAL("Have no backend to generate display ...");
			}
		}
		#ifdef GALE_BUILD_X11
		 else if (request == "X11") {
			if (gale::context::x11::isBackendPresent() == true) {
				context = gale::context::x11::createInstance(_application, _argc, _argv);
			} else {
				GALE_CRITICAL("Backend 'X11' is not present");
			}
		}
		#endif
		#ifdef GALE_BUILD_WAYLAND
		 else if (request == "wayland") {
			if (gale::context::wayland::isBackendPresent() == true) {
				context = gale::context::wayland::createInstance(_application, _argc, _argv);
			} else {
				GALE_CRITICAL("Backend 'wayland' is not present");
			}
		#endif
		#ifdef GALE_BUILD_SIMULATION
		} else if (request == "simulation") {
			if (gale::context::simulation::isBackendPresent() == true) {
				context = gale::context::simulation::createInstance(_application, _argc, _argv);
			} else {
				GALE_CRITICAL("Backend 'simulation' is not present");
			}
		#endif
		} else {
			GALE_CRITICAL("Must not appear");
		}
	#elif defined(__TARGET_OS__Windows)
		if (request == "") {
			if (gale::context::windows::isBackendPresent() == true) {
				context = gale::context::windows::createInstance(_application, _argc, _argv);
			}
			#ifdef GALE_BUILD_SIMULATION
			 else if (gale::context::simulation::isBackendPresent() == true) {
				context = gale::context::simulation::createInstance(_application, _argc, _argv);
			}
			#endif
			 else {
				GALE_CRITICAL("Have no backend to generate display ...");
			}
		} else if (request == "windows") {
			if (gale::context::windows::isBackendPresent() == true) {
				context = gale::context::windows::createInstance(_application, _argc, _argv);
			} else {
				GALE_CRITICAL("Backend 'windows' is not present");
			}
		}
		#ifdef GALE_BUILD_SIMULATION
		 else if (request == "simulation") {
			if (gale::context::simulation::isBackendPresent() == true) {
				context = gale::context::simulation::createInstance(_application, _argc, _argv);
			} else {
				GALE_CRITICAL("Backend 'simulation' is not present");
			}
		#endif
		} else {
			GALE_CRITICAL("Must not appear");
		}
	#elif defined(__TARGET_OS__Web)
		context = gale::context::web::createInstance(_application, _argc, _argv);
	#elif defined(__TARGET_OS__Android)
		context = gale::context::andoid::createInstance(_application, _argc, _argv);
	#elif defined(__TARGET_OS__IOs)
		context = gale::context::ios::createInstance(_application, _argc, _argv);
	#elif defined(__TARGET_OS__MacOs)
		if (request == "") {
			if (gale::context::macos::isBackendPresent() == true) {
				context = gale::context::macos::createInstance(_application, _argc, _argv);
			}
			#ifdef GALE_BUILD_SIMULATION
			 else if (gale::context::simulation::isBackendPresent() == true) {
				context = gale::context::simulation::createInstance(_application, _argc, _argv);
			}
			#endif
			 else {
				GALE_CRITICAL("Have no backend to generate display ...");
			}
		} else if (request == "macos") {
			if (gale::context::macos::isBackendPresent() == true) {
				context = gale::context::macos::createInstance(_application, _argc, _argv);
			} else {
				GALE_CRITICAL("Backend 'macos' is not present");
			}
		#ifdef GALE_BUILD_SIMULATION
		} else if (request == "simulation") {
			if (gale::context::simulation::isBackendPresent() == true) {
				context = gale::context::simulation::createInstance(_application, _argc, _argv);
			} else {
				GALE_CRITICAL("Backend 'simulation' is not present");
			}
		#endif
		} else {
			GALE_CRITICAL("Must not appear");
		}
	#endif
	
	if (context == nullptr) {
		GALE_ERROR("Can not allocate the interface of the GUI ...");
		return -1;
	}
	return context->run();
}
#endif
