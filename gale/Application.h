/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <ememory/memory.h>
#include <etk/types.h>
#include <etk/math/Vector2D.h>
#include <gale/orientation.h>
#include <gale/key/status.h>
#include <gale/key/type.h>
#include <gale/key/Special.h>
#include <gale/context/cursor.h>
#include <gale/context/clipBoard.h>

namespace gale {
	class Context;
	class Application : public ememory::EnableSharedFromThis<gale::Application> {
		public:
			Application();
			virtual ~Application();
		public:
			/**
			 * @brief The application is created.
			 * @param[in] _context Current gale context.
			 */
			virtual void onCreate(gale::Context& _context);
			/**
			 * @brief The application is started.
			 * @param[in] _context Current gale context.
			 */
			virtual void onStart(gale::Context& _context);
			/**
			 * @brief The application is resumed (now visible).
			 * @param[in] _context Current gale context.
			 */
			virtual void onResume(gale::Context& _context);
			/**
			 * @brief call application to precalculate drawing.
			 * @param[in] _context Current gale context.
			 */
			virtual void onRegenerateDisplay(gale::Context& _context);
			/**
			 * @brief Real draw of the application
			 * @param[in] _context Current gale context.
			 */
			virtual void onDraw(gale::Context& _context);
			/**
			 * @brief The application is Hide / not visible.
			 * @param[in] _context Current gale context.
			 */
			virtual void onPause(gale::Context& _context);
			/**
			 * @brief The application is stopped.
			 * @param[in] _context Current gale context.
			 */
			virtual void onStop(gale::Context& _context);
			/**
			 * @brief The application is removed (call destructor just adter it.).
			 * @param[in] _context Current gale context.
			 */
			virtual void onDestroy(gale::Context& _context);
			/**
			 * @brief The user request application removing.
			 * @param[in] _context Current gale context.
			 */
			virtual void onKillDemand(gale::Context& _context);
			/**
			 * @brief Exit the application (not availlable on IOs, ==> the user will not understand the comportement. He will think the application has crashed (Apple philosophie))
			 * @param[in] _value value to return on the program
			 */
			virtual void exit(int32_t _value);
			
		private:
			bool m_needRedraw;
		public:
			virtual void markDrawingIsNeeded();
			virtual bool isDrawingNeeded();
		public:
			/**
			 * @brief Get touch/mouse/... event.
			 * @param[in] _type Type of pointer event
			 * @param[in] _pointerID Pointer id of the touch event.
			 * @param[in] _pos Position of the event (can be <0 if out of window).
			 * @param[in] _state Key state (up/down/move)
			 */
			virtual void onPointer(enum gale::key::type _type,
			                       int32_t _pointerID,
			                       const vec2& _pos,
			                       gale::key::status _state);
			/**
			 * @brief Get keyborad value input.
			 * @param[in] _special Current special key status (ctrl/alt/shift ...).
			 * @param[in] _type Type of the event.
			 * @param[in] _value Unicode value of the char pushed (viable only if _type==gale::key::keyboard::character).
			 * @param[in] _state State of the key (up/down/upRepeate/downRepeate)
			 */
			virtual void onKeyboard(const gale::key::Special& _special,
			                        enum gale::key::keyboard _type,
			                        char32_t _value,
			                        gale::key::status _state);
			/**
			 * @brief Show the virtal keyboard (if possible : only on iOs/Android)
			 */
			virtual void keyboardShow();
			/**
			 * @brief Hide the virtal keyboard (if possible : only on iOs/Android)
			 */
			virtual void keyboardHide();
		public:
			/**
			 * @brief Event generated when user change the size of the window.
			 * @param[in] _size New size of the window.
			 */
			virtual void onResize(const ivec2& _size);
			/**
			 * @brief Set the size of the window (if possible: Android and Ios does not support it)
			 * @param[in] _size New size of the window.
			 * @return 
			 */
			virtual void setSize(const vec2& _size);
			/**
			 * @brief Get the size of the window.
			 * @return Current size of the window.
			 */
			virtual vec2 getSize() const;
		public:
			/**
			 * @brief Event generated when user change the position of the window.
			 * @param[in] _size New position of the window.
			 */
			virtual void onMovePosition(const vec2& _size);
			/**
			 * @brief Set the position of the window (if possible: Android and Ios does not support it)
			 * @param[in] _size New position of the window.
			 */
			virtual void setPosition(const vec2& _size);
			/**
			 * @brief Get the position of the window.
			 * @return Current position of the window.
			 */
			virtual vec2 getPosition() const;
		private:
			std::string m_title;
		public:
			/**
			 * @brief Set the title of the application
			 * @param[in] _title New title to set at the application (if possible: Android and Ios does not support it)
			 */
			virtual void setTitle(const std::string& _title);
			/**
			 * @brief Get the current title of the application
			 * @return Current title
			 */
			virtual std::string getTitle();
		private:
			std::string m_iconName;
		public:
			/**
			 * @brief set the Icon of the application.
			 * @param[in] _iconFile File name icon (.bmp/.png).
			 */
			virtual void setIcon(const std::string& _iconFile);
			/**
			 * @brief Get the current filename of the application.
			 * @return Filename of the icon.
			 */
			virtual std::string getIcon();
		private:
			enum gale::context::cursor m_cursor;
		public:
			/**
			 * @brief Set the cursor type.
			 * @param[in] _newCursor Selected cursor.
			 */
			virtual void setCursor(enum gale::context::cursor _newCursor);
			/**
			 * @brief Get the cursor type.
			 * @return the current cursor.
			 */
			virtual enum gale::context::cursor getCursor();
		public:
			/**
			 * @brief Open an URL on an internal brother.
			 * @param[in] _url URL to open.
			 */
			virtual void openURL(const std::string& _url);
		private:
			enum gale::orientation m_orientation;
		public:
			/**
			 * @brief set the screen orientation (if possible : only on iOs/Android)
			 * @param[in] _orientation New orientation.
			 */
			virtual void setOrientation(enum gale::orientation _orientation);
			/**
			 * @brief get the screen orientation (if possible : only on iOs/Android)
			 * @return Current orientation.
			 */
			virtual enum gale::orientation getOrientation();
		public:
			/**
			 * @brief A clipboard data is back (apear after a request of a new clipboard).
			 * @param[in] _clipboardId Id of the clipboard.
			 */
			virtual void onClipboardEvent(enum gale::context::clipBoard::clipboardListe _clipboardId);
		public:
			/**
			 * @brief Call every time a draw is called (not entirely periodic, but faster at we can ...
			 * @param[in] _time Current time of the call;
			 */
			virtual void onPeriod(int64_t _time) {};
	};
}
