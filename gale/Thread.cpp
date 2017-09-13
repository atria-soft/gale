/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#include <gale/Thread.hpp>
#include <gale/debug.hpp>

#include <gale/context/Context.hpp>

gale::Thread::Thread() :
  m_state(state::stop),
  m_context(0) {
	GALE_INFO("Create new Thread");
}

gale::Thread::~Thread() {
	GALE_INFO("Remove Thread [START]");
	stop();
	GALE_INFO("Remove Thread [STOP]");
}

void gale::Thread::start() {
	if (m_state == state::stop) {
		GALE_DEBUG("Allocate ethread::Thread [START]");
		m_state = state::starting;
		m_context = &gale::getContext();
		m_thread = ememory::makeShared<ethread::Thread>([=](){this->threadCall();}, "galeThread");
		if (m_thread == nullptr) {
			GALE_ERROR("Can not create thread ...");
			return;
		}
		GALE_DEBUG("Allocate ethread::Thread [Set priority]");
		// set priority
		
		GALE_DEBUG("Allocate ethread::Thread [Register context]");
		// set association with the gale context ...
		//gale::contextRegisterThread(m_thread);
		
		GALE_DEBUG("Allocate ethread::Thread [set State]");
		m_state = state::running;
		GALE_DEBUG("Allocate ethread::Thread [STOP]");
	}
}

void gale::Thread::stop() {
	if (m_state == state::stop) {
		return;
	}
	// Request thread stop ==> other wise, we waint indefinitly ...
	m_state = state::stopping;
	while (    m_state == state::running
	        || m_state == state::starting) {
		// requesting a stop ...
		GALE_INFO("wait Thread stopping");
		ethread::sleepMilliSeconds((100));
	}
	GALE_DEBUG("stop ethread::Thread [START]");
	m_thread->join();
	//gale::contextUnRegisterThread(m_thread);
	GALE_DEBUG("stop ethread::Thread [delete]");
	m_thread.reset();
	GALE_DEBUG("stop ethread::Thread [set state]");
	m_state = state::stop;
	GALE_INFO("stop ethread::Thread [STOP]");
}

void gale::Thread::threadCall() {
	GALE_DEBUG("THREAD MAIN [START]");
	if (m_name != "") {
		ethread::setName(m_name);
		m_lastUpdatateName = echrono::Steady::now();
	}
	gale::setContext(m_context);
	while (m_state != state::stopping) {
		if (m_state == state::starting) {
			GALE_DEBUG("run ethread::Thread [NOTHING to do]");
			ethread::sleepMilliSeconds((1));
			continue;
		}
		if (m_name != "") {
			if ((echrono::Steady::now()-m_lastUpdatateName) > echrono::seconds(10)) {
				m_lastUpdatateName = echrono::Steady::now();
				ethread::setName(m_name);
			}
		}
		if (onThreadCall() == true) {
			GALE_DEBUG("run ethread::Thread [AUTO STOP]");
			m_state = state::stopping;
			return;
		}
	}
	GALE_DEBUG("THREAD MAIN [STOP]");
	gale::setContext(nullptr);
	m_state = state::stopping;
}

bool gale::Thread::onThreadCall() {
	return true;
};

enum gale::Thread::state gale::Thread::getState() {
	return m_state;
}

void gale::Thread::setName(etk::String _name) {
	m_name = _name;
}
