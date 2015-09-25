/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_THREAD_H__
#define __GALE_THREAD_H__

#include <etk/types.h>
#include <etk/thread/tools.h>

namespace gale {
	/**
	 * @brief in the dimention class we store the data as the more usefull unit (pixel) 
	 * but one case need to be dynamic the %, then when requested in % the register the % value
	 */
	class Thread {
		private:
			enum state {
				state_stop,
				state_starting,
				state_running,
				state_stopping
			};
			enum state m_state;
			std11::thread* m_thread;
		public:
			/**
			 * @brief Constructor (default :0,0 mode pixel)
			 */
			Thread();
			/**
			 * @brief Destructor
			 */
			virtual ~Thread();
			void start();
			void stop();
		private:
			void threadCall();
		protected:
			virtual bool onThreadCall() { return true; };
	};
};

#endif

