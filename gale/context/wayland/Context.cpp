/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <signal.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <etk/types.hpp>
#include <etk/os/FSNode.hpp>
#include <etk/tool.hpp>

#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/key/key.hpp>
#include <gale/context/commandLine.hpp>
#include <gale/resource/Manager.hpp>
#include <gale/context/Context.hpp>
#include <gale/Dimension.hpp>
#include <etk/etk.hpp>

extern "C" {
	#include <linux/input.h>
	//https://people.freedesktop.org/~whot/wayland-doxygen/wayland/Client/
	#include <wayland-client.h>
	#include <wayland-egl.h>
	#include <wayland-cursor.h>
	#include <GLES2/gl2.h>
	#include <EGL/egl.h>
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
}

#include <gale/renderer/openGL/openGL-include.hpp>

bool hasDisplay = false;
#define DEBUG_WAYLAND_EVENT
#ifdef DEBUG_WAYLAND_EVENT
	#define WAYLAND_DEBUG      GALE_DEBUG
	#define WAYLAND_VERBOSE    GALE_VERBOSE
	#define WAYLAND_INFO       GALE_INFO
	#define WAYLAND_CRITICAL   GALE_CRITICAL
#else
	#define WAYLAND_DEBUG      GALE_VERBOSE
	#define WAYLAND_VERBOSE    GALE_VERBOSE
	#define WAYLAND_INFO       GALE_VERBOSE
	#define WAYLAND_CRITICAL   GALE_VERBOSE
#endif

// ***********************************************************************
// ** Define all calbac "C" needed to wrap Wayland in C++
// ***********************************************************************
static void global_registry_handler(void* _data, struct wl_registry* _registry, uint32_t _id, const char* _interface, uint32_t _version);
static void global_registry_remover(void* _data, struct wl_registry* _registry, uint32_t _id);
static const struct wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover
};
static void seat_handle_capabilities(void* _data, struct wl_seat* _seat, uint32_t _caps);
static const struct wl_seat_listener seat_listener = {
	seat_handle_capabilities,
};
static void pointer_handle_enter(void* _data, struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface, wl_fixed_t _sx, wl_fixed_t _sy);
static void pointer_handle_leave(void* _data, struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface);
static void pointer_handle_motion(void* _data, struct wl_pointer* _pointer, uint32_t _time, wl_fixed_t _sx, wl_fixed_t _sy);
static void pointer_handle_button(void* _data, struct wl_pointer* _pointer, uint32_t _serial, uint32_t _time, uint32_t _button, uint32_t _state);
static void pointer_handle_axis(void* _data, struct wl_pointer* _pointer, uint32_t _time, uint32_t _axis, wl_fixed_t _value);
static const struct wl_pointer_listener pointer_listener = {
    pointer_handle_enter,
    pointer_handle_leave,
    pointer_handle_motion,
    pointer_handle_button,
    pointer_handle_axis,
};
static void redraw(void* _data, struct wl_callback* _callback, uint32_t _time);
static const struct wl_callback_listener frame_listener = {
    redraw
};
static void configure_callback(void* _data, struct wl_callback* _callback, uint32_t _time);
static struct wl_callback_listener configure_callback_listener = {
    configure_callback,
};
static void handle_ping(void* _data, struct wl_shell_surface* _shellSurface, uint32_t _serial);
static void handle_configure(void* _data, struct wl_shell_surface* _shellSurface, uint32_t _edges, int32_t _width, int32_t _height);
static void handle_popup_done(void* _data, struct wl_shell_surface* _shellSurface);
static const struct wl_shell_surface_listener shell_surface_listener = {
	handle_ping,
	handle_configure,
	handle_popup_done
};
static void keyboard_handle_keymap(void* _data, struct wl_keyboard* _keyboard, uint32_t _format, int _fd, uint32_t _size);
static void keyboard_handle_enter(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface, struct wl_array* _keys);
static void keyboard_handle_leave(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface);
static void keyboard_handle_key(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _time, uint32_t _key, uint32_t _state);
static void keyboard_handle_modifiers(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _modsDepressed, uint32_t _modsLatched, uint32_t _modsLocked, uint32_t _group);
static const struct wl_keyboard_listener keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};

/**
 * @brief Wayland interface context to load specific context wrapper...
 */
class WAYLANDInterface : public gale::Context {
	private:
		gale::key::Special m_guiKeyBoardMode;
		ivec2 m_size;
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		std::string m_uniqueWindowsName;
		bool m_run;
		bool m_fullscreen;
		bool m_configured;
		bool m_opaque;
		enum gale::context::cursor m_cursorCurrent; //!< curent cursor
		vec2 m_cursorCurrentPosition;
		
		// Wayland interface
		struct wl_display* m_display;
		struct wl_registry* m_registry;
		struct wl_compositor* m_compositor;
		struct wl_shell* m_shell;
		struct wl_seat* m_seat;
		struct wl_pointer* m_pointer;
		struct wl_keyboard* m_keyboard;
		struct wl_shm* m_shm;
		struct wl_cursor_theme* m_cursorTheme;
		struct wl_cursor* m_cursorDefault;
		struct wl_surface* m_cursorSurface;
		struct wl_egl_window *m_eglWindow;
		struct wl_surface *m_surface;
		struct wl_shell_surface *m_shellSurface;
		struct wl_callback *m_callback;
		// EGL interface:
		EGLDisplay m_eglDisplay;
		EGLContext m_eglContext;
		EGLConfig m_eglConfig;
		EGLSurface m_eglSurface;
	public:
		WAYLANDInterface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv),
		  m_size(800,600),
		  m_run(false),
		  m_fullscreen(false),
		  m_configured(false),
		  m_opaque(false),
		  m_cursorCurrent(gale::context::cursor::leftArrow),
		  m_display(nullptr),
		  m_registry(nullptr),
		  m_compositor(nullptr),
		  m_shell(nullptr),
		  m_seat(nullptr),
		  m_pointer(nullptr),
		  m_keyboard(nullptr),
		  m_shm(nullptr),
		  m_cursorTheme(nullptr),
		  m_cursorDefault(nullptr),
		  m_cursorSurface(nullptr),
		  m_eglWindow(nullptr),
		  m_surface(nullptr),
		  m_shellSurface(nullptr),
		  m_callback(nullptr) {
			// in case ...
			GALE_WARNING("WAYLAND: INIT [START]");
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				m_inputIsPressed[iii] = false;
			}
			int i, ret = 0;
			
			m_display = wl_display_connect(nullptr);
			assert(m_display);
			
			m_registry = wl_display_get_registry(m_display);
			wl_registry_add_listener(m_registry, &registry_listener, this);
			
			wl_display_dispatch(m_display);
			initEgl(m_opaque);
			
			createSurface();
			
			m_cursorSurface = wl_compositor_create_surface(m_compositor);
			
			m_uniqueWindowsName = "GALE_" + etk::to_string(etk::tool::irand(0, 1999999999));
			m_run = true;
			GALE_WARNING("WAYLAND: INIT [STOP]");
		}
		
		~WAYLANDInterface() {
			destroySurface();
			unInitEgl();
			wl_surface_destroy(m_cursorSurface);
			if (m_cursorTheme) {
				wl_cursor_theme_destroy(m_cursorTheme);
			}
			if (m_shell) {
				wl_shell_destroy(m_shell);
			}
			if (m_compositor) {
				wl_compositor_destroy(m_compositor);
			}
			wl_registry_destroy(m_registry);
			wl_display_flush(m_display);
			wl_display_disconnect(m_display);
		}
		/****************************************************************************************/
		int32_t run() {
			int ret = 0;
			while (    m_run == true
			        && ret != -1) {
				ret = wl_display_dispatch(m_display);
				GALE_INFO("loop dispatch event " << ret);
			}
			GALE_INFO("Normal application exit ...");
			return 0;
		}
		
		void initEgl(int _opaque) {
			GALE_INFO("Init EGL [START]");
			static const EGLint context_attribs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 2,
				EGL_NONE
			};
			EGLint config_attribs[] = {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_RED_SIZE, 1,
				EGL_GREEN_SIZE, 1,
				EGL_BLUE_SIZE, 1,
				EGL_ALPHA_SIZE, 1,
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
				EGL_NONE
			};
			EGLint major, minor;
			if (_opaque) {
				config_attribs[9] = 0;
			}
			m_eglDisplay = eglGetDisplay(m_display);
			assert(m_eglDisplay);
			
			if (eglInitialize(m_eglDisplay, &major, &minor) != EGL_TRUE) {
				GALE_CRITICAL("Can't initialise egl display");
				return;
			}
			if(eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) {
				GALE_CRITICAL("Can't initialise Bind");
				return;
			}
			EGLint nnn;
			EGLBoolean ret = eglChooseConfig(m_eglDisplay, config_attribs, &m_eglConfig, 1, &nnn);
			/*
			EGLint count = 0;
			eglGetConfigs(m_egl_display, nullptr, 0, &count);
			GALE_INFO("EGL has " << count << " configs");
			EGLConfig* configs = (EGLConfig*)calloc(count, sizeof *configs);
			GALE_INFO("Display all configs:");
			for (int32_t iii=0; iii<nnn; ++iii) {
				EGLint size = 0;
				EGLint sizeRed = 0;
				eglGetConfigAttrib(m_eglDisplay, configs[iii], EGL_BUFFER_SIZE, &size);
				eglGetConfigAttrib(m_eglDisplay, configs[iii], EGL_RED_SIZE, &sizeRed);
				GALE_INFO("    " << iii << "     BufferSize=" << size << "     red size=" << sizeRed);
			}
			*/
			//assert(ret && n == 1);
			
			m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, context_attribs);
			assert(m_eglContext);
			GALE_INFO("Init EGL [STOP]");
		}
		
		void unInitEgl() {
			GALE_INFO("un-Init EGL [START]");
			/* Required, otherwise segfault in egl_dri2.c: dri2_make_current() on eglReleaseThread(). */
			eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			eglTerminate(m_eglDisplay);
			eglReleaseThread();
			GALE_INFO("un-Init EGL [STOP]");
		}
		
		
		void createSurface() {
			GALE_INFO("CRATE the SURFACE [START]");
			EGLBoolean ret;
			m_surface = wl_compositor_create_surface(m_compositor);
			m_shellSurface = wl_shell_get_shell_surface(m_shell, m_surface);
			wl_shell_surface_add_listener(m_shellSurface, &shell_surface_listener, this);
			m_eglWindow = wl_egl_window_create(m_surface, m_size.x(), m_size.y());
			m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_eglWindow, nullptr);
			wl_shell_surface_set_title(m_shellSurface, m_uniqueWindowsName.c_str());
			ret = eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
			assert(ret == EGL_TRUE);
			toggleFullscreen();
			GALE_INFO("CRATE the SURFACE [STOP]");
		}
		
		void destroySurface() {
			GALE_INFO("DESTROY the SURFACE [START]");
			wl_egl_window_destroy(m_eglWindow);
			wl_shell_surface_destroy(m_shellSurface);
			wl_surface_destroy(m_surface);
			if (m_callback) {
				wl_callback_destroy(m_callback);
			}
			GALE_INFO("DESTROY the SURFACE [STOP]");
		}
		
		void toggleFullscreen() {
			GALE_INFO("toggleFullscreen [START]");
			m_configured = false;
			if (m_fullscreen) {
				wl_shell_surface_set_fullscreen(m_shellSurface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, nullptr);
			} else {
				wl_shell_surface_set_toplevel(m_shellSurface);
				handleConfigure(m_shellSurface, 0, m_size);
			}
			struct wl_callback *callback = wl_display_sync(m_display);
			wl_callback_add_listener(callback, &configure_callback_listener, this);
			GALE_INFO("toggleFullscreen [STOP]");
		}
		
		/****************************************************************************************/
		// recorder on elements ...
		
		void registryHandler(struct wl_registry* _registry, uint32_t _id, const char* _interface, uint32_t _version) {
			GALE_WARNING("Got a registry event for '" << _interface  << "' id=" << _id);
			if (strcmp(_interface, "wl_compositor") == 0) {
				m_compositor = (struct wl_compositor *)wl_registry_bind(_registry, _id, &wl_compositor_interface, 1);
			} else if (strcmp(_interface, "wl_shell") == 0) {
				m_shell = (struct wl_shell*)wl_registry_bind(_registry, _id, &wl_shell_interface, 1);
			} else if (strcmp(_interface, "wl_seat") == 0) {
				m_seat = (struct wl_seat*)wl_registry_bind(_registry, _id, &wl_seat_interface, 1);
				wl_seat_add_listener(m_seat, &seat_listener, this);
			} else if (strcmp(_interface, "wl_shm") == 0) {
				m_shm = (struct wl_shm*)wl_registry_bind(_registry, _id, &wl_shm_interface, 1);
				m_cursorTheme = wl_cursor_theme_load(nullptr, 32, m_shm);
				m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "left_ptr");
			} else {
				GALE_WARNING("    ==> Not used ...");
			}
			GALE_INFO("registry_handle_global [STOP]");
		}
		
		void registryRemover(struct wl_registry* _registry, uint32_t _id) {
			GALE_WARNING("Got a registry losing event for " << _id);
		}
		
		/****************************************************************************************/
		// screen capabilities
		void seatHandleCapabilities(struct wl_seat* _seat, enum wl_seat_capability _caps) {
			if ((_caps & WL_SEAT_CAPABILITY_POINTER) && !m_pointer) {
				GALE_WARNING("Display has a pointer");
				m_pointer = wl_seat_get_pointer(_seat);
				wl_pointer_add_listener(m_pointer, &pointer_listener, this);
			} else if (!(_caps & WL_SEAT_CAPABILITY_POINTER) && m_pointer) {
				GALE_WARNING("Display has No more pointer");
				wl_pointer_destroy(m_pointer);
				m_pointer = nullptr;
			}
			if ((_caps & WL_SEAT_CAPABILITY_KEYBOARD) && !m_keyboard) {
				GALE_WARNING("Display has a keyboard");
				m_keyboard = wl_seat_get_keyboard(_seat);
				wl_keyboard_add_listener(m_keyboard, &keyboard_listener, this);
			} else if (!(_caps & WL_SEAT_CAPABILITY_KEYBOARD) && m_keyboard) {
				wl_keyboard_destroy(m_keyboard);
				m_keyboard = nullptr;
			}
			
			if (_caps & WL_SEAT_CAPABILITY_TOUCH) {
				GALE_WARNING("Display has a touch screen");
			}
		}
		
		/****************************************************************************************/
		// Pointer section event ...
		
		void pointerHandleEnter(struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface, ivec2 _pos) {
			GALE_WARNING("Pointer Enter surface" << _surface << " at pos=" << _pos);
			m_cursorCurrentPosition = vec2(_pos.x(), _pos.y());
			struct wl_buffer *buffer;
			struct wl_cursor *cursor = m_cursorDefault;
			struct wl_cursor_image *image;
			
			if (m_fullscreen == true) {
				wl_pointer_set_cursor(_pointer, _serial, nullptr, 0, 0);
			} else if (cursor) {
				image = m_cursorDefault->images[0];
				buffer = wl_cursor_image_get_buffer(image);
				wl_pointer_set_cursor(_pointer, _serial, m_cursorSurface, image->hotspot_x, image->hotspot_y);
				wl_surface_attach(m_cursorSurface, buffer, 0, 0);
				wl_surface_damage(m_cursorSurface, 0, 0, image->width, image->height);
				wl_surface_commit(m_cursorSurface);
			}
			GALE_WARNING("Pointer enter [STOP]");
		}
		void pointerHandleLeave(struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface) {
			GALE_WARNING("Pointer left surface" << _surface);
		}
		void pointerHandleMotion(struct wl_pointer* _pointer, uint32_t _time, ivec2 _pos) {
			GALE_WARNING("Pointer moved at " << _pos);
			m_cursorCurrentPosition = vec2(_pos.x(), _pos.y());
			bool findPointer = false;
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				if (m_inputIsPressed[iii] == true) {
					findPointer = true;
					OS_SetInput(gale::key::type::mouse,
					            gale::key::status::move,
					            iii,
					            m_cursorCurrentPosition);
				}
			}
			if (findPointer == false) {
				OS_SetInput(gale::key::type::mouse,
				            gale::key::status::move,
				            0,
				            m_cursorCurrentPosition);
			}
		}
		void pointerHandleButton(struct wl_pointer* _wl_pointer, uint32_t _serial, uint32_t _time, uint32_t _button, bool _btPressed) {
			GALE_WARNING("Pointer button");
			int32_t idButton = -1;
			switch (_button) {
				case BTN_LEFT:   idButton = 1; break;
				case BTN_MIDDLE: idButton = 2; break;
				case BTN_RIGHT:  idButton = 3; break;
				default: GALE_ERROR("unknow button:" << _button); break;
			}
			if (idButton != -1) {
				return;
			}
			if (_btPressed == true) {
				OS_SetInput(gale::key::type::mouse,
				            gale::key::status::down,
				            idButton,
				            m_cursorCurrentPosition);
			} else {
				OS_SetInput(gale::key::type::mouse,
				            gale::key::status::up,
				            idButton,
				            m_cursorCurrentPosition);
			}
			m_inputIsPressed[idButton] = _btPressed;
		}
		void pointerHandleAxis(struct wl_pointer* _wl_pointer, uint32_t _time, uint32_t _axis, wl_fixed_t _value) {
			GALE_WARNING("Pointer handle axis");
			// scroll up and down ....
		}
		/****************************************************************************************/
		
		void redraw(struct wl_callback* _callback, uint32_t _time) {
			GALE_WARNING("REDRAW [START]");
			assert(m_callback == _callback);
			m_callback = nullptr;
			if (_callback) {
				wl_callback_destroy(_callback);
			}
			// If display is not configured ==> no need to redraw ...
			if (m_configured == false) {
				return;
			}
			
			bool hasDisplay = OS_Draw(true);
			
			if (    m_opaque == true
			     || m_fullscreen == true) {
				struct wl_region *region = (struct wl_region *)wl_compositor_create_region(m_compositor);
				wl_region_add(region, 0, 0, m_size.x(), m_size.y());
				wl_surface_set_opaque_region(m_surface, region);
				wl_region_destroy(region);
			} else {
				wl_surface_set_opaque_region(m_surface, nullptr);
			}
			m_callback = wl_surface_frame(m_surface);
			wl_callback_add_listener(m_callback, &frame_listener, this);
			eglSwapBuffers(m_eglDisplay, m_eglSurface);
			GALE_WARNING("REDRAW [STOP]");
		}
		
		void configureCallback(struct wl_callback* _callback, uint32_t _time) {
			wl_callback_destroy(_callback);
			m_configured = true;
			if (m_callback == nullptr) {
				GALE_ERROR("    ==> nullptr");
				redraw(nullptr, _time);
			}
		}
		
		/****************************************************************************************/
		void handlePing(struct wl_shell_surface* _shell_surface, uint32_t _serial) {
			wl_shell_surface_pong(_shell_surface, _serial);
			GALE_WARNING("Ping ==> pong");
		}
		
		void handleConfigure(struct wl_shell_surface* _shell_surface, uint32_t _edges, ivec2 _size) {
			GALE_WARNING("configure surface : _edges=" << _edges << " size=" << _size);
			if (m_eglWindow != nullptr) {
				wl_egl_window_resize(m_eglWindow, _size.x(), _size.y(), 0, 0);
			}
			m_size = _size;
			OS_Resize(vec2(m_size.x(), m_size.y()));
			GALE_WARNING("configure [STOP]");
		}
		
		void handlePopupDone(struct wl_shell_surface* _shell_surface) {
			GALE_WARNING("Pop-up done");
		}
		/****************************************************************************************/
		void keyboardKeymap(struct wl_keyboard* _keyboard, uint32_t _format, int _fd, uint32_t _size) {
			GALE_WARNING("callback ...");
			GALE_INFO("KEY MAP : '" << _format << "'");
		}
		
		void keyboardEnter(struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface, struct wl_array* _keys) {
			GALE_WARNING("callback ...");
			
		}
		
		void keyboardLeave(struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface) {
			GALE_WARNING("callback ...");
			
		}
		
		void keyboardKey(struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _time, uint32_t _key, uint32_t _state) {
			GALE_WARNING("callback ...");
			GALE_INFO("KEY : '" << _key << "'");
			if (_key == KEY_F11 && _state) {
				setFullScreen(m_fullscreen?false:true);
			} else if (_key == KEY_ESC && _state) {
				m_run = false;
			}
		}
		
		static void keyboardModifiers(struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _modsDepressed, uint32_t _modsLatched, uint32_t _modsLocked, uint32_t _group) {
			
		}
		
		/****************************************************************************************/
		virtual void stop() {
			WAYLAND_INFO("WAYLAND-API: Stop");
			m_run = false;
		}
		/****************************************************************************************/
		virtual void setSize(const vec2& _size) {
			WAYLAND_INFO("WAYLAND-API: changeSize=" << _size);
			/*
			m_currentHeight = _size.y();
			m_currentWidth = _size.x();
			XResizeWindow(m_display, m_WindowHandle, _size.x(), _size.y());
			*/
		}
		/****************************************************************************************/
		void setFullScreen(bool _status) {
			WAYLAND_INFO("WAYLAND-API: changeFullscreen=" << _status);
			m_fullscreen = _status;
			toggleFullscreen();
			// TODO : Grab all event from keyborad
		}
		/****************************************************************************************/
		virtual void grabKeyboardEvents(bool _status) {
			GALE_WARNING("grabKeyboardEvents [START]");
			/*
			if (_status == true) {
				WAYLAND_INFO("WAYLAND-API: Grab Keyboard Events");
				XGrabKeyboard(m_display, m_WindowHandle,
				              False,
				              GrabModeAsync,
				              GrabModeAsync,
				              CurrentTime);
			} else {
				WAYLAND_INFO("WAYLAND-API: Un-Grab Keyboard Events");
				XUngrabKeyboard(m_display, CurrentTime);
			}
			*/
			GALE_WARNING("grabKeyboardEvents [STOP]");
		}
		/****************************************************************************************/
		virtual void setWindowsDecoration(bool _status) {
			WAYLAND_INFO("WAYLAND-API: setWindows Decoration :" << _status);
			/*
			// Remove/set decoration
			Hints hints;
			hints.flags = 2;
			if (_status == true) {
				hints.decorations = 1;
			} else {
				hints.decorations = 0;
			}
			XChangeProperty(m_display, m_WindowHandle,
			                XInternAtom(m_display, "_MOTIF_WM_HINTS", False),
			                XInternAtom(m_display, "_MOTIF_WM_HINTS", False),
			                32, PropModeReplace,
			                (unsigned char *)&hints,5);
			*/
		};
		/****************************************************************************************/
		virtual void setPos(const vec2& _pos) {
			WAYLAND_INFO("WAYLAND-API: changePos=" << _pos);
			/*
			m_windowsPos = _pos;
			XMoveWindow(m_display, m_WindowHandle, _pos.x(), _pos.y());
			m_originX = _pos.x();
			m_originY = _pos.y();
			*/
		}
		/****************************************************************************************/
		/*
		virtual void getAbsPos(ivec2& pos) {
			WAYLAND_INFO("WAYLAND-API: getAbsPos");
			int tmp;
			unsigned int tmp2;
			Window fromroot, tmpwin;
			XQueryPointer(m_display, m_WindowHandle, &fromroot, &tmpwin, &pos.m_floats[0], &pos.m_floats[1], &tmp, &tmp, &tmp2);
		}
		*/
		/****************************************************************************************/
		virtual void setCursor(enum gale::context::cursor _newCursor) {
			if (m_cursorCurrent == _newCursor) {
				return;
			}
			m_cursorCurrent = _newCursor;
			WAYLAND_DEBUG("WAYLAND-API: set New Cursor : " << _newCursor);
			switch (m_cursorCurrent) {
				case gale::context::cursor::none:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "");
					break;
				case gale::context::cursor::leftArrow:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "left_ptr");
					break;
				case gale::context::cursor::info:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "help");
					break;
				case gale::context::cursor::destroy:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "crossed_circle");
					break;
				case gale::context::cursor::help:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "left_ptr");
					break;
				case gale::context::cursor::cycle:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "left_ptr");
					break;
				case gale::context::cursor::spray:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "hand2");
					break;
				case gale::context::cursor::wait:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "watch");
					break;
				case gale::context::cursor::text:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "xterm");
					break;
				case gale::context::cursor::crossHair:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "crosshair");
					break;
				case gale::context::cursor::slideUpDown:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "left_ptr");
					break;
				case gale::context::cursor::slideLeftRight:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "left_ptr");
					break;
				case gale::context::cursor::resizeUp:
				case gale::context::cursor::resizeDown:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "h_double_arrow");
					break;
				case gale::context::cursor::resizeLeft:
				case gale::context::cursor::resizeRight:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "v_double_arrow");
					break;
				case gale::context::cursor::cornerTopLeft:
				case gale::context::cursor::cornerTopRight:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "fd_double_arrow");
					break;
				case gale::context::cursor::cornerButtomLeft:
				case gale::context::cursor::cornerButtomRight:
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "bd_double_arrow");
					break;
				default :
					// nothing to do ... basic pointer ...
					break;
			}
		}
		/****************************************************************************************/
		void grabPointerEvents(bool _status, const vec2& _forcedPosition) {
			/*
			if (_status == true) {
				WAYLAND_DEBUG("WAYLAND-API: Grab Events");
				int32_t test = XGrabPointer(m_display,RootWindow(m_display, DefaultScreen(m_display)), True,
				                            ButtonPressMask |
				                            ButtonReleaseMask |
				                            PointerMotionMask |
				                            FocusChangeMask |
				                            EnterWindowMask |
				                            LeaveWindowMask,
				                            GrabModeAsync,
				                            GrabModeAsync,
				                            RootWindow(m_display, DefaultScreen(m_display)),
				                            None,
				                            CurrentTime);
				if (GrabSuccess != test) {
					GALE_CRITICAL("Display error " << test);
					switch (test) {
						case BadCursor:
							GALE_CRITICAL(" BadCursor");
							break;
						case BadValue:
							GALE_CRITICAL(" BadValue");
							break;
						case BadWindow:
							GALE_CRITICAL(" BadWindow");
							break;
					}
				}
				m_forcePos = _forcedPosition;
				m_forcePos.setY(m_currentHeight - m_forcePos.y());
				m_grabAllEvent = true;
				// change the pointer position to generate a good mouving at the start ...
				WAYLAND_DEBUG("WAYLAND-API: set pointer position : " << m_forcePos);
				XWarpPointer(m_display, None, m_WindowHandle, 0,0, 0, 0, m_forcePos.x(), m_forcePos.y());
				XFlush(m_display);
				m_positionChangeRequested = true;
				m_curentGrabDelta = vec2(0,0);
			} else {
				WAYLAND_DEBUG("WAYLAND-API: Un-Grab Events");
				XUngrabPointer(m_display, CurrentTime);
				m_grabAllEvent = false;
				m_forcePos = vec2(0,0);
				m_curentGrabDelta = vec2(0,0);
			}
			*/
		}
		/****************************************************************************************/
		void setIcon(const std::string& _inputFile) {
			/*
			#if    defined(GALE_BUILD_EGAMI) \
			    && !defined(__TARGET_OS__Web)
				egami::Image dataImage = egami::load(_inputFile);
				// load data
				if (dataImage.exist() == false) {
					GALE_ERROR("Error when loading Icon");
					return;
				}
				int32_t depth = DefaultDepth(m_display, DefaultScreen(m_display) );
				GALE_DEBUG("WAYLAND Create icon size=(" << dataImage.getWidth() << "," << dataImage.getHeight() << ") depth=" << depth);
				switch(depth) {
					case 8:
						GALE_CRITICAL("Not manage pixmap in 8 bit...  == > no icon ...");
						return;
					case 16:
						break;
					case 24:
						break;
					case 32:
						break;
					default:
						GALE_CRITICAL("Unknow thys type of bitDepth : " << depth);
						return;
				}
				char* tmpVal = new char[4*dataImage.getWidth()*dataImage.getHeight()];
				if (nullptr == tmpVal) {
					GALE_CRITICAL("Allocation error ...");
					return;
				}
				char* tmpPointer = tmpVal;
				switch(depth) {
					case 16:
						for(ivec2 pos(0,0); pos.y()<dataImage.getHeight(); pos.setY(pos.y()+1)) {
							for(pos.setX(0); pos.x()<dataImage.getHeight();  pos.setX(pos.x()+1)) {
								etk::Color<> tmpColor = dataImage.get(pos);
								int16_t tmpVal =   (((uint16_t)((uint16_t)tmpColor.r()>>3))<<11)
								                 + (((uint16_t)((uint16_t)tmpColor.g()>>2))<<5)
								                 +  ((uint16_t)((uint16_t)tmpColor.b()>>3));
								*tmpPointer++ = (uint8_t)(tmpVal>>8);
								*tmpPointer++ = (uint8_t)(tmpVal&0x00FF);
							}
						}
						break;
					case 24:
						for(ivec2 pos(0,0); pos.y()<dataImage.getHeight(); pos.setY(pos.y()+1)) {
							for(pos.setX(0); pos.x()<dataImage.getHeight();  pos.setX(pos.x()+1)) {
								etk::Color<> tmpColor = dataImage.get(pos);
								*tmpPointer++ = tmpColor.b();
								*tmpPointer++ = tmpColor.g();
								*tmpPointer++ = tmpColor.r();
								tmpPointer++;
							}
						}
						break;
					case 32:
						for(ivec2 pos(0,0); pos.y()<dataImage.getHeight(); pos.setY(pos.y()+1)) {
							for(pos.setX(0); pos.x()<dataImage.getHeight();  pos.setX(pos.x()+1)) {
								etk::Color<> tmpColor = dataImage.get(pos);
								*tmpPointer++ = tmpColor.a();
								*tmpPointer++ = tmpColor.b();
								*tmpPointer++ = tmpColor.g();
								*tmpPointer++ = tmpColor.r();
							}
						}
						break;
					default:
						return;
				}
				
				XImage* myImage = XCreateImage(m_display,
				                               m_visual->visual,
				                               depth,
				                               ZPixmap,
				                               0,
				                               (char*)tmpVal,
				                               dataImage.getWidth(),
				                               dataImage.getHeight(),
				                               32,
				                               0);
				
				Pixmap tmpPixmap = XCreatePixmap(m_display,
				                                 m_WindowHandle,
				                                 dataImage.getWidth(),
				                                 dataImage.getHeight(),
				                                 depth);
				switch(tmpPixmap) {
					case BadAlloc:
						GALE_ERROR("WAYLAND: BadAlloc");
						break;
					case BadDrawable:
						GALE_ERROR("WAYLAND: BadDrawable");
						break;
					case BadValue:
						GALE_ERROR("WAYLAND: BadValue");
						break;
					default:
						GALE_DEBUG("Create Pixmap OK");
						break;
				}
				GC tmpGC = DefaultGC(m_display, DefaultScreen(m_display) );
				int error = XPutImage(m_display,
				                      tmpPixmap,
				                      tmpGC,
				                      myImage,
				                      0, 0, 0, 0,
				                      dataImage.getWidth(),
				                      dataImage.getHeight());
				switch(error) {
					case BadDrawable:
						GALE_ERROR("WAYLAND: BadDrawable");
						break;
					case BadGC:
						GALE_ERROR("WAYLAND: BadGC");
						break;
					case BadMatch:
						GALE_ERROR("WAYLAND: BadMatch");
						break;
					case BadValue:
						GALE_ERROR("WAYLAND: BadValue");
						break;
					default:
						GALE_DEBUG("insert image OK");
						break;
				}
				// allocate a WM hints structure.
				XWMHints* win_hints = XAllocWMHints();
				if (win_hints == nullptr) {
					GALE_ERROR("XAllocWMHints - out of memory");
					return;
				}
				// initialize the structure appropriately. first, specify which size hints we want to fill in. in our case - setting the icon's pixmap.
				win_hints->flags = IconPixmapHint;
				// next, specify the desired hints data. in our case - supply the icon's desired pixmap.
				win_hints->icon_pixmap = tmpPixmap;
				// pass the hints to the window manager.
				XSetWMHints(m_display, m_WindowHandle, win_hints);
				GALE_INFO("     == > might be done ");
				// finally, we can free the WM hints structure.
				XFree(win_hints);
				
				// Note when we free the pixmap ... the icon is removed ...  == > this is a real memory leek ...
				//XFreePixmap(m_display, tmpPixmap);
				
				myImage->data = nullptr;
				XDestroyImage(myImage);
				delete[] tmpVal;
			#endif
			*/
		}
		/****************************************************************************************/
		void setTitle(const std::string& _title) {
			WAYLAND_INFO("WAYLAND: set Title (START)");
			m_uniqueWindowsName = _title;
			wl_shell_surface_set_title(m_shellSurface, m_uniqueWindowsName.c_str());
			WAYLAND_INFO("WAYLAND: set Title (END)");
		}
		void openURL(const std::string& _url) {
			std::string req = "xdg-open ";
			req += _url;
			system(req.c_str());
			return;
		}
		/****************************************************************************************/
		void clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			/*
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					if (m_clipBoardOwnerPrimary == false) {
						m_clipBoardRequestPrimary = true;
						// generate a request on WAYLAND
						XConvertSelection(m_display,
						                  XAtomSelection,
						                  XAtomTargetStringUTF8,
						                  XAtomGALE,
						                  m_WindowHandle,
						                  CurrentTime);
					} else {
						// just transmit an event , we have the data in the system
						OS_ClipBoardArrive(_clipboardID);
					}
					break;
				case gale::context::clipBoard::clipboardStd:
					if (m_clipBoardOwnerStd == false) {
						m_clipBoardRequestPrimary = false;
						// generate a request on WAYLAND
						XConvertSelection(m_display,
						                  XAtomClipBoard,
						                  XAtomTargetStringUTF8,
						                  XAtomGALE,
						                  m_WindowHandle,
						                  CurrentTime);
					} else {
						// just transmit an event , we have the data in the system
						OS_ClipBoardArrive(_clipboardID);
					}
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
			*/
		}
		/****************************************************************************************/
		void clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			/*
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					// Request the selection :
					if (m_clipBoardOwnerPrimary == false) {
						XSetSelectionOwner(m_display, XAtomSelection, m_WindowHandle, CurrentTime);
						m_clipBoardOwnerPrimary = true;
					}
					break;
				case gale::context::clipBoard::clipboardStd:
					// Request the clipBoard :
					if (m_clipBoardOwnerStd == false) {
						XSetSelectionOwner(m_display, XAtomClipBoard, m_WindowHandle, CurrentTime);
						m_clipBoardOwnerStd = true;
					}
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
			*/
		}
};
static void global_registry_handler(void* _data, struct wl_registry* _registry, uint32_t _id, const char* _interface, uint32_t _version) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->registryHandler(_registry, _id, _interface, _version);
}

static void global_registry_remover(void* _data, struct wl_registry* _registry, uint32_t _id) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->registryRemover(_registry, _id);
}

static void seat_handle_capabilities(void* _data, struct wl_seat* _seat, uint32_t _caps) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->seatHandleCapabilities(_seat, (enum wl_seat_capability)_caps);
}

static void pointer_handle_enter(void* _data, struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface, wl_fixed_t _sx, wl_fixed_t _sy) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleEnter(_pointer, _serial, _surface, ivec2(_sx, _sy));
}
static void pointer_handle_leave(void* _data, struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleLeave(_pointer, _serial, _surface);
}

static void pointer_handle_motion(void* _data, struct wl_pointer* _pointer, uint32_t _time, wl_fixed_t _sx, wl_fixed_t _sy) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleMotion(_pointer, _time, ivec2(_sx/256, _sy/256));
}

static void pointer_handle_button(void* _data, struct wl_pointer* _pointer, uint32_t _serial, uint32_t _time, uint32_t _button, uint32_t _state) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleButton(_pointer,
	                               _serial,
	                               _time,
	                               _button,
	                               _state==WL_POINTER_BUTTON_STATE_PRESSED);
}

static void pointer_handle_axis(void* _data, struct wl_pointer* _pointer, uint32_t _time, uint32_t _axis, wl_fixed_t _value) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleAxis(_pointer, _time, _axis, _value);
}

static void redraw(void* _data, struct wl_callback* _callback, uint32_t _time) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->redraw(_callback, _time);
}

static void configure_callback(void* _data, struct wl_callback* _callback, uint32_t _time) {
	GALE_WARNING("callback ...");
	
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->configureCallback(_callback, _time);
}

static void handle_ping(void* _data, struct wl_shell_surface* _shellSurface, uint32_t _serial) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->handlePing(_shellSurface, _serial);
}

static void handle_configure(void* _data, struct wl_shell_surface* _shellSurface, uint32_t _edges, int32_t _width, int32_t _height) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->handleConfigure(_shellSurface, _edges, ivec2(_width, _height));
}

static void handle_popup_done(void* _data, struct wl_shell_surface* _shellSurface) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->handlePopupDone(_shellSurface);
}

static void keyboard_handle_keymap(void* _data, struct wl_keyboard* _keyboard, uint32_t _format, int _fd, uint32_t _size) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->keyboardKeymap(_keyboard, _format, _fd, _size);
}

static void keyboard_handle_enter(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface, struct wl_array* _keys) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->keyboardEnter(_keyboard, _serial, _surface, _keys);
}

static void keyboard_handle_leave(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->keyboardLeave(_keyboard, _serial, _surface);
}

static void keyboard_handle_key(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _time, uint32_t _key, uint32_t _state) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->keyboardKey(_keyboard, _serial, _time, _key, _state);
}

static void keyboard_handle_modifiers(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _modsDepressed, uint32_t _modsLatched, uint32_t _modsLocked, uint32_t _group) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->keyboardModifiers(_keyboard, _serial, _modsDepressed, _modsLatched, _modsLocked, _group);
}


int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	WAYLANDInterface* interface = new WAYLANDInterface(_application, _argc, _argv);
	if (interface == nullptr) {
		GALE_CRITICAL("Can not create the WAYLAND interface ... MEMORY allocation error");
		return -2;
	}
	int32_t retValue = interface->run();
	delete(interface);
	interface = nullptr;
	return retValue;
}
