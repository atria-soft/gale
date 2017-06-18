/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once
#include <echrono/Steady.hpp>
#include <echrono/Duration.hpp>

namespace gale {
	namespace context {
		/**
		 * @brief This class is designed to count the number of frame per second in the main renderer system
		 * @not_in_doc
		 */
		class Fps {
			private:
				echrono::Steady m_startTime;
				int64_t m_nbCallTime;
				int64_t m_nbDisplayTime;
				echrono::Duration m_min;
				echrono::Duration m_avg;
				echrono::Duration m_max;
				echrono::Duration m_min_idle;
				echrono::Duration m_avg_idle;
				echrono::Duration m_max_idle;
				echrono::Steady m_ticTime;
				echrono::Steady m_lastDrawTime;
				bool m_display;
				bool m_drawingDone;
				const char * m_displayName;
				bool m_displayFPS;
			public:
				/**
				 * @brief Constructor
				 */
				Fps(const char* _displayName, bool _displayFPS):
				  m_nbCallTime(0),
				  m_nbDisplayTime(0),
				  m_min(99999999,0),
				  m_avg(0,0),
				  m_max(0,0),
				  m_min_idle(99999999,0),
				  m_avg_idle(0,0),
				  m_max_idle(0,0),
				  m_display(false),
				  m_drawingDone(false),
				  m_displayName(_displayName),
				  m_displayFPS(_displayFPS) {
					m_lastDrawTime = echrono::Steady::now();
				}
				/**
				 * @brief Destructor
				 */
				~Fps() {
					
				}
				/**
				 * @brief this might be call every time a diplay start
				 */
				void tic() {
					echrono::Steady currentTime = echrono::Steady::now();
					m_ticTime = currentTime;
					m_nbCallTime++;
					if (m_startTime == echrono::Steady()) {
						m_startTime = currentTime;
					}
					//GALE_DEBUG("current : " << currentTime << "time    diff : " << (currentTime - m_startTime));
					if ( (currentTime - m_startTime) > echrono::seconds(10)) {
						m_display = true;
					}
				}
				/**
				 * @brief this might be call every time a diplay stop, it do the display every second
				 * @param[in] displayTime display curent time of the frame.
				 */
				void toc(bool _displayTime = false) {
					echrono::Steady currentTime = echrono::Steady::now();
					echrono::Duration processTimeLocal = (currentTime - m_ticTime);
					if (_displayTime == true) {
						GALE_PRINT(m_displayName << ": processTime: " << processTimeLocal);
					}
					if (m_drawingDone == true) {
						m_min = std::min(m_min, processTimeLocal);
						m_max = std::max(m_max, processTimeLocal);
						m_avg += processTimeLocal;
						m_drawingDone = false;
					} else {
						m_min_idle = std::min(m_min_idle, processTimeLocal);
						m_max_idle = std::max(m_max_idle, processTimeLocal);
						m_avg_idle += processTimeLocal;
					}
				}
				/**
				 * @brief this might be call when a display is really done
				 */
				void incrementCounter() {
					m_nbDisplayTime++;
					m_drawingDone = true;
				}
				/**
				 * @brief draw debug display ...
				 */
				void draw() {
					if (m_display == true) {
						if (m_nbDisplayTime > 0) {
							GALE_PRINT(m_displayName << " : Active : "
							                         << m_min << " "
							                         << m_avg / m_nbDisplayTime << "ms "
							                         << m_max << " ");
						}
						if (m_nbCallTime-m_nbDisplayTime>0) {
							GALE_PRINT(m_displayName << " : idle   : "
							                         << m_min_idle << " "
							                         << m_avg_idle / (m_nbCallTime-m_nbDisplayTime) << "ms "
							                         << m_max_idle << " ");
						}
						if (m_displayFPS == true) {
							float nbSeconds = (echrono::Steady::now() - m_lastDrawTime).toSeconds();
							m_lastDrawTime = echrono::Steady::now();
							GALE_PRINT("FPS : " << m_nbDisplayTime << "/" << m_nbCallTime << " frames ==> " << (float(m_nbDisplayTime)/nbSeconds) << "fps");
						}
						m_max = echrono::Duration(0);
						m_min = echrono::Duration(99999999,0);
						m_avg = echrono::Duration(0);
						m_max_idle = echrono::Duration(0);
						m_min_idle = echrono::Duration(99999999,0);
						m_avg_idle = echrono::Duration(0);
						m_nbCallTime = 0;
						m_nbDisplayTime = 0;
						m_startTime = echrono::Steady();
						m_display = false;
					}
				}
		};
	}
}

