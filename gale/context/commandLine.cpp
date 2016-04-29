/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <gale/debug.h>
#include <gale/context/commandLine.h>
#include <vector>

void gale::context::CommandLine::parse(int32_t _argc, const char* _argv[]) {
	for (int32_t i=1 ; i<_argc; i++) {
		GALE_INFO("commandLine : \"" << _argv[i] << "\"" );
		m_listArgs.push_back(_argv[i]);
	}
}

int32_t gale::context::CommandLine::size() {
	return m_listArgs.size();
}

const std::string& gale::context::CommandLine::get(int32_t _id) {
	static const std::string errorArg("");
	if (    _id < 0
	     && _id >= (int64_t)m_listArgs.size()) {
		return errorArg;
	}
	return m_listArgs[_id];
}

void gale::context::CommandLine::add(const std::string& _newElement) {
	m_listArgs.push_back(_newElement);
}

void gale::context::CommandLine::remove(int32_t _id) {
	m_listArgs.erase(m_listArgs.begin()+_id);
}

