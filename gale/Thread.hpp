/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <ethread/tools.hpp>
#include <gale/context/Context.hpp>

#if defined(__TARGET_OS__Android)
	#include <pthread.h>
#else
	#include <thread>
#endif

namespace gale {
	/**
	 * @brief in the dimention class we store the data as the more usefull unit (pixel) 
	 * but one case need to be dynamic the %, then when requested in % the register the % value
	 */
	class Thread {
		private:
			enum class state {
				stop,
				starting,
				running,
				stopping
			};
			enum state m_state;
			#if defined(__TARGET_OS__Android)
				pthread_t m_thread;
			#else
				ememory::SharedPtr<std::thread> m_thread;
			#endif
			gale::Context* m_context;
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
			#if defined(__TARGET_OS__Android)
				static void* threadCallback(void* _userData);
			#endif
			void threadCall();
		protected:
			virtual bool onThreadCall() { return true; };
	};
}

