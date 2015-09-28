/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#include <gale/Thread.h>
#include <gale/debug.h>
#include <unistd.h>
#include <gale/context/Context.h>



#if defined(__TARGET_OS__Android)
	static void* threadCallback(void* _userData) {
		gale::Thread* threadHandle = static_cast<gale::Thread*>(_userData);
		if (threadHandle != nullptr) {
			threadHandle->threadCall();
		}
		return nullptr;
	}
#endif


gale::Thread::Thread() :
  m_state(state_stop),
  #if !defined(__TARGET_OS__Android)
  	m_thread(nullptr),
  #endif
  m_context(nullptr) {
	GALE_INFO("Create new Thread");
}

gale::Thread::~Thread() {
	GALE_INFO("Remove Thread [START]");
	stop();
	GALE_INFO("Remove Thread [STOP]");
}

void gale::Thread::start() {
	if (m_state == state_stop) {
		GALE_DEBUG("Allocate std11::thread [START]");
		m_state = state_starting;
		m_context = &gale::getContext();
		#if defined(__TARGET_OS__Android)
			pthread_create(&m_thread, nullptr, &threadCallback, this);
		#else
			m_thread = new std11::thread(&gale::Thread::threadCall, this);//, &gale::getContext());
			if (m_thread == nullptr) {
				GALE_ERROR("Can not create thread ...");
				return;
			}
		#endif
		//m_thread->detach();
		GALE_DEBUG("Allocate std11::thread [Set priority]");
		// set priority
		
		GALE_DEBUG("Allocate std11::thread [Register context]");
		// set association with the gale context ...
		//gale::contextRegisterThread(m_thread);
		
		GALE_DEBUG("Allocate std11::thread [set State]");
		m_state = state_running;
		GALE_DEBUG("Allocate std11::thread [STOP]");
	}
}

void gale::Thread::stop() {
	if (m_state == state_stop) {
		return;
	}
	while (    m_state == state_running
	        || m_state == state_starting) {
		// requesting a stop ...
		GALE_INFO("wait Thread stopping");
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	GALE_DEBUG("stop std11::thread [START]");
	#if defined(__TARGET_OS__Android)
		//m_thread.join();
	#else
		m_thread->join();
	#endif
	//gale::contextUnRegisterThread(m_thread);
	GALE_DEBUG("stop std11::thread [delete]");
	#if defined(__TARGET_OS__Android)
	
	#else
		delete m_thread;
		m_thread = nullptr;
	#endif
	GALE_DEBUG("stop std11::thread [set state]");
	m_state = state_stop;
	GALE_DEBUG("stop std11::thread [STOP]");
}

void gale::Thread::threadCall() {
	GALE_DEBUG("THREAD MAIN [START]");
	gale::setContext(m_context);
	while (m_state != state_stopping) {
		if (m_state == state_starting) {
			GALE_DEBUG("run std11::thread [NOTHING to do]");
			usleep(1000);
			continue;
		}
		if (onThreadCall() == true) {
			GALE_DEBUG("run std11::thread [AUTO STOP]");
			m_state = state_stopping;
			return;
		}
	}
	GALE_DEBUG("THREAD MAIN [STOP]");
	m_state = state_stopping;
}
