/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <gale/debug.hpp>
#include <gale/key/keyboard.hpp>

namespace gale {
	namespace key {
		/**
		 * @brief This class consider generic special keyborad key (insert, control, shift ...)
		 */
		class Special {
			private:
				uint32_t m_value;
			public:
				/**
				 * @brief Main constructor
				 */
				Special();
				/**
				 * @brief get the current CapLock Status
				 * @return The CapLock value
				 */
				bool getCapsLock() const;
				/**
				 * @brief set the current CapLock Status
				 * @param[in] _value The new CapLock value
				 */
				void setCapsLock(bool _value);
				/**
				 * @brief Get the current Shift key status
				 * @return The Shift value
				 */
				bool getShift() const;
				/**
				 * @brief Get the current Shift left key status
				 * @return The Shift value
				 */
				bool getShiftLeft() const;
				/**
				 * @brief Get the current Shift right key status
				 * @return The Shift value
				 */
				bool getShiftRight() const;
				/**
				 * @brief Set the current Shift left key status
				 * @param[in] _value The new Shift value
				 */
				void setShiftLeft(bool _value);
				/**
				 * @brief Set the current Shift right key status
				 * @param[in] _value The new Shift value
				 */
				void setShiftRight(bool _value);
				/**
				 * @brief Get the Current Control key status
				 * @return The Control value
				 */
				bool getCtrl() const;
				/**
				 * @brief Get the Current Control left key status
				 * @return The Control value
				 */
				bool getCtrlLeft() const;
				/**
				 * @brief Get the Current Control right key status
				 * @return The Control value
				 */
				bool getCtrlRight() const;
				/**
				 * @brief Set the Current Control left key status
				 * @param[in] _value The new Control value
				 */
				void setCtrlLeft(bool _value);
				/**
				 * @brief Set the Current Control right key status
				 * @param[in] _value The new Control value
				 */
				void setCtrlRight(bool _value);
				/**
				 * @brief Get the current Meta key status (also named windows or apple key)
				 * @return The Meta value (name Windows key, apple key, command key ...)
				 */
				bool getMeta() const;
				/**
				 * @brief Get the current Meta left key status (also named windows or apple key)
				 * @return The Meta value (name Windows key, apple key, command key ...)
				 */
				bool getMetaLeft() const;
				/**
				 * @brief Get the current Meta right key status (also named windows or apple key)
				 * @return The Meta value (name Windows key, apple key, command key ...)
				 */
				bool getMetaRight() const;
				/**
				 * @brief Set the current Meta left key status (also named windows or apple key)
				 * @param[in] _value The new Meta value (name Windows key, apple key, command key ...)
				 */
				void setMetaLeft(bool _value);
				/**
				 * @brief Set the current Meta right key status (also named windows or apple key)
				 * @param[in] _value The new Meta value (name Windows key, apple key, command key ...)
				 */
				void setMetaRight(bool _value);
				/**
				 * @brief Get the current Alt key status
				 * @return The Alt value
				 */
				bool getAlt() const;
				/**
				 * @brief Get the current Alt left key status
				 * @return The Alt value
				 */
				bool getAltLeft() const;
				/**
				 * @brief Get the current Alt right key status (alt-gr)
				 * @return The Alt value
				 */
				bool getAltRight() const;
				/**
				 * @brief Set the current Alt left key status
				 * @param[in] _value The new Alt value
				 */
				void setAltLeft(bool _value);
				/**
				 * @brief Set the current Alt right key status (alt-gr)
				 * @param[in] _value The new Alt value
				 */
				void setAltRight(bool _value);
				/**
				 * @brief Get the current Alt-Gr key status
				 * @return The Alt-gr value (does not exist on MacOs)
				 */
				bool getAltGr() const {
					return getAltRight();
				}
				/**
				 * @brief Set the current Alt-Gr key status
				 * @param[in] _value The new Alt-gr value (does not exist on MacOs)
				 */
				void setAltGr(bool _value) {
					setAltRight(_value);
				}
				/**
				 * @brief Get the current Ver-num key status
				 * @return The Numerical Lock value
				 */
				bool getNumLock() const;
				/**
				 * @brief Set the current Ver-num key status
				 * @param[in] _value The new Numerical Lock value
				 */
				void setNumLock(bool _value);
				/**
				 * @brief Get the current Intert key status
				 * @return The Insert value
				 */
				bool getInsert() const;
				/**
				 * @brief Set the current Intert key status
				 * @param[in] _value The new Insert value
				 */
				void setInsert(bool _value);
				/**
				 * @brief Update the internal value with the input moving key.
				 * @param[in] _move Moving key.
				 * @param[in] _isDown The key is pressed or not.
				 */
				void update(enum gale::key::keyboard _move, bool _isDown);
				/**
				 * @brief Get the value with the input moving key.
				 * @param[in] _move Moving key.
				 * @return true The key is pressed.
				 * @return false The key is released.
				 */
				bool get(enum gale::key::keyboard _move);
			protected:
				/**
				 * @brief Set the internal value with the input moving FLAG.
				 * @param[in] _flag Moving Flag.
				 * @param[in] _isDown The key is pressed or not.
				 */
				void setFlag(uint32_t _flag, bool _isDown);
				/**
				 * @brief Get the value with the input moving FLAG.
				 * @param[in] _flag Moving internal FLAG.
				 * @return true The key is pressed.
				 * @return false The key is released.
				 */
				bool getFlag(uint32_t _flag) const;
		};
		etk::Stream& operator <<(etk::Stream& _os, const gale::key::Special& _obj);
	}
}

