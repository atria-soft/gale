/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <gale/Thread.h>
#include <gale/debug.h>
#include <unistd.h>
#include <gale/context/Context.h>

#if defined(__TARGET_OS__Android)
	void* gale::Thread::threadCallback(void* _userData) {
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
		GALE_DEBUG("Allocate std::thread [START]");
		m_state = state_starting;
		m_context = &gale::getContext();
		#if defined(__TARGET_OS__Android)
			pthread_create(&m_thread, nullptr, &gale::Thread::threadCallback, this);
		#else
			m_thread = std::make_shared<std::thread>(&gale::Thread::threadCall, this);
			if (m_thread == nullptr) {
				GALE_ERROR("Can not create thread ...");
				return;
			}
		#endif
		//m_thread->detach();
		GALE_DEBUG("Allocate std::thread [Set priority]");
		// set priority
		
		GALE_DEBUG("Allocate std::thread [Register context]");
		// set association with the gale context ...
		//gale::contextRegisterThread(m_thread);
		
		GALE_DEBUG("Allocate std::thread [set State]");
		m_state = state_running;
		GALE_DEBUG("Allocate std::thread [STOP]");
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
	GALE_DEBUG("stop std::thread [START]");
	#if defined(__TARGET_OS__Android)
		void* ret = nullptr;
		int val = pthread_join(m_thread, &ret);
	#else
		m_thread->join();
	#endif
	//gale::contextUnRegisterThread(m_thread);
	GALE_DEBUG("stop std::thread [delete]");
	#if defined(__TARGET_OS__Android)
		
	#else
		m_thread.reset();
	#endif
	GALE_DEBUG("stop std::thread [set state]");
	m_state = state_stop;
	GALE_DEBUG("stop std::thread [STOP]");
}

void gale::Thread::threadCall() {
	GALE_DEBUG("THREAD MAIN [START]");
	gale::setContext(m_context);
	while (m_state != state_stopping) {
		if (m_state == state_starting) {
			GALE_DEBUG("run std::thread [NOTHING to do]");
			usleep(1000);
			continue;
		}
		if (onThreadCall() == true) {
			GALE_DEBUG("run std::thread [AUTO STOP]");
			m_state = state_stopping;
			return;
		}
	}
	GALE_DEBUG("THREAD MAIN [STOP]");
	m_state = state_stopping;
}
