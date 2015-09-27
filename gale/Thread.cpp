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

gale::Thread::Thread() :
  m_state(state_stop),
  m_thread(nullptr) {
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
		m_thread = new std11::thread(&gale::Thread::threadCall, this, &gale::getContext());
		if (m_thread == nullptr) {
			GALE_ERROR("Can not create thread ...");
			return;
		}
		m_thread->detach();
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
		usleep(500000);
	}
	GALE_DEBUG("stop std11::thread [START]");
	m_thread->join();
	//gale::contextUnRegisterThread(m_thread);
	GALE_DEBUG("stop std11::thread [delete]");
	delete m_thread;
	m_thread = nullptr;
	GALE_DEBUG("stop std11::thread [set state]");
	m_state = state_stop;
	GALE_DEBUG("stop std11::thread [STOP]");
}

void gale::Thread::threadCall(gale::Context* _context) {
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	GALE_ERROR("THREAD MAIN [START]");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	gale::setContext(_context);
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
