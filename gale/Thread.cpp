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
	
}

gale::Thread::~Thread() {
	stop();
}

void gale::Thread::start() {
	if (m_state == state_stop) {
		m_state = state_starting;
		m_thread = new std11::thread(&gale::Thread::threadCall, this);
		// set priority
		
		// set association with the gale context ...
		gale::contextRegisterThread(m_thread);
		
		m_state = state_running;
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
	m_thread->join();
	gale::contextUnRegisterThread(m_thread);
	delete m_thread;
	m_thread = nullptr;
	m_state = state_stop;
}

void gale::Thread::threadCall() {
	while (m_state != state_stopping) {
		if (m_state == state_starting) {
			usleep(1000);
			continue;
		}
		if (onThreadCall() == true) {
			m_state = state_stopping;
			return;
		}
	}
	m_state = state_stopping;
}
