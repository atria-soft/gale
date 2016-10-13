/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <gale/Application.hpp>

namespace gale {
	/**
	 * @brief This is the only one things the User might done in his main();
	 * @note To answare you before you ask the question, this is really simple:
	 *       Due to the fect that the current system is multiple-platform, you "main"
	 *       Does not exist in the android platform, then gale call other start 
	 *       and stop function, to permit to have only one code
	 * @note The main can not be in the gale, due to the fact thet is an librairy
	 * @param[in] _application just created instance of the applicationo
	 * @param[in] _argc Standard argc
	 * @param[in] _argv Standard argv
	 * @return normal error int for the application error management
	 */
	int32_t run(gale::Application* _application, int32_t _argc = 0, const char* _argv[] = NULL);
	/**
	 * @brief get GALE version
	 * @return The string that describe gale version
	 */
	std::string getVersion();
	/**
	 * @brief get current time in us...
	 * @return The current time
	 * @deprecated use std::chrono
	 */
	int64_t getTime();
	/**
	 * @brief get compilation mode (release/debug)
	 * @return the string of the mode of commpilation
	 */
	std::string getCompilationMode();
	/**
	 * @brief get the board type (Android/Linux/MacOs/...)
	 * @return the string of the mode of commpilation
	 */
	std::string getBoardType();
}

