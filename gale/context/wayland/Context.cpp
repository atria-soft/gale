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

// this does not work now ...
#define GALE_XKB_WRAPPER_INPUT
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
	// XKB layout wrapping of keyboard ...
	#ifdef GALE_XKB_WRAPPER_INPUT
		#include <xkbcommon/xkbcommon.h>
		#include <xkbcommon/xkbcommon-compose.h>
		#include <sys/mman.h> // form mmap
		#include <unistd.h> // need for close
	#endif
	#include <poll.h>
}

#include <gale/renderer/openGL/openGL-include.hpp>

static bool hasDisplay = false;
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



struct data_offer {
	struct wl_data_offer *offer;
	struct wl_array types;
	int refcount;
	//struct task io_task;
	int fd;
	//data_func_t func;
	int32_t x, y;
	uint32_t dnd_action;
	uint32_t source_actions;
	void *user_data;
};

static void data_offer_offer(void* _data, struct wl_data_offer* _wl_data_offer, const char* _type) {
	GALE_VERBOSE("plop 1 '" << _type << "'");
	/*
	struct data_offer *offer = (struct data_offer *)_data;
	char **p;
	p = (char **)wl_array_add(&offer->types, sizeof *p);
	*p = strdup(_type);
	*/
}

static void data_offer_source_actions(void* _data, struct wl_data_offer* _wl_data_offer, uint32_t _source_actions) {
	GALE_VERBOSE("plop 2 : " << _source_actions);
	/*
	struct data_offer *offer = (struct data_offer *)_data;
	offer->source_actions = _source_actions;
	*/
}

static void data_offer_action(void* _data, struct wl_data_offer* _wl_data_offer, uint32_t _dnd_action) {
	GALE_VERBOSE("plop 3 : " << _dnd_action);
	/*
	struct data_offer *offer = (struct data_offer *)_data;
	offer->dnd_action = _dnd_action;
	*/
}

static const struct wl_data_offer_listener data_offer_listener = {
	data_offer_offer,
	data_offer_source_actions,
	data_offer_action
};

static void data_offer_destroy(struct data_offer* _offer) {
	char **p;
	_offer->refcount--;
	if (_offer->refcount == 0) {
		wl_data_offer_destroy(_offer->offer);
		for (p = (char **)_offer->types.data; *p; p++) {
			free(*p);
		}
		wl_array_release(&_offer->types);
		free(_offer);
		_offer = nullptr;
	}
}

static void data_device_data_offer(void* _data, struct wl_data_device* _data_device, struct wl_data_offer* _offer);
static void data_device_enter(void* _data, struct wl_data_device* _data_device, uint32_t _serial, struct wl_surface* _surface, wl_fixed_t _x_w, wl_fixed_t _y_w, struct wl_data_offer* _offer);
static void data_device_leave(void* _data, struct wl_data_device* _data_device);
static void data_device_motion(void* _data, struct wl_data_device* _data_device, uint32_t _time, wl_fixed_t _x_w, wl_fixed_t _y_w);
static void data_device_drop(void* _data, struct wl_data_device* _data_device);
static void data_device_selection(void* _data, struct wl_data_device* _wl_data_device, struct wl_data_offer* _offer);
static const struct wl_data_device_listener data_device_listener = {
	data_device_data_offer,
	data_device_enter,
	data_device_leave,
	data_device_motion,
	data_device_drop,
	data_device_selection
};


static void data_source_target(void* _data, struct wl_data_source* _wl_data_source, const char* _mime_type);
static void data_source_send(void* _data, struct wl_data_source* _wl_data_source, const char* _mime_type, int _fd);
static void data_source_cancelled(void* _data, struct wl_data_source* _wl_data_source);
static const struct wl_data_source_listener data_source_listener = {
	data_source_target,
	data_source_send,
	data_source_cancelled
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
		char32_t m_lastKeyPressed;
		
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
		struct wl_data_device_manager* m_dataDeviceManager;
		int32_t m_dataDeviceManagerVersion;
		struct wl_data_device *m_dataDevice;
		int32_t m_serial; //!< Unique ID of the serial element (based on the enter time)
		bool m_offerIsInside; //!< Offer request in inside the windows
		bool m_offerInternalCopy; //!< The windows own th copy buffer
		struct wl_data_offer* m_offerCopy;
		struct wl_data_source* m_dataSource;
		// EGL interface:
		EGLDisplay m_eglDisplay;
		EGLContext m_eglContext;
		EGLConfig m_eglConfig;
		EGLSurface m_eglSurface;
		#ifdef GALE_XKB_WRAPPER_INPUT
			struct xkb_context* m_XKBContext;
			struct xkb_keymap* m_XKBKeymap;
			struct xkb_state* m_XKBState;
		#endif
	public:
		WAYLANDInterface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv),
		  m_size(800,600),
		  m_run(false),
		  m_fullscreen(false),
		  m_configured(false),
		  m_opaque(false),
		  m_cursorCurrent(gale::context::cursor::leftArrow),
		  m_lastKeyPressed(0),
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
		  m_callback(nullptr),
		  m_dataDeviceManager(nullptr),
		  m_dataDeviceManagerVersion(0),
		  m_dataDevice(nullptr),
		  m_serial(0),
		  m_offerIsInside(false),
		  m_offerInternalCopy(false),
		  m_offerCopy(nullptr),
		  m_dataSource(nullptr)
		#ifdef GALE_XKB_WRAPPER_INPUT
		  ,m_XKBContext(nullptr),
		  m_XKBKeymap(nullptr),
		  m_XKBState(nullptr)
		#endif
		  {
			// in case ...
			GALE_WARNING("WAYLAND: INIT [START]");
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				m_inputIsPressed[iii] = false;
			}
			int i, ret = 0;
			#ifdef GALE_XKB_WRAPPER_INPUT
				m_XKBContext = xkb_context_new(XKB_CONTEXT_NO_DEFAULT_INCLUDES);
				if (m_XKBContext == nullptr) {
					GALE_CRITICAL("Couldn't create xkb context");
				}
			#endif
			m_display = wl_display_connect(nullptr);
			assert(m_display);
			
			m_registry = wl_display_get_registry(m_display);
			wl_registry_add_listener(m_registry, &registry_listener, this);
			
			wl_display_dispatch(m_display);
			initEgl(m_opaque);
			
			createSurface();
			
			m_cursorSurface = wl_compositor_create_surface(m_compositor);
			
			// set the DPI for the current screen: TODO : do it with real value, for now, we use a generic dpi value (most common screen)
			gale::Dimension::setPixelRatio(vec2(75,75),gale::distance::inch);
			
			m_uniqueWindowsName = "GALE_" + etk::to_string(etk::tool::irand(0, 1999999999));
			m_run = true;
			GALE_WARNING("WAYLAND: INIT [STOP]");
		}
		
		~WAYLANDInterface() {
			#ifdef GALE_XKB_WRAPPER_INPUT
				if (m_XKBState != nullptr) {
					xkb_state_unref(m_XKBState);
					m_XKBState = nullptr;
				}
				if (m_XKBKeymap != nullptr) {
					xkb_keymap_unref(m_XKBKeymap);
					m_XKBKeymap = nullptr;
				}
				if (m_XKBContext != nullptr) {
					xkb_context_unref(m_XKBContext);
					m_XKBContext = nullptr;
				}
			#endif
			destroySurface();
			unInitEgl();
			wl_surface_destroy(m_cursorSurface);
			if (m_cursorTheme != nullptr) {
				wl_cursor_theme_destroy(m_cursorTheme);
				m_cursorTheme = nullptr;
			}
			if (m_shell != nullptr) {
				wl_shell_destroy(m_shell);
				m_shell = nullptr;
			}
			if (m_compositor != nullptr) {
				wl_compositor_destroy(m_compositor);
				m_compositor = nullptr;
			}
			if (m_dataDeviceManager != nullptr) {
				wl_data_device_manager_destroy(m_dataDeviceManager);
				m_dataDeviceManager = nullptr;
			}
			if (m_registry != nullptr) {
				wl_registry_destroy(m_registry);
				m_registry = nullptr;
			}
			if (m_display != nullptr) {
				wl_display_flush(m_display);
				wl_display_disconnect(m_display);
				m_display = nullptr;
			}
		}
		/****************************************************************************************/
		int32_t run() {
			int ret = 0;
			while (    m_run == true
			        && ret != -1) {
				ret = wl_display_dispatch(m_display);
				//GALE_INFO("loop dispatch event " << ret);
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
			GALE_DEBUG("Got a registry event for '" << _interface  << "' id=" << _id);
			if (strcmp(_interface, "wl_compositor") == 0) {
				m_compositor = (struct wl_compositor *)wl_registry_bind(_registry, _id, &wl_compositor_interface, 1);
			} else if (strcmp(_interface, "wl_shell") == 0) {
				m_shell = (struct wl_shell*)wl_registry_bind(_registry, _id, &wl_shell_interface, 1);
			} else if (strcmp(_interface, "wl_seat") == 0) {
				m_seat = (struct wl_seat*)wl_registry_bind(_registry, _id, &wl_seat_interface, 1);
				wl_seat_add_listener(m_seat, &seat_listener, this);
				if (m_dataDeviceManager != nullptr) {
					m_dataDevice = wl_data_device_manager_get_data_device(m_dataDeviceManager, m_seat);
					wl_data_device_add_listener(m_dataDevice, &data_device_listener, this);
					m_dataSource = wl_data_device_manager_create_data_source(m_dataDeviceManager);
					wl_data_source_add_listener(m_dataSource, &data_source_listener, this);
				}
			} else if (strcmp(_interface, "wl_shm") == 0) {
				m_shm = (struct wl_shm*)wl_registry_bind(_registry, _id, &wl_shm_interface, 1);
				m_cursorTheme = wl_cursor_theme_load(nullptr, 32, m_shm);
				if (m_cursorTheme != nullptr) {
					m_cursorDefault = wl_cursor_theme_get_cursor(m_cursorTheme, "left_ptr");
				} else {
					GALE_WARNING("Can not get the generic theme");
				}
			} else if (strcmp(_interface, "wl_data_device_manager") == 0) {
				m_dataDeviceManagerVersion = std::min(3, int32_t(_version));
				m_dataDeviceManager = (struct wl_data_device_manager*)wl_registry_bind(_registry, _id, &wl_data_device_manager_interface, m_dataDeviceManagerVersion);
			} else {
				GALE_WARNING("    ==> Not used ... '" << _interface  << "'");
			}
			GALE_DEBUG("registry_handle_global [STOP]");
		}
		
		void registryRemover(struct wl_registry* _registry, uint32_t _id) {
			GALE_WARNING("Got a registry losing event for " << _id);
		}
		
		/****************************************************************************************/
		// screen capabilities
		void seatHandleCapabilities(struct wl_seat* _seat, enum wl_seat_capability _caps) {
			if ((_caps & WL_SEAT_CAPABILITY_POINTER) && !m_pointer) {
				GALE_DEBUG("Display has a pointer");
				m_pointer = wl_seat_get_pointer(_seat);
				wl_pointer_add_listener(m_pointer, &pointer_listener, this);
			} else if (!(_caps & WL_SEAT_CAPABILITY_POINTER) && m_pointer) {
				GALE_DEBUG("Display has No more pointer");
				wl_pointer_destroy(m_pointer);
				m_pointer = nullptr;
			}
			if ((_caps & WL_SEAT_CAPABILITY_KEYBOARD) && !m_keyboard) {
				GALE_DEBUG("Display has a keyboard");
				m_keyboard = wl_seat_get_keyboard(_seat);
				wl_keyboard_add_listener(m_keyboard, &keyboard_listener, this);
			} else if (!(_caps & WL_SEAT_CAPABILITY_KEYBOARD) && m_keyboard) {
				wl_keyboard_destroy(m_keyboard);
				m_keyboard = nullptr;
				#ifdef GALE_XKB_WRAPPER_INPUT
					if (m_XKBState != nullptr) {
						xkb_state_unref(m_XKBState);
						m_XKBState = nullptr;
					}
					if (m_XKBKeymap != nullptr) {
						xkb_keymap_unref(m_XKBKeymap);
						m_XKBKeymap = nullptr;
					}
				#endif
			}
			
			if (_caps & WL_SEAT_CAPABILITY_TOUCH) {
				GALE_DEBUG("Display has a touch screen");
			}
		}
		
		/****************************************************************************************/
		// Pointer section event ...
		
		void pointerHandleEnter(struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface, ivec2 _pos) {
			m_cursorCurrentPosition = vec2(_pos.x(), m_size.y()-_pos.y());
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
			m_offerIsInside = true;
		}
		void pointerHandleLeave(struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface) {
			GALE_VERBOSE("Pointer left surface" << _surface);
			m_offerIsInside = false;
		}
		void pointerHandleMotion(struct wl_pointer* _pointer, uint32_t _time, ivec2 _pos) {
			m_cursorCurrentPosition = vec2(_pos.x(), m_size.y()-_pos.y());
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
			int32_t idButton = -1;
			m_serial = _time;
			switch (_button) {
				case BTN_LEFT:   idButton = 1; break;
				case BTN_MIDDLE: idButton = 2; break;
				case BTN_RIGHT:  idButton = 3; break;
				default: GALE_ERROR("unknow button:" << _button); break;
			}
			if (idButton == -1) {
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
			int32_t idButton = -1;
			// scroll up and down .... 
			if (_axis == WL_POINTER_AXIS_VERTICAL_SCROLL) {
				if (_value > 0) {
					idButton = 5;
				} else {
					idButton = 4;
				}
			} else if (_axis == WL_POINTER_AXIS_HORIZONTAL_SCROLL) {
				if (_value > 0) {
					idButton = 11;
				} else {
					idButton = 10;
				}
			} else {
				GALE_ERROR("Unknow the axix mode of this button " << _axis);
				return;
			}
			OS_SetInput(gale::key::type::mouse,
			            gale::key::status::down,
			            idButton,
			            m_cursorCurrentPosition);
			OS_SetInput(gale::key::type::mouse,
			            gale::key::status::up,
			            idButton,
			            m_cursorCurrentPosition);
		}
		/****************************************************************************************/
		
		void redraw(struct wl_callback* _callback, uint32_t _time) {
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
		void keyboardKeymap(struct wl_keyboard* _keyboard, enum wl_keyboard_keymap_format _format, int _fd, uint32_t _size) {
			//GALE_INFO("KEY MAP : '" << _format << "'");
			switch (_format) {
				case WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP:
					GALE_ERROR("NO keymap: client must understand how to interpret the raw keycode");
				case WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1:
					GALE_DEBUG("XKB_V1: Memory on the keymap use ... XKB-v1");
			}
			#ifdef GALE_XKB_WRAPPER_INPUT
				void *buf;
				buf = mmap(nullptr, _size, PROT_READ, MAP_SHARED, _fd, 0);
				if (buf == MAP_FAILED) {
					GALE_ERROR("Failed to mmap keymap: " << errno);
					close(_fd);
					return;
				}
				m_XKBKeymap = xkb_keymap_new_from_buffer(m_XKBContext, (const char *)buf, _size - 1, XKB_KEYMAP_FORMAT_TEXT_V1, (enum xkb_keymap_compile_flags)0);
				munmap(buf, _size);
				close(_fd);
				if (!m_XKBKeymap) {
					GALE_ERROR("Failed to compile XKB keymap");
					return;
				}
				m_XKBState = xkb_state_new(m_XKBKeymap);
				if (!m_XKBState) {
					GALE_ERROR("Failed to create XKB state");
					return;
				}
			#else
				// use internal wrapper:
				GALE_INFO("KEY MAP size=" << _size);
			#endif
		}
		
		void keyboardEnter(struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface, struct wl_array* _keys) {
			GALE_VERBOSE("keyboard Enter...");
		}
		
		void keyboardLeave(struct wl_keyboard* _keyboard, uint32_t _serial, struct wl_surface* _surface) {
			GALE_VERBOSE("keyboardLeave...");
		}
		#if 0
		void test_print_keycode_state(struct xkb_state* _state,
		                              struct xkb_compose_state* _composeState,
		                              xkb_keycode_t _keycode,
		                              enum xkb_consumed_mode _consumedMode) {
			struct xkb_keymap *keymap;
			xkb_keysym_t sym;
			const xkb_keysym_t *syms;
			int nsyms;
			char s[16];
			xkb_layout_index_t layout;
			enum xkb_compose_status status;
			keymap = xkb_state_get_keymap(_state);
			nsyms = xkb_state_key_get_syms(_state, _keycode, &syms);
			if (nsyms <= 0) {
				return;
			}
			status = XKB_COMPOSE_NOTHING;
			sym = xkb_state_key_get_one_sym(_state, _keycode);
			syms = &sym;
			printf("keysyms [ ");
			for (int i = 0; i < nsyms; i++) {
				xkb_keysym_get_name(syms[i], s, sizeof(s));
				printf("%-*s ", (int) sizeof(s), s);
			}
			printf("] ");
			
			xkb_state_key_get_utf8(_state, _keycode, s, sizeof(s));
			printf("unicode [ %s ] ", s);
			layout = xkb_state_key_get_layout(_state, _keycode);
			printf("layout [ %s (%d) ] ",
			xkb_keymap_layout_get_name(keymap, layout), layout);
			printf("level [ %d ] ",
			xkb_state_key_get_level(_state, _keycode, layout));
			printf("mods [ ");
			for (xkb_mod_index_t mod = 0; mod < xkb_keymap_num_mods(keymap); mod++) {
				if (xkb_state_mod_index_is_active(_state, mod, XKB_STATE_MODS_EFFECTIVE) <= 0) {
					continue;
				}
				if (xkb_state_mod_index_is_consumed2(_state, _keycode, mod, _consumedMode)) {
					printf("-%s ", xkb_keymap_mod_get_name(keymap, mod));
				} else {
					printf("%s ", xkb_keymap_mod_get_name(keymap, mod));
				}
			}
			printf("] ");
			
			printf("leds [ ");
			for (xkb_led_index_t led = 0; led < xkb_keymap_num_leds(keymap); led++) {
				if (xkb_state_led_index_is_active(_state, led) <= 0) {
					continue;
				}
				printf("%s ", xkb_keymap_led_get_name(keymap, led));
			}
			printf("] ");
			printf("\n");
		}
		#endif
		void keyboardKey(struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _time, uint32_t _key, bool _isDown) {
			#if 0
				if (_isDown == true) {
					test_print_keycode_state(m_XKBState, nullptr, _key + 8, XKB_CONSUMED_MODE_GTK);
					/* Exit on ESC. */
					if (xkb_state_key_get_one_sym(m_XKBState, _key + 8) == XKB_KEY_Escape) {
						m_run = false;
					}
				}
			#endif
			GALE_VERBOSE("KEY : '" << _key << "' _isDown=" << _isDown);
			bool find = true;
			enum gale::key::keyboard keyInput;
			switch (_key) {
				//case 80: // keypad
				case KEY_UP:          keyInput = gale::key::keyboard::up;            break;
				//case 83: // keypad
				case KEY_LEFT:        keyInput = gale::key::keyboard::left;          break;
				//case 85: // keypad
				case KEY_RIGHT:       keyInput = gale::key::keyboard::right;         break;
				//case 88: // keypad
				case KEY_DOWN:        keyInput = gale::key::keyboard::down;          break;
				//case 81: // keypad
				case KEY_PAGEUP:      keyInput = gale::key::keyboard::pageUp;        break;
				//case 89: // keypad
				case KEY_PAGEDOWN:    keyInput = gale::key::keyboard::pageDown;      break;
				//case 79: // keypad
				case KEY_HOME:        keyInput = gale::key::keyboard::start;         break;
				case KEY_END:         keyInput = gale::key::keyboard::end;           break;
				case KEY_SCROLLLOCK:  keyInput = gale::key::keyboard::stopDefil;     break;
				case KEY_PAUSE:       keyInput = gale::key::keyboard::wait;          break;
				// screen shot ...
				//case KEY_SYSRQ:       keyInput = gale::key::keyboard::screenShot;    break;
				//case 90: // keypad
				case KEY_INSERT:
					keyInput = gale::key::keyboard::insert;
					if(_isDown == false) {
						if (m_guiKeyBoardMode.getInsert() == true) {
							m_guiKeyBoardMode.setInsert(false);
						} else {
							m_guiKeyBoardMode.setInsert(true);
						}
					}
					break;
				//case 84:  keyInput = gale::key::keyboardCenter; break; // Keypad
				case KEY_F1:     keyInput = gale::key::keyboard::f1; break;
				case KEY_F2:     keyInput = gale::key::keyboard::f2; break;
				case KEY_F3:     keyInput = gale::key::keyboard::f3; break;
				case KEY_F4:     keyInput = gale::key::keyboard::f4; break;
				case KEY_F5:     keyInput = gale::key::keyboard::f5; break;
				case KEY_F6:     keyInput = gale::key::keyboard::f6; break;
				case KEY_F7:     keyInput = gale::key::keyboard::f7; break;
				case KEY_F8:     keyInput = gale::key::keyboard::f8; break;
				case KEY_F9:     keyInput = gale::key::keyboard::f9; break;
				case KEY_F10:    keyInput = gale::key::keyboard::f10; break;
				case KEY_F11:    keyInput = gale::key::keyboard::f11; break;
				case KEY_F12:    keyInput = gale::key::keyboard::f12; break;
				case KEY_CAPSLOCK:    keyInput = gale::key::keyboard::capLock;     m_guiKeyBoardMode.setCapsLock(_isDown); break;
				case KEY_LEFTSHIFT:   keyInput = gale::key::keyboard::shiftLeft;   m_guiKeyBoardMode.setShift   (_isDown); break;
				case KEY_RIGHTSHIFT:  keyInput = gale::key::keyboard::shiftRight;  m_guiKeyBoardMode.setShift   (_isDown); break;
				case KEY_LEFTCTRL:    keyInput = gale::key::keyboard::ctrlLeft;    m_guiKeyBoardMode.setCtrl    (_isDown); break;
				case KEY_RIGHTCTRL:   keyInput = gale::key::keyboard::ctrlRight;   m_guiKeyBoardMode.setCtrl    (_isDown); break;
				case KEY_LEFTMETA:    keyInput = gale::key::keyboard::metaLeft;    m_guiKeyBoardMode.setMeta    (_isDown); break;
				case KEY_RIGHTMETA:   keyInput = gale::key::keyboard::metaRight;   m_guiKeyBoardMode.setMeta    (_isDown); break;
				case KEY_LEFTALT:     keyInput = gale::key::keyboard::alt;         m_guiKeyBoardMode.setAlt     (_isDown); break;
				case KEY_RIGHTALT:    keyInput = gale::key::keyboard::altGr;       m_guiKeyBoardMode.setAltGr   (_isDown); break;
				case KEY_COMPOSE:     keyInput = gale::key::keyboard::contextMenu; break;
				case KEY_NUMLOCK:     keyInput = gale::key::keyboard::numLock;     m_guiKeyBoardMode.setNumLock (_isDown); break;
				case KEY_DELETE: // Suppr on keypad
					find = false;
					OS_setKeyboard(m_guiKeyBoardMode,
					               gale::key::keyboard::character,
					               (_isDown==true?gale::key::status::down:gale::key::status::up),
					               false,
					               0x7F);
					break;
				case KEY_KPDOT: // Suppr on keypad
					find = false;
					if(m_guiKeyBoardMode.getNumLock() == true){
						OS_setKeyboard(m_guiKeyBoardMode,
						               gale::key::keyboard::character,
						               (_isDown==true?gale::key::status::down:gale::key::status::up),
						               false,
						               '.');
					} else {
						OS_setKeyboard(m_guiKeyBoardMode,
						               gale::key::keyboard::character,
						               (_isDown==true?gale::key::status::down:gale::key::status::up),
						               false,
						               0x7F);
					}
					break;
				case KEY_TAB: // special case for TAB
					find = false;
					OS_setKeyboard(m_guiKeyBoardMode,
					               gale::key::keyboard::character,
					               (_isDown==true?gale::key::status::down:gale::key::status::up),
					               false,
					               0x09);
					break;
				default:
					{
						xkb_keysym_t sym = xkb_state_key_get_one_sym(m_XKBState, _key + 8);
						char buf[16];
						xkb_state_key_get_utf8(m_XKBState, _key + 8, buf, 16);
						if (buf[0] == '\r') {
							buf[0] = '\n';
							buf[1] = '\0';
						}
						if (buf[0] != '\0') {
							GALE_DEBUG("KEY : val='" << buf << "' _isDown=" << _isDown);
							m_lastKeyPressed = utf8::convertChar32(buf);
							GALE_DEBUG("KEY : _key='" << _key << "' val='" << buf << "'='" << m_lastKeyPressed <<"' _isDown=" << _isDown << "  " << m_guiKeyBoardMode);
							OS_setKeyboard(m_guiKeyBoardMode,
							               gale::key::keyboard::character,
							               (_isDown==true?gale::key::status::down:gale::key::status::up),
							               false,
							               m_lastKeyPressed);
							m_lastKeyPressed = 0;
						} else {
							find = true;
							switch(sym) {
								case XKB_KEY_dead_circumflex: m_lastKeyPressed = '^'; break;
								case XKB_KEY_dead_diaeresis: m_lastKeyPressed = '"'; break;
								//case XKB_KEY_ISO_Level3_Shif: m_lastKeyPressed = '~'; break;
								//case XKB_KEY_: m_lastKeyPressed = ''; break;
								default:
									GALE_ERROR("UNKNOW KEY : sym='" << sym << "' _isDown=" << _isDown);
									find = false;
									break;
							}
							if (find == false) {
								//GALE_INFO("KEY : sym='" << sym << "' _isDown=" << _isDown);
							} else {
								OS_setKeyboard(m_guiKeyBoardMode,
								               gale::key::keyboard::character,
								               (_isDown==true?gale::key::status::down:gale::key::status::up),
								               false,
								               m_lastKeyPressed);
							}
						}
						// must use xkbcommon library to manage correct map ...
					}
					find = false;
			}
			if (find == true) {
				GALE_DEBUG("    ==> " << keyInput);
				OS_setKeyboard(m_guiKeyBoardMode,
				               keyInput,
				               (_isDown==true?gale::key::status::down:gale::key::status::up),
				               false);
			}
		}
		
		void keyboardModifiers(struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _modsDepressed, uint32_t _modsLatched, uint32_t _modsLocked, uint32_t _group) {
			GALE_DEBUG("keyboard Modifiers...  _modsDepressed=" << _modsDepressed << " _modsLatched=" << _modsLatched << " _modsLocked=" << _modsLocked << " group=" << _group);
			GALE_VERBOSE("         _modsDepressed=" << _modsDepressed);
			GALE_VERBOSE("                 0x80 = " << ((_modsDepressed&0x80)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x40 = " << ((_modsDepressed&0x40)!=0?"true":"false") << "  Meta");
			GALE_VERBOSE("                 0x20 = " << ((_modsDepressed&0x20)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x10 = " << ((_modsDepressed&0x10)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x08 = " << ((_modsDepressed&0x08)!=0?"true":"false") << "  ALT");
			GALE_VERBOSE("                 0x04 = " << ((_modsDepressed&0x04)!=0?"true":"false") << "  ctrl");
			GALE_VERBOSE("                 0x02 = " << ((_modsDepressed&0x02)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x01 = " << ((_modsDepressed&0x01)!=0?"true":"false") << "  shift");
			GALE_VERBOSE("         _modsLatched=" << _modsLatched);
			GALE_VERBOSE("                 0x80 = " << ((_modsLatched&0x80)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x40 = " << ((_modsLatched&0x40)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x20 = " << ((_modsLatched&0x20)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x10 = " << ((_modsLatched&0x10)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x08 = " << ((_modsLatched&0x08)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x04 = " << ((_modsLatched&0x04)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x02 = " << ((_modsLatched&0x02)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x01 = " << ((_modsLatched&0x01)!=0?"true":"false") << "  ");
			GALE_VERBOSE("         _modsLocked=" << _modsLocked);
			GALE_VERBOSE("                 0x80 = " << ((_modsLocked&0x80)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x40 = " << ((_modsLocked&0x40)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x20 = " << ((_modsLocked&0x20)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x10 = " << ((_modsLocked&0x10)!=0?"true":"false") << "  VER num");
			GALE_VERBOSE("                 0x08 = " << ((_modsLocked&0x08)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x04 = " << ((_modsLocked&0x04)!=0?"true":"false") << "  ");
			GALE_VERBOSE("                 0x02 = " << ((_modsLocked&0x02)!=0?"true":"false") << "  CAP-lock");
			GALE_VERBOSE("                 0x01 = " << ((_modsLocked&0x01)!=0?"true":"false") << "  ");
			if ((_modsLocked&0x02)!=0) {
				m_guiKeyBoardMode.setCapsLock(true);
			} else {
				m_guiKeyBoardMode.setCapsLock(false);
			}
			if ((_modsDepressed&0x01)!=0) {
				m_guiKeyBoardMode.setShift(true);
			} else {
				m_guiKeyBoardMode.setShift(false);
			}
			if ((_modsDepressed&0x04)!=0) {
				m_guiKeyBoardMode.setCtrl(true);
			} else {
				m_guiKeyBoardMode.setCtrl(false);
			}
			if ((_modsDepressed&0x40)!=0) {
				m_guiKeyBoardMode.setMeta(true);
			} else {
				m_guiKeyBoardMode.setMeta(false);
			}
			if ((_modsDepressed&0x08)!=0) {
				m_guiKeyBoardMode.setAlt(true);
			} else {
				m_guiKeyBoardMode.setAlt(false);
			}
			// this is not specific ...
			m_guiKeyBoardMode.setAltGr(false);
			if ((_modsLocked&0x10)!=0) {
				m_guiKeyBoardMode.setNumLock(true);
			} else {
				m_guiKeyBoardMode.setNumLock(false);
			}
			GALE_INFO("        ==> " << m_guiKeyBoardMode);
			#ifdef GALE_XKB_WRAPPER_INPUT
				// remove the ctrl check ==> create many error in parsing specific short-cut ... (ex ctrl+v change in  a unknow ascii value ...)
				xkb_state_update_mask(m_XKBState, _modsDepressed & 0xFB, _modsLatched, _modsLocked, 0, 0, _group);
			#endif
		}
		
		
		void dataDeviceDataOffer(struct wl_data_device* _data_device, struct wl_data_offer* _offer) {
			GALE_VERBOSE("CALL : data_device_data_offer");
			if (m_offerIsInside == true) {
				//We get our own copy or drag & drop ...
				m_offerInternalCopy = true;
			} else {
				m_offerInternalCopy = false;
			}
			if (m_offerCopy != nullptr) {
				//data_offer_destroy(m_offerCopy);
				wl_data_offer_destroy(m_offerCopy);
				m_offerCopy = nullptr;
			}
			if (_offer == nullptr) {
				GALE_ERROR("    nullptr offer");
			} else {
				m_offerCopy = _offer;
				wl_data_offer_add_listener(_offer, &data_offer_listener, this);
			}
		}
		
		void dataDeviceEnter(struct wl_data_device* _data_device, uint32_t _serial, struct wl_surface* _surface, vec2 _pos, struct wl_data_offer* _offer) {
			GALE_VERBOSE("CALL : data_device_enter (drag & drop)");
			/*
			struct input *input = data;
			struct window *window;
			void *types_data;
			float x = wl_fixed_to_double(x_w);
			float y = wl_fixed_to_double(y_w);
			char **p;
			window = wl_surface_get_user_data(surface);
			input->drag_enter_serial = serial;
			input->drag_focus = window,
			input->drag_x = x;
			input->drag_y = y;
			if (!input->touch_grab) {
				input->pointer_enter_serial = serial;
			}
			if (offer) {
				input->drag_offer = wl_data_offer_get_user_data(offer);
				p = wl_array_add(&input->drag_offer->types, sizeof *p);
				*p = nullptr;
				types_data = input->drag_offer->types.data;
				if (input->display->data_device_manager_version >= WL_DATA_OFFER_SET_ACTIONS_SINCE_VERSION) {
					wl_data_offer_set_actions(offer,
					                            WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY
					                          | WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE,
					                          WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE);
				}
			} else {
				input->drag_offer = nullptr;
				types_data = nullptr;
			}
			if (window->data_handler) {
				window->data_handler(window, input, x, y, types_data, window->user_data);
			}
			*/
		}
		
		void dataDeviceLeave(struct wl_data_device* _data_device) {
			GALE_VERBOSE("CALL : data_device_leave (drag & drop)");
			/*
			struct input *input = data;
			if (input->drag_offer) {
				data_offer_destroy(input->drag_offer);
				input->drag_offer = nullptr;
			}
			*/
		}
		
		void dataDeviceMotion(struct wl_data_device* _data_device, uint32_t _time, vec2 _pos) {
			GALE_VERBOSE("CALL : data_device_motion (drag & drop)");
			/*
			struct input *input = data;
			struct window *window = input->drag_focus;
			float x = wl_fixed_to_double(x_w);
			float y = wl_fixed_to_double(y_w);
			void *types_data;
			input->drag_x = x;
			input->drag_y = y;
			if (input->drag_offer) {
				types_data = input->drag_offer->types.data;
			} else {
				types_data = nullptr;
			}
			if (window->data_handler) {
				window->data_handler(window, input, x, y, types_data, window->user_data);
			}
			*/
		}
		
		void dataDeviceDrop(struct wl_data_device* _data_device) {
			GALE_VERBOSE("CALL : data_device_drop (drag & drop)");
			/*
			struct input *input = data;
			struct window *window = input->drag_focus;
			float x, y;
			x = input->drag_x;
			y = input->drag_y;
			if (window->drop_handler) {
				window->drop_handler(window, input, x, y, window->user_data);
			}
			if (input->touch_grab) {
				touch_ungrab(input);
			}
			*/
		}
		
		void dataDeviceSelection(struct wl_data_device* _wl_data_device, struct wl_data_offer* _offer) {
			GALE_VERBOSE("CALL : data_device_selection");
			// I do not understand what it is used for ???
		}
		
		
		void dataSourceTarget(struct wl_data_source* _wl_data_source, const char* _mime_type) {
			GALE_VERBOSE("CALL : dataSourceTarget " << _mime_type);
		}
		void dataSourceSend(struct wl_data_source* _wl_data_source, const char* _mime_type, int _fd) {
			GALE_VERBOSE("CALL : dataSourceSend " << _mime_type);
			std::string data = gale::context::clipBoard::get(gale::context::clipBoard::clipboardStd);
			if (_fd == 0) {
				GALE_ERROR("    ==> No data availlable ...");
				return;
			}
			// simple is best ...
			write(_fd, data.c_str(), data.size());
			GALE_VERBOSE("    ==> send " << data.size() << " Bytes");
			close(_fd);
			GALE_VERBOSE("    ==> Close done");
		}
		void dataSourceCancelled(struct wl_data_source* _wl_data_source) {
			GALE_VERBOSE("CALL : dataSourceCancelled");
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
			if (m_cursorTheme == nullptr) {
				GALE_WARNING("WAYLAND-API: set New Cursor : " << _newCursor << " missing acces on theme ...");
				return;
			}
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
			if (m_shellSurface == nullptr) {
				GALE_ERROR("WAYLAND: set Title (END) ==> missing surface pointer");
				return;
			}
			wl_shell_surface_set_title(m_shellSurface, m_uniqueWindowsName.c_str());
			WAYLAND_INFO("WAYLAND: set Title (END)");
		}
		void openURL(const std::string& _url) {
			std::string req = "xdg-open ";
			req += _url;
			system(req.c_str());
			return;
		}
		bool dataPaste() {
			GALE_VERBOSE("Request PAST ...");
			if(m_offerCopy == nullptr) {
				return false;
			} 
			GALE_VERBOSE("Request PAST .2.");
			int pipe_fd[2] = {0, 0};
			if (pipe(pipe_fd) == -1) {
				return false;
			}
			GALE_VERBOSE("Request PAST .3.");
			
			//MIME must be from the ones returned by data_offer_offer()
			//wl_data_offer_receive(m_offerCopy, "TEXT", pipe_fd[1]);
			//wl_data_offer_receive(m_offerCopy, "text/plain", pipe_fd[1]);
			wl_data_offer_receive(m_offerCopy, "text/plain;charset=utf-8", pipe_fd[1]);
			close(pipe_fd[1]);
			wl_display_flush(m_display);
			
			int len = 1;
			char buffer[4097];
			buffer[0] = '\0';
			std::string localBufferData;
			
			struct pollfd fds;
			fds.fd = pipe_fd[0];
			fds.events = POLLIN;
			
			// Choose some reasonable timeout here in ms
			int ret = poll(&fds, 1, 300);
			if (    ret == -1
			     || ret == 0) {
				GALE_WARNING("Can not open FD ...");
				return false;
			}
			buffer[0] = '\0';
			while (len > 0) {
				len = read(pipe_fd[0], buffer, 4096);
				if (len > 0) {
					buffer[len] = '\0';
					localBufferData += buffer;
				}
			}
			close(pipe_fd[0]);
			pipe_fd[0] = 0;
			if (localBufferData.size() != 0) {
				GALE_VERBOSE("Clipboard data: '" << localBufferData << "' len = " << localBufferData.size());
				gale::context::clipBoard::setSystem(gale::context::clipBoard::clipboardStd, localBufferData);
				return true;
			} else {
				GALE_ERROR("can not past data ...");
			}
			return false;
		}
		/****************************************************************************************/
		void clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					OS_ClipBoardArrive(_clipboardID);
					break;
				case gale::context::clipBoard::clipboardStd:
					if (m_offerInternalCopy == false) {
						if (dataPaste() == false) {
							gale::context::clipBoard::setSystem(gale::context::clipBoard::clipboardSelection, "");
						}
					}
					OS_ClipBoardArrive(_clipboardID);
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
		/****************************************************************************************/
		void clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					// Use internal middle button ...
					break;
				case gale::context::clipBoard::clipboardStd:
					// Request the clipBoard:
					GALE_VERBOSE("Request copy ...");
					// Destroy any existing data source
					if (m_dataSource != nullptr) {
						wl_data_source_destroy(m_dataSource);
						m_dataSource = nullptr;
					}
					// try to create a new data source
					m_dataSource = wl_data_device_manager_create_data_source(m_dataDeviceManager);
					if (m_dataSource == nullptr) {
						GALE_ERROR("Can not create the data source interface");
						return;
					}
					// set the value of availlable elements:
					wl_data_source_offer(m_dataSource, "UTF8_STRING");
					wl_data_source_offer(m_dataSource, "COMPOUND_TEXT");
					wl_data_source_offer(m_dataSource, "TEXT");
					wl_data_source_offer(m_dataSource, "STRING");
					wl_data_source_offer(m_dataSource, "text/plain;charset=utf-8");
					wl_data_source_offer(m_dataSource, "text/plain");
					// add a listener for data source events
					wl_data_source_add_listener(m_dataSource, &data_source_listener, this);
					// set the selection
					wl_data_device_set_selection(m_dataDevice, m_dataSource, m_serial++);
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
};


static void global_registry_handler(void* _data, struct wl_registry* _registry, uint32_t _id, const char* _interface, uint32_t _version) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->registryHandler(_registry, _id, _interface, _version);
}

static void global_registry_remover(void* _data, struct wl_registry* _registry, uint32_t _id) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->registryRemover(_registry, _id);
}

static void seat_handle_capabilities(void* _data, struct wl_seat* _seat, uint32_t _caps) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->seatHandleCapabilities(_seat, (enum wl_seat_capability)_caps);
}

static void pointer_handle_enter(void* _data, struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface, wl_fixed_t _sx, wl_fixed_t _sy) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleEnter(_pointer, _serial, _surface, ivec2(_sx, _sy));
}

static void pointer_handle_leave(void* _data, struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleLeave(_pointer, _serial, _surface);
}

static void pointer_handle_motion(void* _data, struct wl_pointer* _pointer, uint32_t _time, wl_fixed_t _sx, wl_fixed_t _sy) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleMotion(_pointer, _time, ivec2(_sx/256, _sy/256));
}

static void pointer_handle_button(void* _data, struct wl_pointer* _pointer, uint32_t _serial, uint32_t _time, uint32_t _button, uint32_t _state) {
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
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleAxis(_pointer, _time, _axis, _value);
}

static void redraw(void* _data, struct wl_callback* _callback, uint32_t _time) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->redraw(_callback, _time);
}

static void configure_callback(void* _data, struct wl_callback* _callback, uint32_t _time) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->configureCallback(_callback, _time);
}

static void handle_ping(void* _data, struct wl_shell_surface* _shellSurface, uint32_t _serial) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->handlePing(_shellSurface, _serial);
}

static void handle_configure(void* _data, struct wl_shell_surface* _shellSurface, uint32_t _edges, int32_t _width, int32_t _height) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->handleConfigure(_shellSurface, _edges, ivec2(_width-1, _height));
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
	interface->keyboardKeymap(_keyboard, (enum wl_keyboard_keymap_format)_format, _fd, _size);
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
	interface->keyboardKey(_keyboard,
	                       _serial,
	                       _time,
	                       _key,
	                       _state==WL_KEYBOARD_KEY_STATE_PRESSED);
}

static void keyboard_handle_modifiers(void* _data, struct wl_keyboard* _keyboard, uint32_t _serial, uint32_t _modsDepressed, uint32_t _modsLatched, uint32_t _modsLocked, uint32_t _group) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->keyboardModifiers(_keyboard, _serial, _modsDepressed, _modsLatched, _modsLocked, _group);
}

static void data_device_data_offer(void* _data, struct wl_data_device* _data_device, struct wl_data_offer* _offer) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataDeviceDataOffer(_data_device, _offer);
}

static void data_device_enter(void* _data, struct wl_data_device* _data_device, uint32_t _serial, struct wl_surface* _surface, wl_fixed_t _x_w, wl_fixed_t _y_w, struct wl_data_offer* _offer) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataDeviceEnter(_data_device, _serial, _surface, vec2(_x_w,_y_w), _offer);
}

static void data_device_leave(void* _data, struct wl_data_device* _data_device) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataDeviceLeave(_data_device);
}

static void data_device_motion(void* _data, struct wl_data_device* _data_device, uint32_t _time, wl_fixed_t _x_w, wl_fixed_t _y_w) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataDeviceMotion(_data_device, _time, vec2(_x_w,_y_w));
}

static void data_device_drop(void* _data, struct wl_data_device* _data_device) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataDeviceDrop(_data_device);
}

static void data_device_selection(void* _data, struct wl_data_device* _wl_data_device, struct wl_data_offer* _offer) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataDeviceSelection(_wl_data_device, _offer);
}

static void data_source_target(void* _data, struct wl_data_source* _wl_data_source, const char* _mime_type) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataSourceTarget(_wl_data_source, _mime_type);
}
static void data_source_send(void* _data, struct wl_data_source* _wl_data_source, const char* _mime_type, int _fd) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataSourceSend(_wl_data_source, _mime_type, _fd);
}
static void data_source_cancelled(void* _data, struct wl_data_source* _wl_data_source) {
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		return;
	}
	interface->dataSourceCancelled(_wl_data_source);
}


#include <gale/context/wayland/Context.hpp>

bool gale::context::wayland::isBackendPresent() {
	// TODO : Do it better...
	return true;
}

ememory::SharedPtr<gale::Context> gale::context::wayland::createInstance(gale::Application* _application, int _argc, const char *_argv[]) {
	return ememory::makeShared<WAYLANDInterface>(_application, _argc, _argv);
}


