/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <gale/debug.h>

namespace gale {
	namespace context {
		class CommandLine {
			private:
				std::vector<std::string> m_listArgs; //!< list of all argument parsed
			public:
				/**
				 * @brief Parse the command line parameters
				 */
				void parse(int32_t _argc, const char* _argv[]);
				/**
				 * @brief get the number of element in the Command line
				 * @return the number of element
				 */
				int32_t size();
				/**
				 * @brief get an element with a specific ID
				 * @return _id The cmdLine Id element
				 */
				const std::string& get(int32_t _id);
				/**
				 * @brief add one element at the Command line
				 * @param[in] _newElement String in the input that might be added.
				 */
				void add(const std::string& _newElement);
				/**
				 * @brief remove an element
				 * @param[in] _id Id of the element
				 */
				void remove(int32_t _id);
		};
	}
}

