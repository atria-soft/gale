/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_EVENT_ENTRY_H__
#define __GALE_EVENT_ENTRY_H__

#include <etk/types.h>
#include <gale/key/key.h>

namespace gale {
	namespace event {
		class Entry {
			private:
				enum gale::key::keyboard m_type; //!< type of hardware event
				enum gale::key::status m_status; //!< status of hardware event
				gale::key::Special m_specialKey; //!< input key status (prevent change in time..)
				char32_t m_unicodeData; //!< Unicode data (in some case)
			public:
				Entry(enum gale::key::keyboard _type,
				      enum gale::key::status _status,
				      gale::key::Special _specialKey,
				      char32_t _char) :
				  m_type(_type),
				  m_status(_status),
				  m_specialKey(_specialKey),
				  m_unicodeData(_char) {
					
				};
				void setType(enum gale::key::keyboard _type) {
					m_type = _type;
				};
				inline const enum gale::key::keyboard& getType() const {
					return m_type;
				};
				void setStatus(enum gale::key::status _status) {
					m_status = _status;
				};
				inline const enum gale::key::status& getStatus() const {
					return m_status;
				};
				void setSpecialKey(const gale::key::Special& _specialKey) {
					m_specialKey = _specialKey;
				};
				inline const gale::key::Special& getSpecialKey() const {
					return m_specialKey;
				};
				void setChar(char32_t _char) {
					m_unicodeData = _char;
				};
				inline const char32_t& getChar() const {
					return m_unicodeData;
				};
		};
		std::ostream& operator <<(std::ostream& _os, const gale::event::Entry& _obj);
		
		class EntrySystem {
			public:
				EntrySystem(enum gale::key::keyboard _type,
				            enum gale::key::status _status,
				            gale::key::Special _specialKey,
				            char32_t _char) :
				  m_event(_type, _status, _specialKey, _char) {
					
				};
				gale::event::Entry m_event;
		};
		std::ostream& operator <<(std::ostream& _os, const gale::event::EntrySystem& _obj);
	};
};

#endif

