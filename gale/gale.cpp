/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <gale/gale.hpp>
#include <gale/context/Context.hpp>

#include <gale/context/commandLine.hpp>
#include <etk/os/FSNode.hpp>
#include <gale/Dimension.hpp>
#include <chrono>

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

// generic vertion of time: ==> DEPRECATED
int64_t gale::getTime() {
	std::chrono::high_resolution_clock::time_point globalTimePoint = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds timeSinceEpoch = std::chrono::duration_cast<std::chrono::microseconds>(globalTimePoint.time_since_epoch());
	return timeSinceEpoch.count();
}

