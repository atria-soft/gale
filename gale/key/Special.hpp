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
				 * @brief Set the current Shift key status
				 * @param[in] _value The new Shift value
				 */
				void setShift(bool _value);
				/**
				 * @brief Get the Current Control key status
				 * @return The Control value
				 */
				bool getCtrl() const;
				/**
				 * @brief Set the Current Control key status
				 * @param[in] _value The new Control value
				 */
				void setCtrl(bool _value);
				/**
				 * @brief Get the current Meta key status (also named windows or apple key)
				 * @return The Meta value (name Windows key, apple key, command key ...)
				 */
				bool getMeta() const;
				/**
				 * @brief Set the current Meta key status (also named windows or apple key)
				 * @param[in] _value The new Meta value (name Windows key, apple key, command key ...)
				 */
				void setMeta(bool _value);
				/**
				 * @brief Get the current Alt key status
				 * @return The Alt value
				 */
				bool getAlt() const;
				/**
				 * @brief Set the current Alt key status
				 * @param[in] _value The new Alt value
				 */
				void setAlt(bool _value);
				/**
				 * @brief Get the current Alt-Gr key status
				 * @return The Alt-gr value (does not exist on MacOs)
				 */
				bool getAltGr() const;
				/**
				 * @brief Set the current Alt-Gr key status
				 * @param[in] _value The new Alt-gr value (does not exist on MacOs)
				 */
				void setAltGr(bool _value);
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
				 * @param[in] _isFown The Key is pressed or not.
				 */
				void update(enum gale::key::keyboard _move, bool _isDown);
		};
		std::ostream& operator <<(std::ostream& _os, const gale::key::Special& _obj);
	}
}

