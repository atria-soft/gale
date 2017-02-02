/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/os/Fifo.hpp>
#include <etk/os/FSNode.hpp>
#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/key/key.hpp>
#include <gale/resource/Manager.hpp>
#include <gale/Application.hpp>
#include <gale/context/clipBoard.hpp>
#include <gale/context/commandLine.hpp>
// TODO : #include <gale/context/InputManager.hpp>
#include <gale/context/Fps.hpp>
#include <gale/Thread.hpp>
#include <ememory/memory.hpp>
#include <gale/orientation.hpp>
#include <gale/context/clipBoard.hpp>
#include <ethread/tools.hpp>
#include <echrono/Steady.hpp>

#define MAX_MANAGE_INPUT (15)

namespace gale {
	class Thread;
	class Context {
		protected:
			std::recursive_mutex m_mutex;
			ememory::SharedPtr<gale::Thread> m_periodicThread;
		private:
			ememory::SharedPtr<gale::Application> m_application; //!< Application handle
		public:
			ememory::SharedPtr<gale::Application> getApplication() {
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
		protected:
			// simulation area:
			bool m_simulationActive;
			etk::FSNode m_simulationFile;
		private:
			echrono::Steady m_previousDisplayTime;  // this is to limit framerate ... in case...
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
			void postAction(std::function<void(gale::Context& _context)> _action);
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
		protected:
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
			 * @brief The application request a change of his current size force the fullscreen mode.
			 * @param[in] _status status of the fullscreen mode.
			 */
			virtual void setFullScreen(bool _status);
		protected:
			ivec2 m_windowsPos; //!< current size of the system
		public:
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
			 * @brief The Application request the current position of the windows.
			 * @return Turrent position of the Windows.
			 */
			virtual vec2 getPos();
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
			 * @brief get all Keyboard event from the X system (like many time use of META)
			 * @param[in] _status "true" if all the event will be get, false if we want only ours.
			 */
			virtual void grabKeyboardEvents(bool _status) {}
			/**
			 * @brief get all Mouse/Touch events from the X system
			 * @param[in] _status "true" if all the event will be get, false if we want only ours.
			 * @param[in] _forcedPosition the position where the mouse might be reset at  every events ...
			 */
			virtual void grabPointerEvents(bool _status, const vec2& _forcedPosition) { };
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
			 * @brief Enable or Disable the decoration on the Windows (availlable only on Desktop)
			 * @param[in] _status "true" to enable decoration / false otherwise
			 */
			virtual void setWindowsDecoration(bool _status) {};
			/**
			 * @brief get the curent time in micro-second
			 * @note : must be implemented in all system OS implementation
			 * @return The curent time of the process
			 */
			// TODO : REMOVE this ... deprecated since c++11
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
			/**
			 * @brief Internal API to run the processing of the event loop ...
			 * @return The Exit value of the program
			 * @note INTERNAL API
			 */
			virtual int32_t run() = 0;
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

