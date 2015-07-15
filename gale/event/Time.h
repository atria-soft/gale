/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_EVENT_CALL_TIME_H__
#define __GALE_EVENT_CALL_TIME_H__

#include <etk/types.h>

namespace gale {
	namespace event {
		class Time {
			private:
				int64_t m_timeSystem; //!< Current system time (micro-second)
				int64_t m_timeUpAppl; //!< Current application wake up-time (micro-second)
				float m_timeDelta; //!< Time from the last cycle call of the system (main appl tick) (second)
				float m_timeDeltaCall; //!< Time from the last call (when we can manage periodic call with specifying periode) (second)
			public:
				Time(int64_t _timeSystem,
				     int64_t _timeUpAppl,
				     float _timeDelta,
				     float _timeDeltaCall) :
				  m_timeSystem(_timeSystem),
				  m_timeUpAppl(_timeUpAppl),
				  m_timeDelta(_timeDelta),
				  m_timeDeltaCall(_timeDeltaCall){
					
				};
			public:
				void setTime(int64_t _timeSystem) {
					m_timeSystem=_timeSystem;
				};
				inline int64_t getTime() const {
					return m_timeSystem;
				};
				void setApplWakeUpTime(int64_t _timeUpAppl) {
					m_timeUpAppl=_timeUpAppl;
				};
				inline int64_t getApplWakeUpTime() const {
					return m_timeUpAppl;
				};
				inline int64_t getApplUpTime() const {
					return m_timeSystem-m_timeUpAppl;
				};
				void setDelta(float _timeDelta) {
					m_timeDelta=_timeDelta;
				};
				inline float getDelta() const {
					return m_timeDelta;
				};
				void setDeltaCall(float _timeDeltaCall) {
					m_timeDeltaCall=_timeDeltaCall;
				};
				inline float getDeltaCall() const {
					return m_timeDeltaCall;
				};
		};
		std::ostream& operator <<(std::ostream& _os, const gale::event::Time& _obj);
	};
};

#endif

