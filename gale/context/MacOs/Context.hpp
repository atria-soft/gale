/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <gale/key/key.hpp>
#include <ememory/SharedPtr.hpp>
#include <gale/context/Context.hpp>

namespace MacOs	{
	// return true if a flush is needed
	bool draw(bool _displayEveryTime);
	/**
	 * @brief The OS inform that the current windows has change his size.
	 * @param[in] _size new size of the windows.
	 */
	void resize(float _x, float _y);
	void setMouseState(int32_t _id, bool _isDown, float _x, float _y);
	void setMouseMotion(int32_t _id, float _x, float _y);
	void setKeyboard(gale::key::Special _keyboardMode, int32_t _unichar, bool _isDown, bool _isAReapeateKey);
	void setKeyboardMove(gale::key::Special& _keyboardMode, enum gale::key::keyboard _move, bool _isDown, bool _isAReapeateKey);
	void stopRequested();
	/*
	void setRedrawCallback(const std::function<void()>& _func);
	*/
}


namespace gale {
	namespace context {
		namespace macos {
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
