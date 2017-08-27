/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/debug.hpp>
#include <gale/context/commandLine.hpp>
#include <etk/Vector.hpp>

void gale::context::CommandLine::parse(int32_t _argc, const char* _argv[]) {
	for (int32_t i=1 ; i<_argc; i++) {
		GALE_INFO("commandLine : \"" << _argv[i] << "\"" );
		m_listArgs.pushBack(_argv[i]);
	}
}

int32_t gale::context::CommandLine::size() {
	return m_listArgs.size();
}

const etk::String& gale::context::CommandLine::get(int32_t _id) {
	static const etk::String errorArg("");
	if (    _id < 0
	     && _id >= (int64_t)m_listArgs.size()) {
		return errorArg;
	}
	return m_listArgs[_id];
}

void gale::context::CommandLine::add(const etk::String& _newElement) {
	m_listArgs.pushBack(_newElement);
}

void gale::context::CommandLine::remove(int32_t _id) {
	m_listArgs.erase(m_listArgs.begin()+_id);
}

