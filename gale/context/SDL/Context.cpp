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

#include <GL/gl.h>
#include <GL/glut.h>
#include <emscripten.h>

bool hasDisplay = false;
#define DEBUG_GLUT_EVENT
#ifdef DEBUG_GLUT_EVENT
	#define GLUT_DEBUG      GALE_DEBUG
	#define GLUT_VERBOSE    GALE_VERBOSE
	#define GLUT_INFO       GALE_INFO
	#define GLUT_CRITICAL   GALE_CRITICAL
#else
	#define GLUT_DEBUG      GALE_VERBOSE
	#define GLUT_VERBOSE    GALE_VERBOSE
	#define GLUT_INFO       GALE_VERBOSE
	#define GLUT_CRITICAL   GALE_VERBOSE
#endif

#ifdef GALE_BUILD_EGAMI
	#include <egami/egami.hpp>
#endif


class GLUTInterface : public gale::Context {
	private:
		gale::key::Special m_guiKeyBoardMode;
		int32_t m_originX;
		int32_t m_originY;
		vec2 m_cursorEventPos;
		vec2 m_currentSize;
		bool m_doubleBuffered;
		bool m_run;
		//forcing the position
		bool m_grabAllEvent; //!< grab mode enable...
		vec2 m_forcePos; //!< position to reset the cursor
		bool m_positionChangeRequested; //!< the position modifiquation has been requested
		vec2 m_curentGrabDelta; //!< the position in GLUT will arrive by pool
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		std::string m_uniqueWindowsName;
		enum gale::context::cursor m_currentCursor; //!< select the current cursor to display :
		char32_t m_lastKeyPressed; //!< The last element key presed...
	public:
		GLUTInterface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv),
		  m_originX(0),
		  m_originY(0),
		  m_cursorEventPos(0,0),
		  m_currentSize(640,480),
		  m_doubleBuffered(0),
		  m_run(false),
		  m_grabAllEvent(false),
		  m_forcePos(0,0),
		  m_positionChangeRequested(false),
		  m_curentGrabDelta(0,0),
		  m_currentCursor(gale::context::cursor::arrow),
		  m_lastKeyPressed(0) {
			GLUT_INFO("GLUT:INIT");
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				m_inputIsPressed[iii] = false;
			}
			if (m_doubleBuffered) {
				//XSync(m_display,0);
			}
			OS_Resize(m_currentSize);
			
			// Configure the DPI of the screen:
			{
				vec2 dpi(0,0);
				dpi.setX(75);
				dpi.setY(75);
				gale::Dimension::setPixelRatio(dpi, gale::distance::inch);
				GALE_INFO("monitor property : dpi=" << dpi << " px/inch");
			}
			m_run = true;
			start2ntThreadProcessing();
		}
		
		~GLUTInterface() {
			//GLUT_Quit();
		}
		
		int32_t draw() {
			GLUT_INFO("RUN");
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			GLUT_INFO("RUN 2");
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLUT_INFO("RUN 3");
			//glLoadIdentity();
			GLUT_INFO("Draw [START]");
			OS_Draw(true);
			GLUT_INFO("Draw [STOP]");
			glutSwapBuffers();
			//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			return 0;
		}
		/****************************************************************************************/
		virtual void stop() {
			GLUT_INFO("GLUT-API: Stop");
			m_run = false;
		}
		void reshape(int _width, int _height) {
			glViewport(0, 0, (GLint) _width, (GLint) _height);
			m_currentSize = vec2(_width, _height);
			OS_Resize(vec2(_width, _height));
		}
		void special(int _special) {
			GALE_INFO("Value=" << _special);
			bool find = true;
			enum gale::key::keyboard keyInput = gale::key::keyboard::unknow;
			switch (_special) {
				//case 80: // keypad
				case GLUT_KEY_UP:        keyInput = gale::key::keyboard::up;            break;
				//case 83: // keypad
				case GLUT_KEY_LEFT:      keyInput = gale::key::keyboard::left;          break;
				//case 85: // keypad
				case GLUT_KEY_RIGHT:     keyInput = gale::key::keyboard::right;         break;
				//case 88: // keypad
				case GLUT_KEY_DOWN:      keyInput = gale::key::keyboard::down;          break;
				//case 81: // keypad
				case GLUT_KEY_PAGE_UP:   keyInput = gale::key::keyboard::pageUp;        break;
				//case 89: // keypad
				case GLUT_KEY_PAGE_DOWN: keyInput = gale::key::keyboard::pageDown;      break;
				//case 79: // keypad
				case GLUT_KEY_HOME:      keyInput = gale::key::keyboard::start;         break;
				//case 87: // keypad
				case GLUT_KEY_END:       keyInput = gale::key::keyboard::end;           break;
				// TODO: case 78:	keyInput = gale::key::keyboard::stopDefil;     break;
				// TODO: case 127:	keyInput = gale::key::keyboard::wait;          break;
				//case 90: // keypad
				case GLUT_KEY_INSERT:    keyInput = gale::key::keyboard::insert;        break;
				//case 84:  keyInput = gale::key::keyboardCenter; break; // Keypad
				case GLUT_KEY_F1:    keyInput = gale::key::keyboard::f1; break;
				case GLUT_KEY_F2:    keyInput = gale::key::keyboard::f2; break;
				case GLUT_KEY_F3:    keyInput = gale::key::keyboard::f3; break;
				case GLUT_KEY_F4:    keyInput = gale::key::keyboard::f4; break;
				case GLUT_KEY_F5:    keyInput = gale::key::keyboard::f5; break;
				case GLUT_KEY_F6:    keyInput = gale::key::keyboard::f6; break;
				case GLUT_KEY_F7:    keyInput = gale::key::keyboard::f7; break;
				case GLUT_KEY_F8:    keyInput = gale::key::keyboard::f8; break;
				case GLUT_KEY_F9:    keyInput = gale::key::keyboard::f9; break;
				case GLUT_KEY_F10:   keyInput = gale::key::keyboard::f10; break;
				case GLUT_KEY_F11:   keyInput = gale::key::keyboard::f11; break;
				case GLUT_KEY_F12:   keyInput = gale::key::keyboard::f12; break;
				case 112:    keyInput = gale::key::keyboard::shiftLeft;   break;
				case 114:    keyInput = gale::key::keyboard::ctrlLeft;   break;
				case 116:    keyInput = gale::key::keyboard::alt;   break;
				case 111: // Suppr
					find = false;
					OS_setKeyboard(m_guiKeyBoardMode,
					               gale::key::keyboard::character,
					               gale::key::status::down,
					               false,
					               0x7F);
					OS_setKeyboard(m_guiKeyBoardMode,
					               gale::key::keyboard::character,
					               gale::key::status::up,
					               false,
					               0x7F);
					break;
				case 120: // delete
					find = false;
					OS_setKeyboard(m_guiKeyBoardMode,
					               gale::key::keyboard::character,
					               gale::key::status::down,
					               false,
					               0x08);
					OS_setKeyboard(m_guiKeyBoardMode,
					               gale::key::keyboard::character,
					               gale::key::status::up,
					               false,
					               0x08);
					break;
				default:
					find = false;
					GALE_ERROR("Not find special:" << _special);
			}
			if (find == true) {
				OS_setKeyboard(m_guiKeyBoardMode,
				               keyInput,
				               gale::key::status::down,
				               false);
				OS_setKeyboard(m_guiKeyBoardMode,
				               keyInput,
				               gale::key::status::up,
				               false);
			}
			/*		case GLUT_KEY_F11:
						glutFullScreen();
						break;
			*/
			GALE_ERROR("eventKey Move type : " << keyInput );
		}
		
		void keyboard(unsigned char _key) {
			GALE_ERROR("keyboard: " << int(_key));
			OS_setKeyboard(m_guiKeyBoardMode,
			               gale::key::keyboard::character,
			               gale::key::status::down,
			               false,
			               _key);
			OS_setKeyboard(m_guiKeyBoardMode,
			               gale::key::keyboard::character,
			               gale::key::status::up,
			               false,
			               _key);
		}
		
		void mouse(int _button, bool _up, const vec2& _pos) {
			GALE_ERROR("mouse: " << _button << " " << (_up==true?"up":"down") << " " << _pos);
			m_cursorEventPos = vec2(_pos.x(), m_currentSize.y() - _pos.y());
			OS_SetInput(gale::key::type::mouse,
			            (_up==true?gale::key::status::up:gale::key::status::down),
			            _button+1,
			            m_cursorEventPos);
			m_inputIsPressed[_button+1] = !_up;
		}
		
		void mousePasive(const vec2& _pos) {
			GALE_ERROR("mouse: " << _pos);
			m_cursorEventPos = vec2(_pos.x(), m_currentSize.y() - _pos.y());
			
			// For compatibility of the Android system : 
			bool findOne = false;
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT ; iii++) {
				if (m_inputIsPressed[iii] == true) {
					OS_SetInput(gale::key::type::mouse,
					            gale::key::status::move,
					            iii,
					            m_cursorEventPos);
					findOne = true;
				}
			}
			if (findOne == false) {
				OS_SetInput(gale::key::type::mouse,
				            gale::key::status::move,
				            0,
				            m_cursorEventPos);
			}
		}
		int32_t run() {
			glutMainLoop();
			return 0;
		}
};

GLUTInterface* g_interface = nullptr;

static void gale_glut_idle() {
	glutPostRedisplay();
	//g_interface->idle();
}
static void gale_glut_reshape(int _width, int _height) {
	g_interface->reshape(_width, _height);
}
static void gale_glut_draw() {
	g_interface->draw();
}
static void gale_glut_special(int _special, int _x, int _y) {
	g_interface->special(_special);
}
static void gale_glut_keyboard(unsigned char _key, int _x, int _y) {
	g_interface->keyboard(_key);
}
static void gale_glut_mouse(int _button, int _state, int _x, int _y) {
	g_interface->mouse( _button, bool(_state), vec2(_x, _y));
}
static void gale_glut_mouse_pasive(int _x, int _y) {
	g_interface->mousePasive(vec2(_x, _y));
}




#include <gale/context/SDL/Context.hpp>

bool gale::context::sdl::isBackendPresent() {
	// TODO : Do it better...
	return true;
}

ememory::SharedPtr<gale::Context> gale::context::sdl::createInstance(gale::Application* _application, int _argc, const char *_argv[]) {
	#ifndef __EMSCRIPTEN__
		// include GL stuff, to check that we can compile hybrid 2d/GL apps
		extern void glBegin(int mode);
		extern void glBindBuffer(int target, int buffer);
		if (_argc == 9876) {
			glBegin(0);
			glBindBuffer(0, 0);
		}
	#endif
	ememory::SharedPtr<gale::Context> out; ememory::makeShared<X11Interface>(_application, _argc, _argv);
	
	/* Initialize the window */
	glutInit(&_argc, (char**)_argv);
	glutInitWindowSize(600, 450);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("coucou, name to change ...");
	/* Set up glut callback functions */
	glutIdleFunc(gale_glut_idle);
	glutReshapeFunc(gale_glut_reshape);
	glutDisplayFunc(gale_glut_draw);
	glutSpecialFunc(gale_glut_special);
	//glutEntryFunc(gale_glut_entry);
	glutKeyboardFunc(gale_glut_keyboard);
	glutMouseFunc(gale_glut_mouse);
	glutMotionFunc(gale_glut_mouse_pasive);
	GALE_DEBUG("7987984654654\n");
	
	return out;
}
