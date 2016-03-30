/** @file
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#include <gale/debug.h>
#include <gale/gale.h>
#include <etk/types.h>
#include <etk/os/FSNode.h>

#include <etk/math/Vector2D.h>
#include <etk/math/Vector3D.h>

#include <gale/key/key.h>
#include <gale/resource/Texture.h>
#include <gale/context/Context.h>
#include <gale/renderer/openGL/openGL.h>
#include <gale/renderer/openGL/openGL-include.h>

#include <sys/time.h>
#include <windows.h>
#include <windowsx.h>
#include <etk/etk.h>



int64_t gale::getTime() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    //GALE_VERBOSE("current time : " << now.tv_sec << "s " << now.tv_usec << "us");
    return (int64_t)((int64_t)now.tv_sec*(int64_t)1000000 + (int64_t)now.tv_usec);
}


#undef __class__
#define __class__ "ContextWindows"


class WindowsContext : public gale::Context {
	private:
		int32_t m_currentHeight = 0;
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		gale::key::Special m_guiKeyBoardMode;
		bool m_run = true;
		bool m_clipBoardOwnerStd = false;
	public:
		WindowsContext(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv) {
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; ++iii) {
				m_inputIsPressed[iii] = false;
			}
		}
		
		~WindowsContext() {
			
		}
		
		int32_t run() {
			HINSTANCE hInstance = 0;
			WNDCLASS wc;
			HWND hWnd;
			HDC hDC;
			HGLRC hRC;
			MSG msg;
			
			// register window class
			wc.style = CS_OWNDC;
			wc.lpfnWndProc = WndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInstance;
			wc.hIcon = LoadIcon( nullptr, IDI_APPLICATION );
			wc.hCursor = LoadCursor( nullptr, IDC_ARROW );
			wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
			wc.lpszMenuName = nullptr;
			wc.lpszClassName = "GaleMainWindows";
			RegisterClass( &wc );
			
			// create main window
			hWnd = CreateWindow( "GaleMainWindows", "Gale ... TODO Title", 
			                     WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE | WS_SIZEBOX,
			                     0, 0, 800, 600,
			                     nullptr, nullptr, hInstance, nullptr);
			int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME);
			int title_size = GetSystemMetrics(SM_CYCAPTION);
			m_currentHeight = 600-2*border_thickness -title_size;
			OS_Resize(vec2(800-2*border_thickness, m_currentHeight));
			
			// enable openGL for the window
			enableOpenGL( hWnd, &hDC, &hRC );
			
			// program main loop
			while(true == m_run) {
				// check for messages
				if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE )) {
					// handle or dispatch messages
					if ( msg.message == WM_QUIT ) {
						m_run = false;
					} else {
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
				} else {
					OS_Draw(true);
					SwapBuffers( hDC );
				}
			}
			// shutdown openGL
			disableOpenGL( hWnd, hDC, hRC );
			// destroy the window explicitly
			DestroyWindow( hWnd );
			return msg.wParam;
		}
		
		void Stop() {
			m_run = false;
			// To exit program ...
			PostQuitMessage(0);
		}
		
		void setSize(const vec2& _size) {
			float border_thickness = GetSystemMetrics(SM_CXSIZEFRAME);
			float title_size = GetSystemMetrics(SM_CYCAPTION);
			vec2 newSize(_size.x() + border_thickness*2.0f,
			             _size.y() + border_thickness*2.0f + title_size);
			//m_currentHeight = size.y;
			// TODO : Later
		}
		
		void ClipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			// this is to force the local system to think we have the buffer
			// TODO : remove this 2 line when code will be writen
			m_clipBoardOwnerStd = true;
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					// NOTE : Windows does not support the middle button the we do it internaly
					// just transmit an event , we have the data in the system
					OS_ClipBoardArrive(gale::context::clipBoard::clipboardSelection);
					break;
				case gale::context::clipBoard::clipboardStd:
					if (false == m_clipBoardOwnerStd) {
						// generate a request TO the OS
						// TODO : Send the message to the OS "We disire to receive the copy buffer ...
					} else {
						// just transmit an event , we have the data in the system
						OS_ClipBoardArrive(gale::context::clipBoard::clipboardStd);
					}
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
		
		void ClipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					// NOTE : nothing to do : Windows deas ot supported Middle button
					break;
				case gale::context::clipBoard::clipboardStd:
					// Request the clipBoard :
					if (false == m_clipBoardOwnerStd) {
						// TODO : Inform the OS that we have the current buffer of copy ...
						m_clipBoardOwnerStd = true;
					}
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
		
		
		
		// enable openGL
		void enableOpenGL(HWND _hWnd, HDC* _hDC, HGLRC* _hRC) {
			// get the device context (DC)
			*_hDC = GetDC( _hWnd );
			
			PIXELFORMATDESCRIPTOR pfd;
			// set the pixel format for the DC
			ZeroMemory(&pfd, sizeof( pfd ) );
			pfd.nSize = sizeof( pfd );
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 24;
			pfd.cDepthBits = 16;
			pfd.iLayerType = PFD_MAIN_PLANE;
			int format = ChoosePixelFormat( *_hDC, &pfd );
			SetPixelFormat( *_hDC, format, &pfd );
			
			// create and enable the render context(RC)
			*_hRC = wglCreateContext( *_hDC );
			wglMakeCurrent( *_hDC, *_hRC );
			
		}
		
		// disable openGL
		
		void disableOpenGL(HWND _hWnd, HDC _hDC, HGLRC _hRC) {
			wglMakeCurrent( nullptr, NULL );
			wglDeleteContext( _hRC );
			ReleaseDC( _hWnd, _hDC );
		}
		
		
		// Window Procedure
		static LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
			// TODO : set this function really work...
			// TODO : return classPointer->WndProcReal(_hWnd, _message, _wParam, _lParam);
			return 0;
		}
		
		LRESULT CALLBACK WndProcReal(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
			bool buttonIsDown = true;
			int32_t mouseButtonId = 0;
			ivec2 pos;
			// to know all _message : http://wiki.winehq.org/List_Of_Windows__messages
			switch (_message) {
				/* **************************************************************************
				 *                             Gui event
				 * **************************************************************************/
				case WM_CREATE:
					GALE_DEBUG("WM_CREATE");
					return 0;
				case WM_CLOSE:
					GALE_DEBUG("WM_CLOSE");
					PostQuitMessage( 0 );
					return 0;
				case WM_DESTROY:
					GALE_DEBUG("WM_DESTROY");
					return 0;
				case WM_MOVE:
					GALE_DEBUG("WM_MOVE");
					return 0;
				case WM_SIZE:
					GALE_DEBUG("WM_SIZE");
					return 0;
				/*
				case WM_GETMINMAXINFO:
					{
						MINMAXINFO* tmpVal = (MINMAXINFO*)lParam;
						GALE_DEBUG("WM_GETMINMAXINFO : ");
						GALE_DEBUG("    ptMaxSize : " << tmpVal->ptMaxSize.x << "," << tmpVal->ptMaxSize.y << ")");
						GALE_DEBUG("    ptMaxPosition : " << tmpVal->ptMaxPosition.x << "," << tmpVal->ptMaxPosition.y << ")");
						GALE_DEBUG("    ptMinTrackSize : " << tmpVal->ptMinTrackSize.x << "," << tmpVal->ptMinTrackSize.y << ")");
						GALE_DEBUG("    ptMaxTrackSize : " << tmpVal->ptMaxTrackSize.x << "," << tmpVal->ptMaxTrackSize.y << ")");
					}
					return 0;
				*/
				case WM_WINDOWPOSCHANGING: {
					WINDOWPOS* tmpVal = (WINDOWPOS*)_lParam;
					if (nullptr != tmpVal) {
						//GALE_DEBUG("WM_WINDOWPOSCHANGING : : (" << tmpVal->x << "," << tmpVal->y << ") ( " << tmpVal->cx << "," << tmpVal->cy << ")");
						// in windows system, we need to remove the size of the border elements : 
						int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME);
						int title_size = GetSystemMetrics(SM_CYCAPTION);
						m_currentHeight = tmpVal->cy - 2*border_thickness - title_size;
						OS_Resize(vec2(tmpVal->cx-2*border_thickness, m_currentHeight));
					}
					return 0;
				}
				// these message are not parse by us ...
				case WM_SETCURSOR: // Call the windows if we want the mouse event :
				case WM_NCHITTEST: // inform the application the position of the mouse is moving
					return DefWindowProc( _hWnd, _message, _wParam, _lParam );
				
				/* **************************************************************************
				 *                             Keyboard management
				 * **************************************************************************/
				case WM_KEYUP:
					buttonIsDown = false;
				case WM_KEYDOWN: {
					char32_t tmpChar = 0;
					enum gale::key::keyboard keyInput;
					switch (_wParam) {
						//case 80: // keypad
						case VK_UP:     keyInput = gale::key::keyboard_up; break;
						//case 83: // keypad
						case VK_LEFT:   keyInput = gale::key::keyboard_left; break;
						//case 85: // keypad
						case VK_RIGHT:  keyInput = gale::key::keyboard_right; break;
						//case 88: // keypad
						case VK_DOWN:   keyInput = gale::key::keyboard_down; break;
						//case 81: // keypad
						case VK_PRIOR:  keyInput = gale::key::keyboard_pageUp; break;
						//case 89: // keypad
						case VK_NEXT:   keyInput = gale::key::keyboard_pageDown; break;
						//case 79: // keypad
						case VK_HOME:   keyInput = gale::key::keyboard_start; break;
						//case 87: // keypad
						case VK_END:    keyInput = gale::key::keyboard_end; break;
						//case VK_:     keyInput = gale::key::keyboardStopDefil; break;
						case VK_PAUSE:  keyInput = gale::key::keyboard_wait; break;
						//case 90: // keypad
						case VK_INSERT:
							keyInput = gale::key::keyboard_insert;
							m_guiKeyBoardMode.setInsert(buttonIsDown);
							break;
						case VK_F1: keyInput = gale::key::keyboard_f1; break;
						case VK_F2: keyInput = gale::key::keyboard_f2; break;
						case VK_F3: keyInput = gale::key::keyboard_f3; break;
						case VK_F4: keyInput = gale::key::keyboard_f4; break;
						case VK_F5: keyInput = gale::key::keyboard_f5; break;
						case VK_F6: keyInput = gale::key::keyboard_f6; break;
						case VK_F7: keyInput = gale::key::keyboard_f7; break;
						case VK_F8: keyInput = gale::key::keyboard_f8; break;
						case VK_F9: keyInput = gale::key::keyboard_f9; break;
						case VK_F10: keyInput = gale::key::keyboard_f10; break;
						case VK_F11: keyInput = gale::key::keyboard_f11; break;
						case VK_F12:
						case VK_F13:
						case VK_F14:
						case VK_F15:
						case VK_F16:
						case VK_F17:
						case VK_F18:
						case VK_F19:
						case VK_F20:
						case VK_F21:
						case VK_F22:
						case VK_F23:
						case VK_F24: keyInput = gale::key::keyboard_f12; break;
						case VK_CAPITAL:
							keyInput = gale::key::keyboard_capLock;
							m_guiKeyBoardMode.setCapsLock(buttonIsDown);
							break;
						case VK_SHIFT:
						case VK_LSHIFT:
							keyInput = gale::key::keyboard_shiftLeft;
							m_guiKeyBoardMode.setShift(buttonIsDown);
							break;
						case VK_RSHIFT:
							keyInput = gale::key::keyboard_shiftRight;
							m_guiKeyBoardMode.setShift(buttonIsDown);
							break;
						case VK_CONTROL:
						case VK_LCONTROL:
							keyInput = gale::key::keyboard_ctrlLeft;
							m_guiKeyBoardMode.setCtrl(buttonIsDown);
							break;
						case VK_RCONTROL:
							keyInput = gale::key::keyboard_ctrlRight;
							m_guiKeyBoardMode.setCtrl(buttonIsDown);
							break;
						case VK_LWIN:
							keyInput = gale::key::keyboard_metaLeft;
							m_guiKeyBoardMode.setMeta(buttonIsDown);
							break;
						case VK_RWIN:
							keyInput = gale::key::keyboard_metaRight;
							m_guiKeyBoardMode.setMeta(buttonIsDown);
							break;
						case VK_MENU:
						case VK_LMENU:
							keyInput = gale::key::keyboard_alt;
							m_guiKeyBoardMode.setAlt(buttonIsDown);
							break;
						case VK_RMENU:
							keyInput = gale::key::keyboard_altGr;
							m_guiKeyBoardMode.setAltGr(buttonIsDown);
							break;
						//case :   keyInput = gale::key::keyboardContextMenu; break;
						case VK_NUMLOCK:
							keyInput = gale::key::keyboard_numLock;
							m_guiKeyBoardMode.setNumLock(buttonIsDown);
							break;
						case VK_BACK: // DEL
							tmpChar = 0x08;
							break;
						// TODO : Really strang, need to understand why ...
						case 46: // Suppr
							tmpChar = 0x7F;
							break;
						case VK_TAB: // special case for TAB
							tmpChar = 0x09;
							break;
						case VK_RETURN: // special case for TAB
							tmpChar = '\n';
							break;
						default:
							{
								BYTE kbd[256];
								GetKeyboardState(kbd);
								const int BUFFER_LENGTH = 8; //Length of the buffer
								WCHAR chars[BUFFER_LENGTH];
								
								ToUnicode(_wParam, _lParam, kbd, chars,BUFFER_LENGTH,0);
								tmpChar = utf8::convertChar32((char*)chars);
							}
							break;
					}
					GALE_DEBUG("kjhkjhkjhkjhkj = " << _wParam);
					if (tmpChar == 0) {
						//GALE_DEBUG("eventKey Move type : " << getCharTypeMoveEvent(keyInput) );
						OS_setKeyboard(m_guiKeyBoardMode,
						               keyInput,
						               (buttonIsDown==true?gale::key::status_down:gale::key::status_up));
					} else {
						OS_setKeyboard(m_guiKeyBoardMode,
						               gale::key::keyboard_char,
						               (buttonIsDown==true?gale::key::status_down:gale::key::status_up),
						               false,
						               tmpChar);
					}
					return 0;
				}
				/* **************************************************************************
				 *                             Mouse management
				 * **************************************************************************/
				case WM_LBUTTONUP:
					buttonIsDown = false;
				case WM_LBUTTONDOWN:
					mouseButtonId = 1;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					m_inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetInput(gale::key::type_mouse,
					            (buttonIsDown==true?gale::key::status_down:gale::key::status_up),
					            mouseButtonId,
					             vec2(pos.x(),pos.y()));
					return 0;
				
				case WM_MBUTTONUP:
					buttonIsDown = false;
				case WM_MBUTTONDOWN:
					mouseButtonId = 2;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					m_inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetInput(gale::key::type_mouse,
					            (buttonIsDown==true?gale::key::status_down:gale::key::status_up),
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					return 0;
				
				case WM_RBUTTONUP:
					buttonIsDown = false;
				case WM_RBUTTONDOWN:
					mouseButtonId = 3;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					m_inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetInput(gale::key::type_mouse,
					            (buttonIsDown==true?gale::key::status_down:gale::key::status_up),
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					return 0;
				
				case WM_MOUSEWHEEL:
					if (_wParam & 0x200000) {
						GALE_DEBUG("event SCROOL UP");
						mouseButtonId = 4;
					} else {
						GALE_DEBUG("event SCROOL DOWN");
						mouseButtonId = 5;
					}
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					OS_SetInput(gale::key::type_mouse,
					            gale::key::status_down,
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					OS_SetInput(gale::key::type_mouse,
					            gale::key::status_up,
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					return 0;
				
				case WM_MOUSEHOVER:
				case WM_MOUSEMOVE:
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					for (int32_t iii=0; iii<MAX_MANAGE_INPUT ; iii++) {
						if (true == m_inputIsPressed[iii]) {
							GALE_VERBOSE("Windows event: bt=" << iii << " " << _message << " = \"WM_MOUSEMOVE\" " << pos );
							OS_SetInput(gale::key::type_mouse,
							            gale::key::status_move,
							            iii,
							            vec2(pos.x(),pos.y()));
							return 0;
						}
					}
					GALE_VERBOSE("Windows event: bt=" << 0 << " " << _message << " = \"WM_MOUSEMOVE\" " << pos );
					OS_SetInput(gale::key::type_mouse,
					            gale::key::status_move,
					            0,
					            vec2(pos.x(),pos.y()));
					return 0;
				
				default:
					GALE_DEBUG("event ..." << _message );
					return DefWindowProc( _hWnd, _message, _wParam, _lParam );
			}
		}
};

/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		// Problem: glewInit failed, something is seriously wrong.
		GALE_ERROR("Error:" << glewGetErrorString(err));
	}
	if (!glewIsSupported("GL_VERSION_2_0")) {
		GALE_ERROR("OpenGL 2.0 not available");
		//return 1;
	}
	WindowsContext* localInterface = new WindowsContext(_application, _argc, _argv);
	if (localInterface == nullptr) {
		GALE_CRITICAL("Can not create the 'Windows' interface ... MEMORY allocation error");
		return -2;
	}
	
	int32_t retValue = localInterface->run();
	delete(localInterface);
	localInterface = nullptr;
	return retValue;
}

