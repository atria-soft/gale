/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/os/Fifo.h>
#include <etk/os/FSNode.h>
#include <gale/debug.h>
#include <gale/gale.h>
#include <gale/key/key.h>
#include <gale/resource/Manager.h>
#include <gale/Application.h>
#include <gale/context/clipBoard.h>
#include <gale/context/commandLine.h>
// TODO : #include <gale/context/InputManager.h>
#include <gale/context/Fps.h>
#include <memory>
#include <gale/orientation.h>
#include <gale/context/clipBoard.h>
#include <ethread/tools.h>

#define MAX_MANAGE_INPUT (15)

namespace gale {
	class Context/* : private gale::object::RemoveEvent */{
		private:
			std::shared_ptr<gale::Application> m_application; //!< Application handle
		public:
			std::shared_ptr<gale::Application> getApplication() {
				return m_application;
			}
		private:
			gale::context::CommandLine m_commandLine; //!< Start command line information
		public:
			gale::context::CommandLine& getCmd() {
				return m_commandLine;
			};
		private:
			gale::resource::Manager m_resourceManager; //!< global resources Manager
		public:
			gale::resource::Manager& getResourcesManager() {
				return m_resourceManager;
			};
		public:
			Context(gale::Application* _application, int32_t _argc=0, const char* _argv[]=nullptr);
			virtual ~Context();
		protected:
			/**
			 * @brief set the curent interface.
			 * @note this lock the main mutex
			 */
			void lockContext();
			/**
			 * @brief set the curent interface at nullptr.
			 * @note this un-lock the main mutex
			 */
			void unLockContext();
		private:
			// simulation area:
			bool m_imulationActive;
			etk::FSNode m_simulationFile;
		private:
			int64_t m_previousDisplayTime;  // this is to limit framerate ... in case...
			// TODO : gale::context::InputManager m_input;
			etk::Fifo<std::function<void(gale::Context& _context)> > m_msgSystem;
			bool m_displayFps;
			gale::context::Fps m_FpsSystemEvent;
			gale::context::Fps m_FpsSystemContext;
			gale::context::Fps m_FpsSystem;
			gale::context::Fps m_FpsFlush;
			/**
			 * @brief Processing all the event arrived ... (commoly called in draw function)
			 */
			void processEvents();
		public:
			
			virtual void setArchiveDir(int _mode, const char* _str, const char* _applName=nullptr);
			
			virtual void OS_SetInput(enum gale::key::type _type,
			                         enum gale::key::status _status,
			                         int32_t _pointerID,
			                         const vec2& _pos);
			virtual void OS_setKeyboard(const gale::key::Special& _special,
			                            enum gale::key::keyboard _type,
			                            enum gale::key::status _state,
			                            bool _isARepeateKey = false,
			                            char32_t _char = u32char::Null);
			/**
			 * @brief The current context is suspended
			 */
			virtual void OS_Suspend();
			/**
			 * @brief The current context is resumed
			 */
			virtual void OS_Resume();
			
			/**
			 * @brief The current context is set in foreground (framerate is maximum speed)
			 */
			virtual void OS_Foreground();
			/**
			 * @brief The current context is set in background (framerate is slowing down (max fps)/5 # 4fps)
			 */
			virtual void OS_Background();
		public:
			void requestUpdateSize();
		public:
			// return true if a flush is needed
			bool OS_Draw(bool _displayEveryTime);
		public:
			/**
			 * @brief reset event management for the IO like Input ou Mouse or keyborad
			 */
			void resetIOEvent();
			/**
			 * @brief The OS inform that the openGL constext has been destroy  == > use to automaticly reload the texture and other thinks ...
			 */
			void OS_OpenGlContextDestroy();
			/**
			 * @brief The OS Inform that the Window has been killed
			 */
			void OS_Stop();
			/**
			 * @brief The application request that the Window will be killed
			 */
			virtual void stop();
		private:
			ivec2 m_windowsSize; //!< current size of the system
		public:
			/**
			 * @brief get the current windows size
			 * @return the current size ...
			 */
			const ivec2& getSize() {
				return m_windowsSize;
			};
			/**
			 * @brief The OS inform that the current windows has change his size.
			 * @param[in] _size new size of the windows.
			 */
			virtual void OS_Resize(const vec2& _size);
			/**
			 * @brief The application request a change of his curent size.
			 * @param[in] _size new Requested size of the windows.
			 */
			virtual void setSize(const vec2& _size);
			/**
			 * @brief The OS inform that the current windows has change his position.
			 * @param[in] _pos New position of the Windows.
			 */
			void OS_Move(const vec2& _pos);
			/**
			 * @brief The Application request that the current windows will change his position.
			 * @param[in] _pos New position of the Windows requested.
			 */
			virtual void setPos(const vec2& _pos);
			/**
			 * @brief The OS inform that the Windows is now Hidden.
			 */
			void OS_Hide();
			/**
			 * @brief The Application request that the Windows will be Hidden.
			 */
			virtual void hide();
			/**
			 * @brief The OS inform that the Windows is now visible.
			 */
			void OS_Show();
			/**
			 * @brief The Application request that the Windows will be visible.
			 */
			virtual void show();
			/**
			 * @brief Redraw all the windows
			 */
			void forceRedrawAll();
			
			// TODO : Later ...
			#if 0
			/**
			 * @brief This is to transfert the event from one widget to another one
			 * @param source the widget where the event came from
			 * @param destination the widget where the event mitgh be generated now
			 */
			void inputEventTransfertWidget(std::shared_ptr<gale::Widget> _source, std::shared_ptr<gale::Widget> _destination);
			/**
			 * @brief This fonction lock the pointer properties to move in relative instead of absolute
			 * @param[in] widget The widget that lock the pointer events
			 */
			void inputEventGrabPointer(std::shared_ptr<gale::Widget> _widget);
			/**
			 * @brief This fonction un-lock the pointer properties to move in relative instead of absolute
			 */
			void inputEventUnGrabPointer();
			#endif
			
			/**
			 * @brief display the virtal keyboard (for touch system only)
			 */
			virtual void keyboardShow();
			/**
			 * @brief Hide the virtal keyboard (for touch system only)
			 */
			virtual void keyboardHide();
			
			/**
			 * @brief Inform the Gui that we want to have a copy of the clipboard
			 * @param[in] _clipboardID ID of the clipboard (STD/SELECTION) only apear here
			 */
			virtual void clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID);
			/**
			 * @brief Inform the Gui that we are the new owner of the clipboard
			 * @param[in] _clipboardID ID of the clipboard (STD/SELECTION) only apear here
			 */
			virtual void clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID);
			/**
			 * @brief Call by the OS when a clipboard arrive to US (previously requested by a widget)
			 * @param[in] Id of the clipboard
			 */
			void OS_ClipBoardArrive(enum gale::context::clipBoard::clipboardListe _clipboardID);
			/**
			 * @brief set the new title of the windows
			 * @param[in] title New desired title
			 */
			virtual void setTitle(const std::string& _title);
			/**
			 * @brief Open an URL on an eternal brother.
			 * @param[in] _url URL to open.
			 */
			virtual void openURL(const std::string& _url) { };
			/**
			 * @brief force the screen orientation (availlable on portable elements ...
			 * @param[in] _orientation Selected orientation.
			 */
			virtual void forceOrientation(enum gale::orientation _orientation) { };
			/**
			 * @brief get all the event from the X system
			 * @param[in] _isGrabbed "true" if all the event will be get, false if we want only ours.
			 * @param[in] _forcedPosition the position where the mouse might be reset at  every events ...
			 */
			virtual void grabPointerEvents(bool _isGrabbed, const vec2& _forcedPosition) { };
			/**
			 * @brief set the cursor display type.
			 * @param[in] _newCursor selected new cursor.
			 */
			virtual void setCursor(enum gale::context::cursor _newCursor) { };
			/**
			 * @brief set the Icon of the program
			 * @param[in] _inputFile new filename icon of the curent program.
			 */
			virtual void setIcon(const std::string& _inputFile) { };
			/**
			 * @brief get the curent time in micro-second
			 * @note : must be implemented in all system OS implementation
			 * @return The curent time of the process
			 */
			static int64_t getTime();
		private:
			// TODO : set user argument here ....
			
		public:
			/**
			 * @brief This is the only one things the User might done in his main();
			 * @note : must be implemented in all system OPS implementation
			 * @note To answare you before you ask the question, this is really simple:
			 *       Due to the fect that the current system is multiple-platform, you "main"
			 *       Does not exist in the android platform, then gale call other start 
			 *       and stop function, to permit to have only one code
			 * @note The main can not be in the gale, due to the fact thet is an librairy
			 * @param[in] _argc Standard argc
			 * @param[in] _argv Standard argv
			 * @return normal error int for the application error management
			 */
			static int main(int _argc, const char *_argv[]);
		public:
			/**
			 * @brief Special for init (main) set the start image when loading data
			 * @param[in] _fileName Name of the image to load
			 */
			void setInitImage(const std::string& _fileName);
		protected:
			#if 0
				TODO : Rework this ==> but how ...
			/**
			 * @brief HARDWARE keyboard event from the system
			 * @param[in] _key event type
			 * @param[in] _status Up or down status
			 * @return Keep the event or not
			 */
			virtual bool systemKeyboradEvent(enum gale::key::keyboard _key, bool _down);
			#endif
	};
	/**
	 * @brief From everyware in the program, we can get the context inteface.
	 * @return current reference on the instance.
	 */
	Context& getContext();
	/**
	 * @brief Set a context of the current thread.
	 * @param[in] _context Current context to associate.
	 */
	void setContext(gale::Context* _context);
	/**
	 * @brief When a new thread is created, it is needed to register it in the gale context interface to permit to get the context associated on it ...
	 * @param[in] _thread generic C++11 thread handle
	 */
	void contextRegisterThread(std::thread* _thread);
	/**
	 * @brief Remove an associated thread
	 * @param[in] _thread generic C++11 thread handle
	 */
	void contextUnRegisterThread(std::thread* _thread);
}

