/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <ethread/tools.hpp>
#include <gale/context/Context.hpp>

#if defined(__TARGET_OS__Android)
	#include <pthread.h>
#else
	#include <ethread/Thread.hpp>
#endif

namespace gale {
	/**
	 * @brief We have our own thread in gale to manage gale context folowing and manage android rong management of ethread::Thread when calling java.
	 */
	class Thread : public ememory::EnableSharedFromThis<Thread> {
		public:
			/**
			 * @brief Internal state capabilities of the thread.
			 */
			enum class state {
				stop, //!< The thread is not active
				starting, //!< The thread is in creation state
				running, //!< The current thread is activate
				stopping //!< The thread is stoping
			};
		private:
			enum state m_state; //!< Current state of the thread.
		public:
			/**
			 * @brief Get the current state of the thread
			 * @return The state of the thread
			 */
			enum gale::Thread::state getState();
		private:
			#if defined(__TARGET_OS__Android)
				pthread_t m_thread; //!< Current handle on the thread
			#else
				ememory::SharedPtr<ethread::Thread> m_thread; //!< Current handle on the thread
			#endif
			gale::Context* m_context; //!< Copy of the gale context (permit to get current gale interface)
		public:
			/**
			 * @brief Constructor
			 */
			Thread();
			/**
			 * @brief Destructor
			 */
			virtual ~Thread();
			/**
			 * @brief Start the Thread (create thread here)
			 */
			virtual void start();
			/**
			 * @brief Stop the Thread (destroy thread here)
			 */
			virtual void stop();
		private:
			etk::String m_name; //!< thread Name
			echrono::Steady m_lastUpdatateName;
		public:
			/**
			 * @brief change name of the thread
			 * @param[in] _name Name of the thread
			 */
			void setName(etk::String m_name);
		private:
			#if defined(__TARGET_OS__Android)
				static void* threadCallback(void* _userData);
			#endif
			/**
			 * @brief Internal periodic call of the thead
			 */
			void threadCall();
		protected:
			/**
			 * @brief Periodic call of the thread process
			 * @return true The thread has ended process and need to be destroyed
			 * @return false Need an other cycle if user does not request stop
			 */
			virtual bool onThreadCall();
	};
}

