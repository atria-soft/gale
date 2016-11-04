/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
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

#include <GL/gl.h>
#include <GL/glut.h>
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
		std::string m_uniqueWindowsName;
		enum gale::context::cursor m_currentCursor; //!< select the current cursor to display :
		char32_t m_lastKeyPressed; //!< The last element key presed...
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
		  m_lastKeyPressed(0) {
			SDL_INFO("SDL:INIT");
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				m_inputIsPressed[iii] = false;
			}
			if (m_doubleBuffered) {
				//XSync(m_display,0);
			}
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
			//SDL_Quit();
		}
		
		int32_t run() {
			SDL_INFO("RUN");
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			SDL_INFO("RUN 2");
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			SDL_INFO("RUN 3");
			//glLoadIdentity();
			SDL_INFO("Draw [START]");
			OS_Draw(true);
			SDL_INFO("Draw [STOP]");
			glutSwapBuffers();
			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			return 0;
		}
		/****************************************************************************************/
		virtual void stop() {
			SDL_INFO("SDL-API: Stop");
			m_run = false;
		}
};

SDLInterface* g_interface = nullptr;

static void gears_idle() {
   glutPostRedisplay();
}

/**
 * Handles a new window size or exposure.
 *
 * @param width the window width
 * @param height the window height
 */
static void gears_reshape(int width, int height) {
   /* Update the projection matrix */
   //perspective(ProjectionMatrix, 60.0, width / (float)height, 1.0, 1024.0);
   /* Set the viewport */
   glViewport(0, 0, (GLint) width, (GLint) height);
}

/**
 * Draws the gears.
 */
static void gears_draw() {
	g_interface->run();
}

/**
 * Handles special glut events.
 *
 * @param special the event to handle.
 */
static void gears_special(int _special, int _crap, int _morecrap) {
	switch (_special) {
		case GLUT_KEY_LEFT:
			GALE_INFO("GLUT_KEY_LEFT");
			break;
		case GLUT_KEY_RIGHT:
			GALE_INFO("GLUT_KEY_RIGHT");
			break;
		case GLUT_KEY_UP:
			GALE_INFO("GLUT_KEY_UP");
			break;
		case GLUT_KEY_DOWN:
			GALE_INFO("GLUT_KEY_DOWN");
			break;
		case GLUT_KEY_F11:
			glutFullScreen();
			break;
	}
}


/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	GALE_DEBUG("lklklklklk\n");
	#ifndef __EMSCRIPTEN__
		// include GL stuff, to check that we can compile hybrid 2d/GL apps
		extern void glBegin(int mode);
		extern void glBindBuffer(int target, int buffer);
		if (_argc == 9876) {
			glBegin(0);
			glBindBuffer(0, 0);
		}
	#endif
	GALE_DEBUG("22222222\n");
	etk::init(_argc, _argv);
	GALE_DEBUG("33333333\n");
	g_interface = new SDLInterface(_application, _argc, _argv);
	if (g_interface == nullptr) {
		GALE_CRITICAL("Can not create the SDL interface ... MEMORY allocation error");
		return -2;
	}
	
	/* Initialize the window */
	glutInit(&_argc, (char**)_argv);
	glutInitWindowSize(600, 450);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("coucou, name to change ...");
	/* Set up glut callback functions */
	glutIdleFunc(gears_idle);
	glutReshapeFunc(gears_reshape);
	glutDisplayFunc(gears_draw);
	glutSpecialFunc(gears_special);
	GALE_DEBUG("7987984654654\n");
	
	glutMainLoop();
	
	GALE_DEBUG("444444\n");
	int32_t retValue = 0; //g_interface->run();
	
	delete(g_interface);
	g_interface = nullptr;
	return retValue;
}
