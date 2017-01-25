/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

namespace gale {
	namespace context {
		namespace windows {
			/**
			 * @brief Check if the backend can be instanciate
			 * @return true if the backend is availlable (false otherwise)
			 */
			bool isBackendPresent();
			/**
			 * @brief Create the context main intance
			 * @param[in] _application pointer on the application
			 * @param[in] _argc number of parameter
			 * @param[in] _argv araay of pointer of each parameters
			 * @return instance on the generic created context
			 */
			ememory::SharedPtr<gale::Context> createInstance(gale::Application* _application, int _argc, const char *_argv[]);
		}
	}
}
