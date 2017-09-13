/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/gale.hpp>
#include <gale/context/Context.hpp>

#include <gale/context/commandLine.hpp>
#include <etk/os/FSNode.hpp>
#include <gale/Dimension.hpp>

#ifndef GALE_VERSION
#define GALE_VERSION "0.0.0"
#endif

etk::String gale::getCompilationMode() {
	#ifdef MODE_RELEASE
		return "Release";
	#else
		return "Debug";
	#endif
}

etk::String gale::getBoardType() {
	#ifdef __TARGET_OS__Linux
		return "Linux";
	#elif defined(__TARGET_OS__Android)
		return "Android";
	#elif defined(__TARGET_OS__Windows)
		return "Windows";
	#elif defined(__TARGET_OS__IOs)
		return "IOs";
	#elif defined(__TARGET_OS__MacOs)
		return "MacOs";
	#else
		return "Unknown";
	#endif
}

etk::String gale::getVersion() {
	return GALE_VERSION;
}

