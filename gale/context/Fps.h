/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

namespace gale {
	namespace context {
		/**
		 * @brief This class is designed to count the number of frame per second in the main renderer system
		 * @not-in-doc
		 */
		class Fps {
			// display every second ...
			#define DISPLAY_PERIODE_US       (1000000)
			private:
				int64_t startTime;
				int64_t nbCallTime;
				int64_t nbDisplayTime;
				int64_t min;
				int64_t avg;
				int64_t max;
				int64_t min_idle;
				int64_t avg_idle;
				int64_t max_idle;
				int64_t ticTime;
				bool display;
				bool drwingDone;
				const char * m_displayName;
				bool m_displayFPS;
			public:
				/**
				 * @brief Constructor
				 */
				Fps(const char * displayName, bool displayFPS) {
					startTime = -1;
					nbCallTime = 0;
					nbDisplayTime = 0;
					min = 99999999999999LL;
					avg = 0;
					max = 0;
					min_idle = 99999999999999LL;
					avg_idle = 0;
					max_idle = 0;
					ticTime = 0;
					display = false;
					drwingDone = false;
					m_displayName = displayName;
					m_displayFPS = displayFPS;
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
					int64_t currentTime = gale::getTime();
					ticTime = currentTime;
					nbCallTime++;
					if (startTime<0) {
						startTime = currentTime;
					}
					//GALE_DEBUG("current : " << currentTime << "time    diff : " << (currentTime - startTime));
					if ( (currentTime - startTime) > DISPLAY_PERIODE_US) {
						display = true;
					}
				}
				/**
				 * @brief this might be call every time a diplay stop, it do the display every second
				 * @param[in] displayTime display curent time of the frame.
				 */
				void toc(bool displayTime = false) {
					int64_t currentTime = gale::getTime();
					int64_t processTimeLocal = (currentTime - ticTime);
					if (displayTime == true) {
						GALE_PRINT(m_displayName << " : processTime : " << (float)((float)processTimeLocal / 1000.0) << "ms ");
					}
					if (drwingDone) {
						min = std::min(min, processTimeLocal);
						max = std::max(max, processTimeLocal);
						avg += processTimeLocal;
						drwingDone = false;
					} else {
						min_idle = std::min(min_idle, processTimeLocal);
						max_idle = std::max(max_idle, processTimeLocal);
						avg_idle += processTimeLocal;
					}
				}
				/**
				 * @brief this might be call when a display is really done
				 */
				void incrementCounter() {
					nbDisplayTime++;
					drwingDone = true;
				}
				/**
				 * @brief draw debug display ...
				 */
				void draw() {
					if (true == display) {
						if (nbDisplayTime>0) {
							GALE_PRINT(m_displayName << " : Active : "
							                         << (float)((float)min / 1000.0) << "ms "
							                         << (float)((float)avg / (float)nbDisplayTime / 1000.0) << "ms "
							                         << (float)((float)max / 1000.0) << "ms ");
						}
						if (nbCallTime-nbDisplayTime>0) {
							GALE_PRINT(m_displayName << " : idle   : "
							                         << (float)((float)min_idle / 1000.0) << "ms "
							                         << (float)((float)avg_idle / (float)(nbCallTime-nbDisplayTime) / 1000.0) << "ms "
							                         << (float)((float)max_idle / 1000.0) << "ms ");
						}
						if (true == m_displayFPS) {
							GALE_PRINT("FPS : " << nbDisplayTime << "/" << nbCallTime << "fps");
						}
						max = 0;
						min = 99999999999999LL;
						avg = 0;
						max_idle = 0;
						min_idle = 99999999999999LL;
						avg_idle = 0;
						nbCallTime = 0;
						nbDisplayTime = 0;
						startTime = -1;
						display = false;
					}
				}
		};
	}
}

