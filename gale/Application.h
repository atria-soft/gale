/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_CONTEXT_APPLICATION_H__
#define __GALE_CONTEXT_APPLICATION_H__

namespace gale {
	class Context;
	class Application : public std::enable_shared_from_this<gale::Application> {
		public:
			Application() {};
			virtual ~Application() {};
		public:
			/**
			 * @brief Initialize the Application
			 * @param[in] _context Current gale context
			 * @param[in] _initId Initialzation ID (start at 0 and will increment wile returning true)
			 * @return true if the init is fisnished
			 * @return false need more inits
			 */
			virtual bool init(gale::Context& _context, size_t _initId) = 0;
			/**
			 * @brief The application is ended ==> call this function before ended
			 */
			virtual void unInit(gale::Context& _context) = 0;
		public:
			/**
			 * @brief Event on an input (finger, mouse, stilet)
			 * @param[in] _event Event properties
			 */
			virtual void onEventInput(const ewol::event::Input& _event);
			/**
			 * @brief Entry event.
			 *        represent the physical event :
			 *            - Keyboard (key event and move event)
			 *            - Accelerometer
			 *            - Joystick
			 * @param[in] _event Event properties
			 */
			virtual void onEventEntry(const ewol::event::Entry& _event);
	};
}

#endif
