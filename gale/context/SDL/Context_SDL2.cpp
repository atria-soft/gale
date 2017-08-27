/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


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

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
//#include <SDL/SDL_opengl.h>
#include <SDL/SDL_opengles2.h>
#include <emscripten.h>

bool hasDisplay = false;
//#define DEBUG_SDL_EVENT
#ifdef DEBUG_SDL_EVENT
	#define SDL_DEBUG      GALE_DEBUG
	#define SDL_VERBOSE    GALE_VERBOSE
	#define SDL_INFO       GALE_INFO
	#define SDL_CRITICAL   GALE_CRITICAL
#else
	#define SDL_DEBUG      GALE_VERBOSE
	#define SDL_VERBOSE    GALE_VERBOSE
	#define SDL_INFO       GALE_VERBOSE
	#define SDL_CRITICAL   GALE_VERBOSE
#endif

#ifdef GALE_BUILD_EGAMI
	#include <egami/egami.hpp>
#endif

int result = 1;

static char *TouchFingerTypeToString(int type) {
  if (type == SDL_FINGERMOTION) return "SDL_FINGERMOTION";
  if (type == SDL_FINGERDOWN) return "SDL_FINGERDOWN";
  if (type == SDL_FINGERUP) return "SDL_FINGERUP";
  return "UNKNOWN";
}

int got_down = 0;
int got_move = 0;
int got_up = 0;


void progress() {
  if (!got_down) printf("Hold down a finger to generate a touch down event.\n");
  else if (!got_move) printf("Drag a finger to generate a touch move event.\n");
  else if (!got_up) printf("Release a finger to generate a touch up event.\n");
  else
  {
    int result = 0;
#ifdef REPORT_RESULT
    REPORT_RESULT();
#endif
  }
}

void loopppp(void* arg) {
  
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_FINGERMOTION:
      case SDL_FINGERDOWN:
      case SDL_FINGERUP: {
        SDL_TouchFingerEvent *t = (SDL_TouchFingerEvent*)&event;
        printf("type: %s, timestamp: %u, touchId: %llu, fingerId: %llu, x: %f, y: %f, dx: %f, dy: %f, pressure: %f\n",
          TouchFingerTypeToString(event.type), t->timestamp, t->touchId, t->fingerId, t->x, t->y, t->dx, t->dy, t->pressure);

        if (t->timestamp != 0 && t->x >= 0.f && t->x <= 1.f && t->y >= 0.f && t->y <= 1.f && t->pressure >= 0.f && t->pressure <= 1.f) {
          if (event.type == SDL_FINGERDOWN) { got_down = 1; progress(); }
          if (event.type == SDL_FINGERMOTION) { got_move = 1; progress(); }
          if (event.type == SDL_FINGERDOWN) { got_up = 1; progress(); }
        }
        break;
      }
    }
  }
}


class SDLInterface : public gale::Context {
	private:
		gale::key::Special m_guiKeyBoardMode;
		int32_t m_originX;
		int32_t m_originY;
		int32_t m_cursorEventX;
		int32_t m_cursorEventY;
		int32_t m_currentHeight;
		int32_t m_currentWidth;
		bool m_doubleBuffered;
		bool m_run;
		//forcing the position
		bool m_grabAllEvent; //!< grab mode enable...
		vec2 m_forcePos; //!< position to reset the cursor
		bool m_positionChangeRequested; //!< the position modifiquation has been requested
		vec2 m_curentGrabDelta; //!< the position in SDL will arrive by pool
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		etk::String m_uniqueWindowsName;
		enum gale::context::cursor m_currentCursor; //!< select the current cursor to display :
		char32_t m_lastKeyPressed; //!< The last element key presed...
		SDL_Window* m_screen;
		//SDL_Surface* m_screen;
		SDL_GLContext m_context;
	public:
		SDLInterface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv),
		  m_originX(0),
		  m_originY(0),
		  m_cursorEventX(0),
		  m_cursorEventY(0),
		  m_currentHeight(450),
		  m_currentWidth(600),
		  m_doubleBuffered(0),
		  m_run(false),
		  m_grabAllEvent(false),
		  m_forcePos(0,0),
		  m_positionChangeRequested(false),
		  m_curentGrabDelta(0,0),
		  m_currentCursor(gale::context::cursor::arrow),
		  m_lastKeyPressed(0),
		  m_screen(nullptr) {
			SDL_INFO("SDL:INIT");
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				m_inputIsPressed[iii] = false;
			}
			if (m_doubleBuffered) {
				//XSync(m_display,0);
			}
			SDL_INFO("SDL:INIT");
			if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
				printf("Unable to initialize SDL: %s\n", SDL_GetError());
				exit(-1);
			}
			SDL_INFO("SDL:INIT");
			
			m_screen = SDL_CreateWindow("sdl_gl_read", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_currentWidth, m_currentHeight, SDL_WINDOW_OPENGL);
			
			SDL_INFO("SDL:INIT");
			if (!m_screen) {
				printf("Unable to create window: %s\n", SDL_GetError());
				exit(-1);
			}
			SDL_INFO("SDL:INIT");
			m_context = SDL_GL_CreateContext(m_screen);
			SDL_INFO("SDL:INIT");
			SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // *new*
/*
			SDL_Init(SDL_INIT_VIDEO);
			m_screen = SDL_SetVideoMode(m_currentWidth, m_currentHeight, 32, SDL_OPENGL/*SDL_HWSURFACE* /);
*/
			SDL_INFO("SDL:INIT");
			OS_Resize(vec2(m_currentWidth, m_currentHeight));
			
			// Configure the DPI of the screen:
			{
				vec2 dpi(0,0);
				dpi.setX(75);
				dpi.setY(75);
				gale::Dimension::setPixelRatio(dpi, gale::distance::inch);
				GALE_INFO("monitor property : dpi=" << dpi << " px/inch");
			}
			m_run = true;
		}
		
		~SDLInterface() {
			SDL_Quit();
		}
		
		int32_t run() {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch(event.type) {
					case SDL_FINGERMOTION:
					case SDL_FINGERDOWN:
					case SDL_FINGERUP: {
						SDL_TouchFingerEvent *t = (SDL_TouchFingerEvent*)&event;
						SDL_INFO("type: " << TouchFingerTypeToString(event.type)
						         << ", timestamp: " << t->timestamp
						         << ", touchId: " << t->touchId
						         << ", fingerId: " << t->fingerId
						         << ", pos=" << vec2(t->x, t->y)
						         << ", delta Pos=" << vec2(t->dx, t->dy)
						         << ", pressure: " << t->pressure);
						if (t->timestamp != 0 && t->x >= 0.f && t->x <= 1.f && t->y >= 0.f && t->y <= 1.f && t->pressure >= 0.f && t->pressure <= 1.f) {
							if (event.type == SDL_FINGERDOWN) {
								got_down = 1; progress();
							}
							if (event.type == SDL_FINGERMOTION) {
								got_move = 1; progress();
							}
							if (event.type == SDL_FINGERDOWN) {
								got_up = 1; progress();
							}
						}
						break;
					}
				}
			}
			/*
			if (SDL_MUSTLOCK(m_screen)) {
				SDL_LockSurface(m_screen);
			}
			*/
			/*
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
			*/
			/*
			if (true) {
				for (int yyy = 0; yyy < m_currentWidth; yyy++) {
					for (int xxx = 0; xxx < m_currentHeight; xxx++) {
						#ifdef TEST_SDL_LOCK_OPTS
							// Alpha behaves like in the browser, so write proper opaque pixels.
							int alpha = 255;
						#else
							// To emulate native behavior with blitting to screen, alpha component is ignored. Test that it is so by outputting
							// data (and testing that it does get discarded)
							int alpha = (yyy+xxx) % 255;
						#endif
						*((Uint32*)m_screen->pixels + yyy * m_currentWidth + xxx) = SDL_MapRGBA(m_screen->format, yyy, xxx, (Uint32)(255.0f * float(yyy)/float(m_currentHeight)), alpha);
					}
				}
			}
			if (SDL_MUSTLOCK(m_screen)) {
				SDL_UnlockSurface(m_screen);
			}
			*/
			SDL_INFO("Draw [START]");
			OS_Draw(true);
			SDL_INFO("Draw [STOP]");
			//SDL_Flip(m_screen);
			
			return 0;
			
			/*
			// main cycle
			while(m_run == true) {
				SDL_Event evt;
				if (    SDL_PollEvent(&evt) != 0
				     && evt.type == SDL_QUIT) {
					m_run = false;
					continue;
				}
				if(m_run == true) {
					if (    m_doubleBuffered == true
					     && hasDisplay == true) {
						// TODO : SWAP ....
					}
					//hasDisplay = OS_Draw(true);
					//SDL_LockSurface(m_screen);
					
					if (SDL_MUSTLOCK(m_screen)) {
						SDL_LockSurface(m_screen);
					}
					for (int i = 0; i < 256; i++) {
						for (int j = 0; j < 256; j++) {
							#ifdef TEST_SDL_LOCK_OPTS
								// Alpha behaves like in the browser, so write proper opaque pixels.
								int alpha = 255;
							#else
								// To emulate native behavior with blitting to screen, alpha component is ignored. Test that it is so by outputting
								// data (and testing that it does get discarded)
								int alpha = (i+j) % 255;
							#endif
							*((Uint32*)m_screen->pixels + i * 256 + j) = SDL_MapRGBA(m_screen->format, i, j, 255-i, alpha);
						}
					}
					if (SDL_MUSTLOCK(m_screen)) {
						SDL_UnlockSurface(m_screen);
					}
					SDL_Flip(m_screen);
					//SDL_UnlockSurface(m_screen);
					SDL_Delay(33);
				}
			}
			*/



			return 0;
		}
		/****************************************************************************************/
		virtual void stop() {
			SDL_INFO("SDL-API: Stop");
			m_run = false;
		}
};

static void loop(void* _arg) {
	SDLInterface* interface = (SDLInterface*)_arg;
	if (interface != nullptr) {
		interface->run();
	}
}


/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	printf("lklklklklk\n");
	#ifndef __EMSCRIPTEN__
		// include GL stuff, to check that we can compile hybrid 2d/GL apps
		extern void glBegin(int mode);
		extern void glBindBuffer(int target, int buffer);
		if (_argc == 9876) {
			glBegin(0);
			glBindBuffer(0, 0);
		}
	#endif
	printf("22222222\n");
	etk::init(_argc, _argv);
	printf("33333333\n");
	SDLInterface* interface = new SDLInterface(_application, _argc, _argv);
	if (interface == nullptr) {
		GALE_CRITICAL("Can not create the SDL interface ... MEMORY allocation error");
		return -2;
	}
	printf("444444\n");
	emscripten_set_main_loop_arg(loop, interface, 0, 0);
	int32_t retValue = interface->run();
	printf("55555\n");
	delete(interface);
	printf("666666\n");
	interface = nullptr;
	return retValue;
}
