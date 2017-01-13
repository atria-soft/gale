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


bool g_runningSignal = true;

static void
signal_int(int signum)
{
	g_runningSignal = false;
}


struct window;
struct seat;

struct display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *shell;
	struct wl_seat *seat;
	struct wl_pointer *pointer;
	struct wl_keyboard *keyboard;
	struct wl_shm *shm;
	struct wl_cursor_theme *cursor_theme;
	struct wl_cursor *default_cursor;
	struct wl_surface *cursor_surface;
	struct {
		EGLDisplay dpy;
		EGLContext ctx;
		EGLConfig conf;
	} egl;
	struct window *window;
};

struct geometry {
	int width, height;
};

struct window {
	struct display *display;
	struct geometry geometry, window_size;
	struct {
		GLuint rotation_uniform;
		GLuint pos;
		GLuint col;
	} gl;

	struct wl_egl_window *native;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	EGLSurface egl_surface;
	struct wl_callback *callback;
	int fullscreen, configured, opaque;
};

static const char *vert_shader_text =
	"uniform mat4 rotation;\n"
	"attribute vec4 pos;\n"
	"attribute vec4 color;\n"
	"varying vec4 v_color;\n"
	"void main() {\n"
	"  gl_Position = rotation * pos;\n"
	"  v_color = color;\n"
	"}\n";

static const char *frag_shader_text =
	"precision mediump float;\n"
	"varying vec4 v_color;\n"
	"void main() {\n"
	"  gl_FragColor = v_color;\n"
	"}\n";

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
static void pointer_handle_button(void* _data, struct wl_pointer* _wl_pointer, uint32_t _serial, uint32_t _time, uint32_t _button, uint32_t _state);
static void pointer_handle_axis(void* _data, struct wl_pointer* _wl_pointer, uint32_t _time, uint32_t _axis, wl_fixed_t _value);
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

static void handle_ping(void* _data, struct wl_shell_surface* _shell_surface, uint32_t _serial);
static void handle_configure(void* _data, struct wl_shell_surface* _shell_surface, uint32_t _edges, int32_t _width, int32_t _height);
static void handle_popup_done(void* _data, struct wl_shell_surface* _shell_surface);
static const struct wl_shell_surface_listener shell_surface_listener = {
	handle_ping,
	handle_configure,
	handle_popup_done
};


static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size) {
	GALE_WARNING("callback ...");
	GALE_INFO("KEY MAP : '" << format << "'");
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys) {
	GALE_WARNING("callback ...");
	
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface) {
	GALE_WARNING("callback ...");
	
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
	GALE_WARNING("callback ...");
	GALE_INFO("KEY : '" << key << "'");
	/*
	struct display *d = (struct display *)data;
	if (key == KEY_F11 && state)
		toggle_fullscreen(d->window, d->window->fullscreen ^ 1);
	else if (key == KEY_ESC && state)
		running = 0;
	*/
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
	
}

static const struct wl_keyboard_listener keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};


class WAYLANDInterface : public gale::Context {
	private:
		gale::key::Special m_guiKeyBoardMode;
		ivec2 m_size;
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		std::string m_uniqueWindowsName;
		bool m_run;
		struct sigaction m_sigint;
		struct display m_display;
		struct window m_window;
		
		bool m_fullscreen;
		bool m_opaque;
		
	public:
		WAYLANDInterface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv),
		  m_size(800,600),
		  m_run(false),
		  m_fullscreen(false),
		  m_opaque(false) {
			// in case ...
			g_runningSignal = true;
			GALE_WARNING("WAYLAND: INIT [START]");
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				m_inputIsPressed[iii] = false;
			}
			memset(&m_sigint, 0, sizeof(struct sigaction));
			memset(&m_display, 0, sizeof(struct display));
			memset(&m_window, 0, sizeof(struct window));
			int i, ret = 0;
			
			m_window.display = &m_display;
			m_display.window = &m_window;
			m_window.window_size.width  = 250;
			m_window.window_size.height = 250;
			
			m_display.display = wl_display_connect(nullptr);
			assert(m_display.display);
			
			m_display.registry = wl_display_get_registry(m_display.display);
			wl_registry_add_listener(m_display.registry, &registry_listener, this);
			
			wl_display_dispatch(m_display.display);
			initEgl(m_window.opaque);
			
			createSurface();
			initGL();
			
			m_display.cursor_surface = wl_compositor_create_surface(m_display.compositor);
			
			m_sigint.sa_handler = signal_int;
			sigemptyset(&m_sigint.sa_mask);
			m_sigint.sa_flags = SA_RESETHAND;
			sigaction(SIGINT, &m_sigint, nullptr);
			
			m_uniqueWindowsName   = "GALE_" + etk::to_string(etk::tool::irand(0, 1999999999));
			m_run = true;
			GALE_WARNING("WAYLAND: INIT [STOP]");
		}
		
		~WAYLANDInterface() {
			destroySurface();
			unInitEgl();
			wl_surface_destroy(m_display.cursor_surface);
			if (m_display.cursor_theme) {
				wl_cursor_theme_destroy(m_display.cursor_theme);
			}
			if (m_display.shell) {
				wl_shell_destroy(m_display.shell);
			}
			if (m_display.compositor) {
				wl_compositor_destroy(m_display.compositor);
			}
			wl_registry_destroy(m_display.registry);
			wl_display_flush(m_display.display);
			wl_display_disconnect(m_display.display);
		}
		/****************************************************************************************/
		int32_t run() {
			int ret = 0;
			while (    g_runningSignal == true
			        && m_run == true
			        && ret != -1) {
				ret = wl_display_dispatch(m_display.display);
				GALE_INFO("loop dispatch event " << ret);
			}
			GALE_INFO("Normal application exit ...");
			return 0;
		}
		
		GLuint create_shader(const char* _source, GLenum _shaderType) {
			GLuint shader = glCreateShader(_shaderType);
			assert(shader != 0);
			glShaderSource(shader, 1, (const char **) &_source, nullptr);
			glCompileShader(shader);
			GLint status;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (!status) {
				char log[1000];
				GLsizei len;
				glGetShaderInfoLog(shader, 1000, &len, log);
				fprintf(stderr, "Error: compiling %s: %*s\n", _shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",	len, log);
				exit(1);
			}
			return shader;
		}
		void initGL() {
			GALE_WARNING("initGL: [START]");
			GLuint frag, vert;
			GLuint program;
			GLint status;
			frag = create_shader(frag_shader_text, GL_FRAGMENT_SHADER);
			vert = create_shader(vert_shader_text, GL_VERTEX_SHADER);
			program = glCreateProgram();
			glAttachShader(program, frag);
			glAttachShader(program, vert);
			glLinkProgram(program);
			glGetProgramiv(program, GL_LINK_STATUS, &status);
			if (!status) {
				char log[1000];
				GLsizei len;
				glGetProgramInfoLog(program, 1000, &len, log);
				fprintf(stderr, "Error: linking:\n%*s\n", len, log);
				exit(1);
			}
			glUseProgram(program);
			m_window.gl.pos = 0;
			m_window.gl.col = 1;
			glBindAttribLocation(program, m_window.gl.pos, "pos");
			glBindAttribLocation(program, m_window.gl.col, "color");
			glLinkProgram(program);
			m_window.gl.rotation_uniform = glGetUniformLocation(program, "rotation");
			GALE_WARNING("initGL: [STOP]");
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
			m_display.egl.dpy = eglGetDisplay(m_display.display);
			assert(m_display.egl.dpy);
			
			if (eglInitialize(m_display.egl.dpy, &major, &minor) != EGL_TRUE) {
				GALE_CRITICAL("Can't initialise egl display");
				return;
			}
			if(eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) {
				GALE_CRITICAL("Can't initialise Bind");
				return;
			}
			EGLint nnn;
			EGLBoolean ret = eglChooseConfig(m_display.egl.dpy, config_attribs, &m_display.egl.conf, 1, &nnn);
			/*
			EGLint count = 0;
			eglGetConfigs(m_egl_display, nullptr, 0, &count);
			GALE_INFO("EGL has " << count << " configs");
			EGLConfig* configs = (EGLConfig*)calloc(count, sizeof *configs);
			GALE_INFO("Display all configs:");
			for (int32_t iii=0; iii<nnn; ++iii) {
				EGLint size = 0;
				EGLint sizeRed = 0;
				eglGetConfigAttrib(m_display.egl.dpy, configs[iii], EGL_BUFFER_SIZE, &size);
				eglGetConfigAttrib(m_display.egl.dpy, configs[iii], EGL_RED_SIZE, &sizeRed);
				GALE_INFO("    " << iii << "     BufferSize=" << size << "     red size=" << sizeRed);
			}
			*/
			//assert(ret && n == 1);
			
			m_display.egl.ctx = eglCreateContext(m_display.egl.dpy, m_display.egl.conf, EGL_NO_CONTEXT, context_attribs);
			assert(m_display.egl.ctx);
			GALE_INFO("Init EGL [STOP]");
		}
		
		void unInitEgl() {
			GALE_INFO("un-Init EGL [START]");
			/* Required, otherwise segfault in egl_dri2.c: dri2_make_current() on eglReleaseThread(). */
			eglMakeCurrent(m_display.egl.dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			eglTerminate(m_display.egl.dpy);
			eglReleaseThread();
			GALE_INFO("un-Init EGL [STOP]");
		}
		
		
		void createSurface() {
			GALE_INFO("CRATE the SURFACE [START]");
			EGLBoolean ret;
			m_window.surface = wl_compositor_create_surface(m_display.compositor);
			m_window.shell_surface = wl_shell_get_shell_surface(m_display.shell, m_window.surface);
			wl_shell_surface_add_listener(m_window.shell_surface, &shell_surface_listener, this);
			m_window.native = wl_egl_window_create(m_window.surface, m_window.window_size.width, m_window.window_size.height);
			m_window.egl_surface = eglCreateWindowSurface(m_display.egl.dpy, m_display.egl.conf, m_window.native, nullptr);
			wl_shell_surface_set_title(m_window.shell_surface, "simple-egl");
			ret = eglMakeCurrent(m_display.egl.dpy, m_window.egl_surface, m_window.egl_surface, m_display.egl.ctx);
			assert(ret == EGL_TRUE);
			toggleFullscreen(m_window.fullscreen);
			GALE_INFO("CRATE the SURFACE [STOP]");
		}
		
		void destroySurface() {
			GALE_INFO("DESTROY the SURFACE [START]");
			wl_egl_window_destroy(m_window.native);
			wl_shell_surface_destroy(m_window.shell_surface);
			wl_surface_destroy(m_window.surface);
			if (m_window.callback) {
				wl_callback_destroy(m_window.callback);
			}
			GALE_INFO("DESTROY the SURFACE [STOP]");
		}
		
		void toggleFullscreen(int _fullscreen) {
			GALE_INFO("toggleFullscreen [START]");
			m_window.fullscreen = _fullscreen;
			m_window.configured = 0;
			if (_fullscreen) {
				wl_shell_surface_set_fullscreen(m_window.shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, nullptr);
			} else {
				wl_shell_surface_set_toplevel(m_window.shell_surface);
				handleConfigure(m_window.shell_surface, 0, ivec2(m_window.window_size.width, m_window.window_size.height));
			}
			struct wl_callback *callback = wl_display_sync(m_display.display);
			wl_callback_add_listener(callback, &configure_callback_listener, this);
			//wl_callback_add_listener(callback, &configure_callback_listener, &m_window);
			GALE_INFO("toggleFullscreen [STOP]");
		}
		
		/****************************************************************************************/
		// recorder on elements ...
		
		void registryHandler(struct wl_registry* _registry, uint32_t _id, const char* _interface, uint32_t _version) {
			GALE_WARNING("Got a registry event for '" << _interface  << "' id=" << _id);
			if (strcmp(_interface, "wl_compositor") == 0) {
				m_display.compositor = (struct wl_compositor *)wl_registry_bind(_registry, _id, &wl_compositor_interface, 1);
			} else if (strcmp(_interface, "wl_shell") == 0) {
				m_display.shell = (struct wl_shell*)wl_registry_bind(_registry, _id, &wl_shell_interface, 1);
			} else if (strcmp(_interface, "wl_seat") == 0) {
				m_display.seat = (struct wl_seat*)wl_registry_bind(_registry, _id, &wl_seat_interface, 1);
				wl_seat_add_listener(m_display.seat, &seat_listener, this);
			} else if (strcmp(_interface, "wl_shm") == 0) {
				m_display.shm = (struct wl_shm*)wl_registry_bind(_registry, _id, &wl_shm_interface, 1);
				m_display.cursor_theme = wl_cursor_theme_load(nullptr, 32, m_display.shm);
				m_display.default_cursor = wl_cursor_theme_get_cursor(m_display.cursor_theme, "left_ptr");
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
			if ((_caps & WL_SEAT_CAPABILITY_POINTER) && !m_display.pointer) {
				GALE_WARNING("Display has a pointer");
				m_display.pointer = wl_seat_get_pointer(_seat);
				wl_pointer_add_listener(m_display.pointer, &pointer_listener, this);
				//wl_pointer_add_listener(m_display.pointer, &pointer_listener, &m_display);
			} else if (!(_caps & WL_SEAT_CAPABILITY_POINTER) && m_display.pointer) {
				GALE_WARNING("Display has No more pointer");
				wl_pointer_destroy(m_display.pointer);
				m_display.pointer = nullptr;
			}
			if ((_caps & WL_SEAT_CAPABILITY_KEYBOARD) && !m_display.keyboard) {
				GALE_WARNING("Display has a keyboard");
				m_display.keyboard = wl_seat_get_keyboard(_seat);
				wl_keyboard_add_listener(m_display.keyboard, &keyboard_listener, this);
				//wl_keyboard_add_listener(m_display.keyboard, &keyboard_listener, &m_display);
			} else if (!(_caps & WL_SEAT_CAPABILITY_KEYBOARD) && m_display.keyboard) {
				wl_keyboard_destroy(m_display.keyboard);
				m_display.keyboard = nullptr;
			}
			
			if (_caps & WL_SEAT_CAPABILITY_TOUCH) {
				GALE_WARNING("Display has a touch screen");
			}
		}
		
		/****************************************************************************************/
		// Pointer section event ...
		
		void pointerHandleEnter(struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface, ivec2 _pos) {
			GALE_WARNING("Pointer Enter surface" << _surface << " at pos=" << _pos);
			struct wl_buffer *buffer;
			struct wl_cursor *cursor = m_display.default_cursor;
			struct wl_cursor_image *image;
			
			if (m_window.fullscreen)
				wl_pointer_set_cursor(_pointer, _serial, nullptr, 0, 0);
			else if (cursor) {
				image = m_display.default_cursor->images[0];
				buffer = wl_cursor_image_get_buffer(image);
				wl_pointer_set_cursor(_pointer, _serial, m_display.cursor_surface, image->hotspot_x, image->hotspot_y);
				wl_surface_attach(m_display.cursor_surface, buffer, 0, 0);
				wl_surface_damage(m_display.cursor_surface, 0, 0, image->width, image->height);
				wl_surface_commit(m_display.cursor_surface);
			}
			GALE_WARNING("Pointer enter [STOP]");
		}
		void pointerHandleLeave(struct wl_pointer* _pointer, uint32_t _serial, struct wl_surface* _surface) {
			GALE_WARNING("Pointer left surface" << _surface);
		}
		void pointerHandleMotion(struct wl_pointer* _pointer, uint32_t _time, ivec2 _pos) {
			GALE_WARNING("Pointer moved at " << _pos);
		}
		void pointerHandleButton(struct wl_pointer* _wl_pointer, uint32_t _serial, uint32_t _time, uint32_t _button, uint32_t _state) {
			GALE_WARNING("Pointer button");
			#if 0
			if (    _button == BTN_LEFT
			     && _state == WL_POINTER_BUTTON_STATE_PRESSED) {
				wl_shell_surface_move(m_window.shell_surface, m_display.seat, _serial);
			}
			#endif
		}
		void pointerHandleAxis(struct wl_pointer* _wl_pointer, uint32_t _time, uint32_t _axis, wl_fixed_t _value) {
			GALE_WARNING("Pointer handle axis");
		}
		/****************************************************************************************/
		
		void redraw(struct wl_callback* _callback, uint32_t _time) {
			GALE_WARNING("REDRAW [START]");
			static const GLfloat verts[3][2] = {
				{ -0.5, -0.5 },
				{  0.5, -0.5 },
				{  0,    0.5 }
			};
			static const GLfloat colors[3][3] = {
				{ 1, 0, 0 },
				{ 0, 1, 0 },
				{ 0, 0, 1 }
			};
			GLfloat angle;
			GLfloat rotation[4][4] = {
				{ 1, 0, 0, 0 },
				{ 0, 1, 0, 0 },
				{ 0, 0, 1, 0 },
				{ 0, 0, 0, 1 }
			};
			static const int32_t speed_div = 5;
			static uint32_t start_time = 0;
			
			
			assert(m_window.callback == _callback);
			m_window.callback = nullptr;
			if (_callback) {
				wl_callback_destroy(_callback);
			}
			if (!m_window.configured) {
				return;
			}
			if (start_time == 0) {
				start_time = _time;
			}
			angle = ((_time-start_time) / speed_div) % 360 * M_PI / 180.0;
			rotation[0][0] =  cos(angle);
			rotation[0][2] =  sin(angle);
			rotation[2][0] = -sin(angle);
			rotation[2][2] =  cos(angle);
			
			glViewport(0, 0, m_window.geometry.width, m_window.geometry.height);
			
			glUniformMatrix4fv(m_window.gl.rotation_uniform, 1, GL_FALSE, (GLfloat *) rotation);
			
			glClearColor(0.0, 0.0, 0.0, 0.5);
			glClear(GL_COLOR_BUFFER_BIT);
			
			glVertexAttribPointer(m_window.gl.pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
			glVertexAttribPointer(m_window.gl.col, 3, GL_FLOAT, GL_FALSE, 0, colors);
			glEnableVertexAttribArray(m_window.gl.pos);
			glEnableVertexAttribArray(m_window.gl.col);
			
			glDrawArrays(GL_TRIANGLES, 0, 3);
			
			glDisableVertexAttribArray(m_window.gl.pos);
			glDisableVertexAttribArray(m_window.gl.col);
			
			if (    m_window.opaque
			     || m_window.fullscreen) {
				struct wl_region *region = (struct wl_region *)wl_compositor_create_region(m_display.compositor);
				wl_region_add(region, 0, 0, m_window.geometry.width, m_window.geometry.height);
				wl_surface_set_opaque_region(m_window.surface, region);
				wl_region_destroy(region);
			} else {
				wl_surface_set_opaque_region(m_window.surface, nullptr);
			}
			m_window.callback = wl_surface_frame(m_window.surface);
			wl_callback_add_listener(m_window.callback, &frame_listener, this);
			eglSwapBuffers(m_display.egl.dpy, m_window.egl_surface);
			GALE_WARNING("REDRAW [STOP]");
		}
		
		void configureCallback(struct wl_callback* _callback, uint32_t _time) {
			wl_callback_destroy(_callback);
			m_window.configured = 1;
			if (m_window.callback == nullptr) {
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
			if (m_window.native != nullptr) {
				wl_egl_window_resize(m_window.native, _size.x(), _size.y(), 0, 0);
			}
			m_size = _size;
			m_window.geometry.width = _size.x();
			m_window.geometry.height = _size.y();
			if (!m_window.fullscreen) {
				m_window.window_size = m_window.geometry;
			}
			GALE_WARNING("configure [STOP]");
		}
		
		void handlePopupDone(struct wl_shell_surface* _shell_surface) {
			GALE_WARNING("Pop-up done");
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
			/*
			XEvent event;
			event.xclient.type = ClientMessage;
			event.xclient.serial = 0;
			event.xclient.send_event = True;
			event.xclient.display = m_display;
			event.xclient.window = m_WindowHandle;
			event.xclient.message_type = XInternAtom(m_display, "_NET_WM_STATE", False);
			event.xclient.format = 32;
			if (_status == true) {
				event.xclient.data.l[0] = 1;//XInternAtom(m_display, "_NET_WM_STATE_REMOVE", False);
			} else {
				event.xclient.data.l[0] = 0;//XInternAtom(m_display, "_NET_WM_STATE_ADD", False);
			}
			event.xclient.data.l[1] = XInternAtom(m_display, "_NET_WM_STATE_FULLSCREEN", False);
			event.xclient.data.l[2] = 0;
			event.xclient.data.l[3] = 0;
			event.xclient.data.l[4] = 0;
			
			//long mask = SubstructureNotifyMask;
			//long mask = StructureNotifyMask | ResizeRedirectMask;
			//long mask = SubstructureRedirectMask;
			long mask = PropertyChangeMask;
			
			XSendEvent(m_display,
			           RootWindow(m_display, DefaultScreen(m_display)),
			           False,
			           mask,
			           &event);
			// associate the keyboard grabing (99% associated case)
			grabKeyboardEvents(_status);
			*/
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
			/*
			if (_newCursor != m_currentCursor) {
				WAYLAND_DEBUG("WAYLAND-API: set New Cursor : " << _newCursor);
				// undefine previous cursors ...
				XUndefineCursor(m_display, m_WindowHandle);
				// set the new one :
				m_currentCursor = _newCursor;
				Cursor myCursor = None;
				switch (m_currentCursor) {
					case gale::context::cursor::none:
						{
							Pixmap bitmapNoData;
							XColor black;
							static char noData[] = { 0,0,0,0,0,0,0,0 };
							black.red = 0;
							black.green = 0;
							black.blue = 0;
							bitmapNoData = XCreateBitmapFromData(m_display, m_WindowHandle, noData, 8, 8);
							myCursor = XCreatePixmapCursor(m_display, bitmapNoData, bitmapNoData, 
							                               &black, &black, 0, 0);
						}
						break;
					case gale::context::cursor::leftArrow:
						myCursor = XCreateFontCursor(m_display, XC_top_left_arrow);
						break;
					case gale::context::cursor::info:
						myCursor = XCreateFontCursor(m_display, XC_hand1);
						break;
					case gale::context::cursor::destroy:
						myCursor = XCreateFontCursor(m_display, XC_pirate);
						break;
					case gale::context::cursor::help:
						myCursor = XCreateFontCursor(m_display, XC_question_arrow);
						break;
					case gale::context::cursor::cycle:
						myCursor = XCreateFontCursor(m_display, XC_exchange);
						break;
					case gale::context::cursor::spray:
						myCursor = XCreateFontCursor(m_display, XC_spraycan);
						break;
					case gale::context::cursor::wait:
						myCursor = XCreateFontCursor(m_display, XC_watch);
						break;
					case gale::context::cursor::text:
						myCursor = XCreateFontCursor(m_display, XC_xterm);
						break;
					case gale::context::cursor::crossHair:
						myCursor = XCreateFontCursor(m_display, XC_crosshair);
						break;
					case gale::context::cursor::slideUpDown:
						myCursor = XCreateFontCursor(m_display, XC_sb_v_double_arrow);
						break;
					case gale::context::cursor::slideLeftRight:
						myCursor = XCreateFontCursor(m_display, XC_sb_h_double_arrow);
						break;
					case gale::context::cursor::resizeUp:
						myCursor = XCreateFontCursor(m_display, XC_top_side);
						break;
					case gale::context::cursor::resizeDown:
						myCursor = XCreateFontCursor(m_display, XC_bottom_side);
						break;
					case gale::context::cursor::resizeLeft:
						myCursor = XCreateFontCursor(m_display, XC_left_side);
						break;
					case gale::context::cursor::resizeRight:
						myCursor = XCreateFontCursor(m_display, XC_right_side);
						break;
					case gale::context::cursor::cornerTopLeft:
						myCursor = XCreateFontCursor(m_display, XC_top_left_corner);
						break;
					case gale::context::cursor::cornerTopRight:
						myCursor = XCreateFontCursor(m_display, XC_top_right_corner);
						break;
					case gale::context::cursor::cornerButtomLeft:
						myCursor = XCreateFontCursor(m_display, XC_bottom_right_corner);
						break;
					case gale::context::cursor::cornerButtomRight:
						myCursor = XCreateFontCursor(m_display, XC_bottom_left_corner);
						break;
					default :
						// nothing to do ... basic pointer ...
						break;
				}
				if (myCursor != None) {
					XDefineCursor(m_display, m_WindowHandle, myCursor);
					XFreeCursor(m_display, myCursor);
				}
			}
			*/
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
			/*
			XTextProperty tp;
			tp.value = (unsigned char *)_title.c_str();
			tp.encoding = XA_WM_NAME;
			tp.format = 8;
			tp.nitems = strlen((const char*)tp.value);
			XSetWMName(m_display, m_WindowHandle, &tp);
			XStoreName(m_display, m_WindowHandle, (const char*)tp.value);
			XSetIconName(m_display, m_WindowHandle, (const char*)tp.value);
			XSetWMIconName(m_display, m_WindowHandle, &tp);
			*/
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
	interface->pointerHandleMotion(_pointer, _time, ivec2(_sx, _sy));
}

static void pointer_handle_button(void* _data, struct wl_pointer* _wl_pointer, uint32_t _serial, uint32_t _time, uint32_t _button, uint32_t _state) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleButton(_wl_pointer, _serial, _time, _button, _state);
}

static void pointer_handle_axis(void* _data, struct wl_pointer* _wl_pointer, uint32_t _time, uint32_t _axis, wl_fixed_t _value) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->pointerHandleAxis(_wl_pointer, _time, _axis, _value);
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

static void handle_ping(void* _data, struct wl_shell_surface* _shell_surface, uint32_t _serial) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->handlePing(_shell_surface, _serial);
}

static void handle_configure(void* _data, struct wl_shell_surface* _shell_surface, uint32_t _edges, int32_t _width, int32_t _height) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->handleConfigure(_shell_surface, _edges, ivec2(_width, _height));
}

static void handle_popup_done(void* _data, struct wl_shell_surface* _shell_surface) {
	GALE_WARNING("callback ...");
	WAYLANDInterface* interface = (WAYLANDInterface*)_data;
	if (interface == nullptr) {
		GALE_ERROR("    ==> nullptr");
		return;
	}
	interface->handlePopupDone(_shell_surface);
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

