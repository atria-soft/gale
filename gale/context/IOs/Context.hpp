/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <gale/key/key.hpp>

namespace IOs {
	// Create and relaese gale::Context interface:
	void createInterface();
	void releaseInterface();
	// return true if a flush is needed
	bool draw(bool _displayEveryTime);
	/**
	 * @brief The OS inform that the current windows has change his size.
	 * @param[in] _size new size of the windows.
	 */
	void resize(float _x, float _y);
	void setMouseState(int32_t _id, bool _isDown, float _x, float _y);
	void setMouseMotion(int32_t _id, float _x, float _y);
	void setInputState(int32_t _id, bool _isDown, float _x, float _y);
	void setInputMotion(int32_t _id, float _x, float _y);
	void setKeyboard(gale::key::Special _keyboardMode, int32_t _unichar, bool _isDown, bool _isAReapeateKey);
	void setKeyboardMove(gale::key::Special& _keyboardMode, enum gale::key::keyboard _move, bool _isDown);
	void start();
	void stop();
	void foreground();
	void background();
	void resume();
	void suspend();
}

