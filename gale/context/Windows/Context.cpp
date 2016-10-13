/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/Dimension.hpp>
#include <etk/types.hpp>
#include <etk/os/FSNode.hpp>

#include <etk/math/Vector2D.hpp>
#include <etk/math/Vector3D.hpp>

#include <gale/key/key.hpp>
#include <gale/resource/Texture.hpp>
#include <gale/context/Context.hpp>
#include <gale/renderer/openGL/openGL.hpp>
#include <gale/renderer/openGL/openGL-include.hpp>

#include <sys/time.h>
#include <windows.h>
#include <windowsx.h>
#include <etk/etk.hpp>

static std::string GetLastErrorAsString() {
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}
	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	                             NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	std::string message(messageBuffer, size);
	//Free the buffer.
	LocalFree(messageBuffer);
	return message;
}

class WindowsContext;
// This is a bad hook to get the wurrent windows pointer (it is unique ...):
static WindowsContext * galeWindowsContext = nullptr;

// Window Procedure
static LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam);

class WindowsContext : public gale::Context {
	private:
		int32_t m_currentHeight = 0;
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		gale::key::Special m_guiKeyBoardMode;
		bool m_run;
		bool m_clipBoardOwnerStd;
		// windows specific instance ....
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		HDC m_hDC;
		HGLRC m_hRC;
	public:
		WindowsContext(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv),
		  m_run(true),
		  m_clipBoardOwnerStd(false),
		  m_hInstance(GetModuleHandle(nullptr)),
		  m_hWnd(0),
		  m_hDC(0),
		  m_hRC(0) {
			galeWindowsContext = this;
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; ++iii) {
				m_inputIsPressed[iii] = false;
			}
			configure();
		}
		
		~WindowsContext() {
			galeWindowsContext = nullptr;
			release();
		}
		
		void configure() {
			// register a new windows class (need it to define external callback)
			WNDCLASSEX windowClass;
			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_OWNDC;//CS_HREDRAW | CS_VREDRAW;//CS_OWNDC;
			windowClass.lpfnWndProc = WndProc;
			windowClass.cbClsExtra = 0;
			windowClass.cbWndExtra = 0;
			windowClass.hInstance = m_hInstance;
			windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
			windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			windowClass.lpszMenuName = NULL;
			windowClass.lpszClassName = "GaleMainWindows";
			//Register window class
			if (RegisterClassEx(&windowClass) == 0) {
				GALE_ERROR("Can not register windows class: '" << GetLastErrorAsString() << "'" );
				MessageBox(m_hWnd, "Error creating window class\n(gale internal error #65231)", "Error", MB_ICONEXCLAMATION);
				PostQuitMessage(0);
				return;
			}
			// Now we use the created windows class "GaleMainWindows" to create the wew windows with callback ... ==> use "STATIC" to not use generic callback ...
			m_hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,       // The extended window style of the window being created.
			                        "GaleMainWindows",     // A null-terminated string or a class atom created by a previous call to the RegisterClass or RegisterClassEx function.
			                        "Gale ... TODO Title", // The window name.
			                        0
			                        //| WS_OVERLAPPEDWINDOW // 
			                        | WS_CAPTION         // Enable title bar
			                        //| WS_DISABLED      // Disable receive Windows event !! need to active EnableWindow
			                        | WS_POPUPWINDOW     // The window is a pop-up window
			                        //| WS_POPUP           // The windows is a pop-up window
			                        //| WS_VISIBLE         // Start in visible mode
			                        //| WS_CLIPSIBLINGS    // 
			                        //| WS_CLIPCHILDREN    // 
			                        | WS_SIZEBOX         // 
			                        ,                      // The style of the window being created.
			                        10, 10,                // start position
			                        800, 600,              // start size
			                        nullptr,               // A handle to the parent or owner window of the window being created
			                        nullptr,               // A handle to a menu, or specifies a child-window identifier, depending on the window style
			                        m_hInstance,             // A handle to the instance of the module to be associated with the window.
			                        nullptr                //A pointer to a value to be passed to the window through the CREATESTRUCT structure
			                        );
			if(m_hWnd == nullptr) {
				GALE_ERROR("Can not create windows '" << GetLastErrorAsString() << "'" );
				MessageBox(m_hWnd, "Error creating window\n(gale internal error #45211)", "Error", MB_ICONEXCLAMATION);
				PostQuitMessage(0);
			}
			int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME);
			int title_size = GetSystemMetrics(SM_CYCAPTION);
			m_currentHeight = 600-2*border_thickness -title_size;
			OS_Resize(vec2(800-2*border_thickness, m_currentHeight));
			
			// enable openGL for the window
			enableOpenGL(m_hWnd, &m_hDC, &m_hRC);
			GLenum err = glewInit();
			if (GLEW_OK != err) {
				// Problem: glewInit failed, something is seriously wrong.
				GALE_CRITICAL("Error: '" << glewGetErrorString(err) << "'" );
				MessageBox(m_hWnd, "Error initilizing open GL\n(gale internal error #8421)", "Error", MB_ICONEXCLAMATION);
				PostQuitMessage(0);
			}
			if (!glewIsSupported("GL_VERSION_2_0")) {
				GALE_ERROR("OpenGL 2.0 not available '" << glewGetErrorString(err) << "'" );
				MessageBox(m_hWnd, "Error initilizing open GL\n OPENGL 2.0 not availlable ...\n(gale internal error #75124)", "Error", MB_ICONEXCLAMATION);
				PostQuitMessage(0);
			}
			// Configure the DPI of the screen:
			{
				vec2 dpi(0,0);
				dpi.setX(GetDeviceCaps(m_hDC, LOGPIXELSX));
				dpi.setY(GetDeviceCaps(m_hDC, LOGPIXELSY));
				gale::Dimension::setPixelRatio(dpi, gale::distance::inch);
				GALE_INFO("monitor property : dpi=" << dpi << " px/inch");
			}
			// Get monitor Size
			{
				HMONITOR monitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO info;
				info.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(monitor, &info);
				int monitor_width = info.rcMonitor.right - info.rcMonitor.left;
				int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;
				GALE_INFO("monitor property: size=" << ivec2(monitor_width, monitor_height) << " px");
			}
			ShowCursor(TRUE);
		}
		int32_t run() {
			// Now we can show it ...
			ShowWindow(m_hWnd, SW_SHOW);
			//EnableWindow(m_hWnd, TRUE);
			// Force update of the windows
			UpdateWindow(m_hWnd);
			MSG msg;
			// program main loop
			while(m_run == true) {
				// check for messages
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					// handle or dispatch messages
					if (msg.message == WM_QUIT) {
						m_run = false;
					} else {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				} else {
					glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glLoadIdentity();
					OS_Draw(true);
					SwapBuffers(m_hDC);
				}
			}
			return msg.wParam;
		}
		void release() {
			// TODO : We destroy the OpenGL context before cleaning GALE : Do it Better ...
			getResourcesManager().contextHasBeenDestroyed();
			// shutdown openGL
			disableOpenGL(m_hWnd, m_hDC, m_hRC);
			// destroy the window explicitly
			DestroyWindow(m_hWnd);
		}
		
		/****************************************************************************************/
		void stop() {
			m_run = false;
			// To exit program ...
			PostQuitMessage(0);
		}
		/****************************************************************************************/
		void setSize(const vec2& _size) {
			float border_thickness = GetSystemMetrics(SM_CXSIZEFRAME);
			float title_size = GetSystemMetrics(SM_CYCAPTION);
			vec2 newSize(_size.x() + border_thickness*2.0f,
			             _size.y() + border_thickness*2.0f + title_size);
			//m_currentHeight = size.y;
			// TODO : Later
		}
		/****************************************************************************************/
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
					// TODO : never done reset at false ...
					if (m_clipBoardOwnerStd == false) {
						char* buffer = nullptr;
						if(OpenClipboard(m_hWnd)) {
							buffer = (char*)GetClipboardData(CF_TEXT);
							std::string tmpppp((char*)buffer);
							// TODO : Check if we need to free buffer ...
							gale::context::clipBoard::setSystem(gale::context::clipBoard::clipboardSelection, tmpppp);
							// just transmit an event , we have the data in the system
							OS_ClipBoardArrive(gale::context::clipBoard::clipboardStd);
						}
						CloseClipboard();
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
		/****************************************************************************************/
		void ClipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					// NOTE : nothing to do : Windows deas ot supported Middle button
					break;
				case gale::context::clipBoard::clipboardStd:
					// Request the clipBoard :
					if (m_clipBoardOwnerStd == false) {
						std::string tmpData = gale::context::clipBoard::get(_clipboardID);
						//put your text in source
						if(OpenClipboard(m_hWnd)) {
							HGLOBAL clipbuffer;
							char * buffer;
							EmptyClipboard();
							clipbuffer = GlobalAlloc(GMEM_DDESHARE, tmpData.size()+1);
							buffer = (char*)GlobalLock(clipbuffer);
							strcpy(buffer, tmpData.c_str());
							GlobalUnlock(clipbuffer);
							SetClipboardData(CF_TEXT, clipbuffer);
							CloseClipboard();
						}
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
			*_hDC = GetDC(_hWnd);
			// Configure the color set ...
			PIXELFORMATDESCRIPTOR pfd;
			// set the pixel format for the DC
			ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cColorBits = 32;
			pfd.cDepthBits = 16;
			pfd.iLayerType = PFD_MAIN_PLANE;
			int format = ChoosePixelFormat(*_hDC, &pfd);
			SetPixelFormat(*_hDC, format, &pfd);
			// create and enable the render context(RC)
			*_hRC = wglCreateContext(*_hDC);
			wglMakeCurrent(*_hDC, *_hRC);
			if (wglGetCurrentContext() == nullptr) {
				GALE_ERROR("can not create OPEN gL context...");
				MessageBox(_hWnd, "Error initilizing open GL\n openGL context creation error...\n(gale internal error #3526)", "Error", MB_ICONEXCLAMATION);
				PostQuitMessage(0);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT);
		}
		
		// disable openGL (fisnish application ...
		void disableOpenGL(HWND _hWnd, HDC _hDC, HGLRC _hRC) {
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(_hRC);
			ReleaseDC(_hWnd, _hDC);
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
					break;
				case WM_CLOSE:
					GALE_DEBUG("WM_CLOSE");
					OS_Stop();
					return 0;
				case WM_DESTROY:
					GALE_DEBUG("WM_DESTROY");
					m_run = false;
					break;
				case WM_MOVE:
					GALE_DEBUG("WM_MOVE");
					break;
				case WM_SIZE:
					GALE_DEBUG("WM_SIZE");
					break;
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
					break;
				*/
				case WM_WINDOWPOSCHANGING: {
					WINDOWPOS* tmpVal = (WINDOWPOS*)_lParam;
					if (tmpVal == nullptr) {
						break;
					}
					GALE_VERBOSE("WM_WINDOWPOSCHANGING : : (" << tmpVal->x << "," << tmpVal->y << ") ( " << tmpVal->cx << "," << tmpVal->cy << ")");
					if (    tmpVal->cx == 0
					     && tmpVal->cy == 0) {
						// special case for start application and hide application
						break;
					}
					// in windows system, we need to remove the size of the border elements:
					int border_thickness = GetSystemMetrics(SM_CXSIZEFRAME);
					int title_size = GetSystemMetrics(SM_CYCAPTION);
					m_currentHeight = tmpVal->cy - 2*border_thickness - title_size;
					float width = tmpVal->cx-2*border_thickness;
					OS_Resize(vec2(width, m_currentHeight));
					break;
				}
				// these message are not parse by us ...
				case WM_SETCURSOR: // Call the windows if we want the mouse event :
				case WM_NCHITTEST: // inform the application the position of the mouse is moving
					break;
				
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
						case VK_UP:     keyInput = gale::key::keyboard::up; break;
						//case 83: // keypad
						case VK_LEFT:   keyInput = gale::key::keyboard::left; break;
						//case 85: // keypad
						case VK_RIGHT:  keyInput = gale::key::keyboard::right; break;
						//case 88: // keypad
						case VK_DOWN:   keyInput = gale::key::keyboard::down; break;
						//case 81: // keypad
						case VK_PRIOR:  keyInput = gale::key::keyboard::pageUp; break;
						//case 89: // keypad
						case VK_NEXT:   keyInput = gale::key::keyboard::pageDown; break;
						//case 79: // keypad
						case VK_HOME:   keyInput = gale::key::keyboard::start; break;
						//case 87: // keypad
						case VK_END:    keyInput = gale::key::keyboard::end; break;
						//case VK_:     keyInput = gale::key::keyboardStopDefil; break;
						case VK_PAUSE:  keyInput = gale::key::keyboard::wait; break;
						//case 90: // keypad
						case VK_INSERT:
							keyInput = gale::key::keyboard::insert;
							m_guiKeyBoardMode.setInsert(buttonIsDown);
							break;
						case VK_F1: keyInput = gale::key::keyboard::f1; break;
						case VK_F2: keyInput = gale::key::keyboard::f2; break;
						case VK_F3: keyInput = gale::key::keyboard::f3; break;
						case VK_F4: keyInput = gale::key::keyboard::f4; break;
						case VK_F5: keyInput = gale::key::keyboard::f5; break;
						case VK_F6: keyInput = gale::key::keyboard::f6; break;
						case VK_F7: keyInput = gale::key::keyboard::f7; break;
						case VK_F8: keyInput = gale::key::keyboard::f8; break;
						case VK_F9: keyInput = gale::key::keyboard::f9; break;
						case VK_F10: keyInput = gale::key::keyboard::f10; break;
						case VK_F11: keyInput = gale::key::keyboard::f11; break;
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
						case VK_F24: keyInput = gale::key::keyboard::f12; break;
						case VK_CAPITAL:
							keyInput = gale::key::keyboard::capLock;
							m_guiKeyBoardMode.setCapsLock(buttonIsDown);
							break;
						case VK_SHIFT:
						case VK_LSHIFT:
							keyInput = gale::key::keyboard::shiftLeft;
							m_guiKeyBoardMode.setShift(buttonIsDown);
							break;
						case VK_RSHIFT:
							keyInput = gale::key::keyboard::shiftRight;
							m_guiKeyBoardMode.setShift(buttonIsDown);
							break;
						case VK_CONTROL:
						case VK_LCONTROL:
							keyInput = gale::key::keyboard::ctrlLeft;
							m_guiKeyBoardMode.setCtrl(buttonIsDown);
							break;
						case VK_RCONTROL:
							keyInput = gale::key::keyboard::ctrlRight;
							m_guiKeyBoardMode.setCtrl(buttonIsDown);
							break;
						case VK_LWIN:
							keyInput = gale::key::keyboard::metaLeft;
							m_guiKeyBoardMode.setMeta(buttonIsDown);
							break;
						case VK_RWIN:
							keyInput = gale::key::keyboard::metaRight;
							m_guiKeyBoardMode.setMeta(buttonIsDown);
							break;
						case VK_MENU:
						case VK_LMENU:
							keyInput = gale::key::keyboard::alt;
							m_guiKeyBoardMode.setAlt(buttonIsDown);
							break;
						case VK_RMENU:
							keyInput = gale::key::keyboard::altGr;
							m_guiKeyBoardMode.setAltGr(buttonIsDown);
							break;
						//case :   keyInput = gale::key::keyboardContextMenu; break;
						case VK_NUMLOCK:
							keyInput = gale::key::keyboard::numLock;
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
					GALE_VERBOSE("Keyboard Value = " << _wParam << " '" << char(tmpChar) << "'");
					if (tmpChar == 0) {
						//GALE_DEBUG("eventKey Move type : " << getCharTypeMoveEvent(keyInput) );
						OS_setKeyboard(m_guiKeyBoardMode,
						               keyInput,
						               (buttonIsDown==true?gale::key::status::down:gale::key::status::up));
					} else {
						OS_setKeyboard(m_guiKeyBoardMode,
						               gale::key::keyboard::character,
						               (buttonIsDown==true?gale::key::status::down:gale::key::status::up),
						               false,
						               tmpChar);
					}
					break;
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
					OS_SetInput(gale::key::type::mouse,
					            (buttonIsDown==true?gale::key::status::down:gale::key::status::up),
					            mouseButtonId,
					             vec2(pos.x(),pos.y()));
					break;
				
				case WM_MBUTTONUP:
					buttonIsDown = false;
				case WM_MBUTTONDOWN:
					mouseButtonId = 2;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					m_inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetInput(gale::key::type::mouse,
					            (buttonIsDown==true?gale::key::status::down:gale::key::status::up),
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					break;
				
				case WM_RBUTTONUP:
					buttonIsDown = false;
				case WM_RBUTTONDOWN:
					mouseButtonId = 3;
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					m_inputIsPressed[mouseButtonId] = buttonIsDown;
					OS_SetInput(gale::key::type::mouse,
					            (buttonIsDown==true?gale::key::status::down:gale::key::status::up),
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					break;
				
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
					OS_SetInput(gale::key::type::mouse,
					            gale::key::status::down,
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					OS_SetInput(gale::key::type::mouse,
					            gale::key::status::up,
					            mouseButtonId,
					            vec2(pos.x(),pos.y()));
					break;
				
				case WM_MOUSEHOVER:
				case WM_MOUSEMOVE:
					pos.setValue(GET_X_LPARAM(_lParam),
					             m_currentHeight-GET_Y_LPARAM(_lParam));
					for (int32_t iii=0; iii<MAX_MANAGE_INPUT ; iii++) {
						if (true == m_inputIsPressed[iii]) {
							GALE_VERBOSE("Windows event: bt=" << iii << " " << _message << " = \"WM_MOUSEMOVE\" " << pos );
							OS_SetInput(gale::key::type::mouse,
							            gale::key::status::move,
							            iii,
							            vec2(pos.x(),pos.y()));
							return 0;
						}
					}
					GALE_VERBOSE("Windows event: bt=" << 0 << " " << _message << " = \"WM_MOUSEMOVE\" " << pos );
					OS_SetInput(gale::key::type::mouse,
					            gale::key::status::move,
					            0,
					            vec2(pos.x(),pos.y()));
					break;
				
				default:
					GALE_DEBUG("event ..." << _message );
					break;
			}
			return DefWindowProc(_hWnd, _message, _wParam, _lParam);
		}
		/****************************************************************************************/
		void setTitle(const std::string& _title) {
			GALE_VERBOSE("Windows: set Title (START)");
			SetWindowText(m_hWnd, _title.c_str());
			GALE_VERBOSE("Windows: set Title (END)");
		}
};

static LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam) {
	// TODO : set this function really work...
	if (galeWindowsContext == nullptr) {
		return 0;
	}
	return galeWindowsContext->WndProcReal(_hWnd, _message, _wParam, _lParam);
}

/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	WindowsContext* localInterface = new WindowsContext(_application, _argc, _argv);
	if (localInterface == nullptr) {
		GALE_CRITICAL("Can not create the 'Windows' interface ... MEMORY allocation error");
		return -2;
	}
	
	int32_t retValue = localInterface->run();
	delete(localInterface);
	localInterface = nullptr;
	GALE_INFO("END APPLICATION");
	return retValue;
}

