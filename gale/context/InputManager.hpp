/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <gale/Application.hpp>


namespace gale {
	namespace context {
		/**
		 * @brief internal structure
		 * @not_in_doc
		 */
		class InputPoperty {
			public:
				bool isUsed;
				int32_t destinationInputId;
				int64_t lastTimeEvent;
				ememory::WeakPtr<gale::Application> curentApplicationEvent;
				vec2 origin;
				vec2 size;
				vec2 downStart;
				vec2 posEvent;
				bool isDown;
				bool isInside;
				int32_t nbClickEvent; // 0 .. 1 .. 2 .. 3
		};
		
		/**
		 * @brief internal structure
		 * @not_in_doc
		 */
		class InputLimit {
			public:
				int32_t sepatateTime;
				int32_t DpiOffset;
		};
		class Context;
		class InputManager {
			// special grab pointer mode : 
			private:
				ememory::WeakPtr<gale::Application> m_grabApplication; //!< Application that grab the curent pointer.
			private:
				int32_t m_dpi;
				InputLimit m_eventInputLimit;
				InputLimit m_eventMouseLimit;
				void calculateLimit();
				InputPoperty m_eventInputSaved[MAX_MANAGE_INPUT];
				InputPoperty m_eventMouseSaved[MAX_MANAGE_INPUT];
				void abortElement(InputPoperty* _eventTable, int32_t _idInput, enum gale::key::type _type);
				void cleanElement(InputPoperty* _eventTable, int32_t _idInput);
				/**
				 * @brief generate the event on the destinated Application.
				 * @param[in] _type Type of the event that might be sended.
				 * @param[in] _destApplication Pointer on the requested Application that element might be sended
				 * @param[in] _IdInput Id of the event (PC : [0..9] and touch : [1..9])
				 * @param[in] _typeEvent type of the eventg generated
				 * @param[in] _pos position of the event
				 * @return true if event has been greped
				 */
				bool localEventInput(enum gale::key::type _type,
				                     ememory::SharedPtr<gale::Application> _destApplication,
				                     int32_t _IdInput,
				                     enum gale::key::status _typeEvent,
				                     vec2 _pos);
				/**
				 * @brief convert the system event id in the correct GALE id depending of the system management mode
				 *        This function find the next input id unused on the specifiic Application
				 *             == > on PC, the ID does not change (GUI is not the same)
				 * @param[in] _type Type of the kay event.
				 * @param[in] _destApplication Pointer of the Application destination
				 * @param[in] _realInputId system Id
				 * @return the gale input id
				 */
				int32_t localGetDestinationId(enum gale::key::type _type,
				                              ememory::SharedPtr<gale::Application> _destApplication,
				                              int32_t _realInputId);
			private:
				gale::Context& m_context;
			public:
				InputManager(gale::Context& _context);
				~InputManager();
				void setDpi(int32_t _newDPI);
				
				// note if id<0  == > the it was finger event ...
				void motion(enum gale::key::type _type, int _pointerID, vec2 _pos );
				void state(enum gale::key::type _type, int _pointerID, bool _isDown, vec2 _pos);
			private:
				gale::key::Special m_specialKey;
			public:
				void setLastKeyboardSpecial(const gale::key::Special& _specialKey) {
					m_specialKey = _specialKey;
				}
		};
	}
}

