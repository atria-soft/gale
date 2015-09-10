/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <gale/gale.h>
#include <gale/context/Context.h>

#include <gale/context/commandLine.h>
#include <etk/os/FSNode.h>
#include <gale/Dimension.h>

#undef __class__
#define __class__ "gale"
#ifndef GALE_VERSION
#define GALE_VERSION "0.0.0"
#endif

std::string gale::getCompilationMode() {
	#ifdef MODE_RELEASE
		return "Release";
	#else
		return "Debug";
	#endif
}

std::string gale::getBoardType() {
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

std::string gale::getVersion() {
	return GALE_VERSION;
}

