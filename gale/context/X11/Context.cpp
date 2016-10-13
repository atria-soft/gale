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


#if defined(__TARGET_OS__Linux)
#include <GL/glx.h>
#elif defined(__TARGET_OS__MacOs)
#include <OpenGL/glx.h>
#endif

#include <X11/Xatom.h>
#include <sys/times.h>

/*
#define GUI_LOCK()          XLockDisplay(m_display)
#define GUI_UNLOCK()        XUnlockDisplay(m_display)
*/
bool hasDisplay = false;
//#define DEBUG_X11_EVENT
#ifdef DEBUG_X11_EVENT
	#define X11_DEBUG      GALE_DEBUG
	#define X11_VERBOSE    GALE_VERBOSE
	#define X11_INFO       GALE_INFO
	#define X11_CRITICAL   GALE_CRITICAL
#else
	#define X11_DEBUG      GALE_VERBOSE
	#define X11_VERBOSE    GALE_VERBOSE
	#define X11_INFO       GALE_VERBOSE
	#define X11_CRITICAL   GALE_VERBOSE
#endif

int64_t gale::getTime() {
	struct timespec now;
	int ret = clock_gettime(CLOCK_REALTIME, &now);
	if (ret != 0) {
		// Error to get the time ...
		now.tv_sec = time(nullptr);
		now.tv_nsec = 0;
	}
	//GALE_VERBOSE("current time : " << now.tv_sec << "s " << now.tv_usec << "us");
	return (int64_t)((int64_t)now.tv_sec*(int64_t)1000000 + (int64_t)now.tv_nsec/(int64_t)1000);
}

// attributes for a single buffered visual in RGBA format with at least 4 bits per color and a 16 bit depth buffer
static int attrListSgl[] = {
	GLX_RGBA,
	GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
	None
};

// attributes for a double buffered visual in RGBA format with at least 4 bits per color and a 16 bit depth buffer
static int attrListDbl[] = {
	GLX_RGBA,
	GLX_DOUBLEBUFFER,
	GLX_RED_SIZE, 4,
	GLX_GREEN_SIZE, 4,
	GLX_BLUE_SIZE, 4,
	GLX_DEPTH_SIZE, 16,
	None
};

extern "C" {
	typedef struct Hints {
		unsigned long   flags;
		unsigned long   functions;
		unsigned long   decorations;
		long            inputMode;
		unsigned long   status;
	} Hints;
}

#ifdef GALE_BUILD_EGAMI
	#include <egami/egami.hpp>
#endif

#include <X11/cursorfont.h>

class X11Interface : public gale::Context {
	private:
		gale::key::Special m_guiKeyBoardMode;
		// for double and triple click selection, we need to save the previous click up and down position , and the previous time ...
		Atom m_delAtom;
		Display* m_display;
		Window m_WindowHandle;
		XIM m_xim;
		XIC m_xic;
		int32_t m_originX;
		int32_t m_originY;
		int32_t m_cursorEventX;
		int32_t m_cursorEventY;
		int32_t m_currentHeight;
		int32_t m_currentWidth;
		XVisualInfo* m_visual;
		bool m_doubleBuffered;
		bool m_run;
		//forcing the position
		bool m_grabAllEvent; //!< grab mode enable...
		vec2 m_forcePos; //!< position to reset the cursor
		bool m_positionChangeRequested; //!< the position modifiquation has been requested
		vec2 m_curentGrabDelta; //!< the position in X11 will arrive by pool
		bool m_inputIsPressed[MAX_MANAGE_INPUT];
		// internal copy of the clipBoard ...
		bool m_clipBoardRequestPrimary; //!< if false : request the copy/past buffer, if true : request current selection
		bool m_clipBoardOwnerPrimary; //!< we are the owner of the current selection
		bool m_clipBoardOwnerStd; //!< we are the owner of the current copy buffer
		// Atom access...
		Atom XAtomSelection;
		Atom XAtomClipBoard;
		Atom XAtomTargetString;
		Atom XAtomTargetStringUTF8;
		Atom XAtomTargetTarget;
		Atom XAtomGALE;
		Atom XAtomDeleteWindows;
		std::string m_uniqueWindowsName;
		enum gale::context::cursor m_currentCursor; //!< select the current cursor to display :
		char32_t m_lastKeyPressed; //!< The last element key presed...
	public:
		X11Interface(gale::Application* _application, int32_t _argc, const char* _argv[]) :
		  gale::Context(_application, _argc, _argv),
		  m_display(nullptr),
		  m_originX(0),
		  m_originY(0),
		  m_cursorEventX(0),
		  m_cursorEventY(0),
		  m_currentHeight(0),
		  m_currentWidth(0),
		  m_visual(nullptr),
		  m_doubleBuffered(0),
		  m_run(false),
		  m_grabAllEvent(false),
		  m_forcePos(0,0),
		  m_positionChangeRequested(false),
		  m_curentGrabDelta(0,0),
		  m_clipBoardRequestPrimary(false),
		  m_clipBoardOwnerPrimary(false),
		  m_clipBoardOwnerStd(false),
		  XAtomSelection(0),
		  XAtomClipBoard(0),
		  XAtomTargetString(0),
		  XAtomTargetStringUTF8(0),
		  XAtomTargetTarget(0),
		  XAtomGALE(0),
		  XAtomDeleteWindows(0),
		  m_currentCursor(gale::context::cursor::arrow),
		  m_lastKeyPressed(0) {
			X11_INFO("X11:INIT");
			for (int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
				m_inputIsPressed[iii] = false;
			}
			if (m_doubleBuffered) {
				glXSwapBuffers(m_display, m_WindowHandle);
				XSync(m_display,0);
			}
			createX11Context();
			createOGlContext();
			// reset the Atom properties ...
			XAtomSelection        = XInternAtom(m_display, "PRIMARY", 0);
			XAtomClipBoard        = XInternAtom(m_display, "CLIPBOARD", 0);
			XAtomTargetString     = XInternAtom(m_display, "STRING", 0);
			XAtomTargetStringUTF8 = XInternAtom(m_display, "UTF8_STRING", 0);
			XAtomTargetTarget     = XInternAtom(m_display, "TARGETS", 0);
			m_uniqueWindowsName   = "GALE_" + etk::to_string(etk::tool::irand(0, 1999999999));
			XAtomGALE             = XInternAtom(m_display, m_uniqueWindowsName.c_str(), 0);
			XAtomDeleteWindows    = XInternAtom(m_display, "WM_DELETE_WINDOW", 0);
			m_run = true;
		}
		
		~X11Interface() {
			// TODO : ...
		}
		
		int32_t run() {
			bool specialEventThatNeedARedraw = false;
			// main cycle
			while(m_run == true) {
				//GALE_ERROR("plop1");
				XEvent event;
				XEvent respond;
				// main X boucle :
				while (XPending(m_display)) {
					XNextEvent(m_display, &event);
					switch (event.type) {
						case ClientMessage: {
							X11_INFO("Receive : ClientMessage");
							if(XAtomDeleteWindows == (uint64_t)event.xclient.data.l[0]) {
								GALE_INFO("     == > Kill Requested ...");
								OS_Stop();
								// We do not close here but in the application only:
								//m_run = false;
							}
							break;
						}
						///////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//                               Selection AREA                                                              //
						///////////////////////////////////////////////////////////////////////////////////////////////////////////////
						case SelectionClear: {
							// Selection has been done on an other program  == > clear ours ...
							X11_INFO("X11 event SelectionClear");
							#ifdef DEBUG_X11_EVENT
							{
								XSelectionRequestEvent *req=&(event.xselectionrequest);
								if (req->property == 0) {
									GALE_ERROR("Get nullptr ATOM ... property");
									break;
								}
								if (req->target == 0) {
									GALE_ERROR("Get nullptr ATOM ... target");
									break;
								}
								char * atomNameProperty = XGetAtomName(m_display, req->property);
								char * atomNameTarget = XGetAtomName(m_display, req->target);
								GALE_INFO("X11    property: \"" << atomNameProperty << "\"");
								GALE_INFO("X11    target:   \"" << atomNameTarget << "\"");
								if (atomNameProperty != nullptr) {
									XFree(atomNameProperty);
								}
								if (atomNameTarget != nullptr) {
									XFree(atomNameTarget);
								}
							}
							#endif
							if (m_clipBoardOwnerPrimary == true) {
								m_clipBoardOwnerPrimary = false;
							} else if (m_clipBoardOwnerStd == true) {
								m_clipBoardOwnerStd = false;
							} else {
								GALE_ERROR("X11 event SelectionClear  == > but no selection requested anymore ...");
							}
							break;
						}
						case SelectionNotify:
							X11_INFO("X11 event SelectionNotify");
							if (event.xselection.property == None) {
								GALE_VERBOSE("     == > no data ...");
							} else {
								unsigned char *buf = 0;
								Atom type;
								int format;
								unsigned long nitems, bytes;
								XGetWindowProperty(m_display,
								                   m_WindowHandle,
								                   event.xselection.property,
								                   0, // offset
								                   (~0L), // length
								                   False, // delete
								                   AnyPropertyType, // reg_type
								                   &type,// *actual_type::return,
								                   &format,// *actual_format_return
								                   &nitems,// *nitems_return
								                   &bytes, // *bytes_after_return
								                   &buf// **prop_return);
								                   );
								if (m_clipBoardRequestPrimary == true) {
									std::string tmpppp((char*)buf);
									gale::context::clipBoard::setSystem(gale::context::clipBoard::clipboardSelection, tmpppp);
									// just transmit an event , we have the data in the system
									OS_ClipBoardArrive(gale::context::clipBoard::clipboardSelection);
								} else {
									std::string tmpppp((char*)buf);
									gale::context::clipBoard::setSystem(gale::context::clipBoard::clipboardStd, tmpppp);
									// just transmit an event , we have the data in the system
									OS_ClipBoardArrive(gale::context::clipBoard::clipboardStd);
								}
							}
							break;
						case SelectionRequest: {
							X11_INFO("X11 event SelectionRequest");
							XSelectionRequestEvent *req=&(event.xselectionrequest);
							#ifdef DEBUG_X11_EVENT
							{
								if (req->property == 0) {
									GALE_ERROR("Get nullptr ATOM ... property");
									break;
								}
								if (req->selection == 0) {
									GALE_ERROR("Get nullptr ATOM ... selection");
									break;
								}
								if (req->target == 0) {
									GALE_ERROR("Get nullptr ATOM ... target");
									break;
								}
								char * atomNameProperty = XGetAtomName(m_display, req->property);
								char * atomNameSelection = XGetAtomName(m_display, req->selection);
								char * atomNameTarget = XGetAtomName(m_display, req->target);
								GALE_INFO("    from: " << atomNameProperty << "  request=" << atomNameSelection << " in " << atomNameTarget);
								if (nullptr != atomNameProperty) { XFree(atomNameProperty); }
								if (nullptr != atomNameSelection) { XFree(atomNameSelection); }
								if (nullptr != atomNameTarget) { XFree(atomNameTarget); }
							}
							#endif
							
							std::string tmpData = "";
							if (req->selection == XAtomSelection) {
								tmpData = gale::context::clipBoard::get(gale::context::clipBoard::clipboardSelection);
							} else if (req->selection == XAtomClipBoard) {
								tmpData = gale::context::clipBoard::get(gale::context::clipBoard::clipboardStd);
							}
							const char * magatTextToSend = tmpData.c_str();
							Atom listOfAtom[4];
							if(strlen(magatTextToSend) == 0 ) {
								respond.xselection.property= None;
							} else if(XAtomTargetTarget == req->target) {
								// We need to generate the list of the possibles target element of atom
								int32_t nbAtomSupported = 0;
								listOfAtom[nbAtomSupported++] = XAtomTargetTarget;
								listOfAtom[nbAtomSupported++] = XAtomTargetString;
								listOfAtom[nbAtomSupported++] = XAtomTargetStringUTF8;
								listOfAtom[nbAtomSupported++] = None;
								XChangeProperty( m_display,
								                 req->requestor,
								                 req->property,
								                 XA_ATOM,
								                 32,
								                 PropModeReplace,
								                 (unsigned char*)listOfAtom,
								                 nbAtomSupported );
								respond.xselection.property=req->property;
								GALE_INFO("             == > Respond ... (test)");
							} else if(XAtomTargetString == req->target) {
								XChangeProperty( m_display,
								                 req->requestor,
								                 req->property,
								                 req->target,
								                 8,
								                 PropModeReplace,
								                 (unsigned char*)magatTextToSend,
								                 strlen(magatTextToSend));
								respond.xselection.property=req->property;
								GALE_INFO("             == > Respond ...");
							} else if (XAtomTargetStringUTF8 == req->target) {
								XChangeProperty( m_display,
								                 req->requestor,
								                 req->property,
								                 req->target,
								                 8,
								                 PropModeReplace,
								                 (unsigned char*)magatTextToSend,
								                 strlen(magatTextToSend));
								respond.xselection.property=req->property;
								GALE_INFO("             == > Respond ...");
							} else {
								respond.xselection.property= None;
							}
							respond.xselection.type= SelectionNotify;
							respond.xselection.display= req->display;
							respond.xselection.requestor= req->requestor;
							respond.xselection.selection=req->selection;
							respond.xselection.target= req->target;
							respond.xselection.time = req->time;
							XSendEvent (m_display, req->requestor,0,0,&respond);
							// flush the message on the pipe ...
							XFlush (m_display);
							break;
						}
						///////////////////////////////////////////////////////////////////////////////////////////////////////////////
						case Expose:
							X11_INFO("X11 event Expose");
							specialEventThatNeedARedraw = true;
							break;
						case GraphicsExpose:
							X11_INFO("X11 event GraphicsExpose");
							specialEventThatNeedARedraw = true;
							break;
						case NoExpose:
							X11_INFO("X11 event NoExpose");
							break;
						case CreateNotify:
							X11_INFO("X11 event CreateNotify");
							break;
						case DestroyNotify:
							X11_INFO("X11 event DestroyNotify");
							break;
						case GravityNotify:
							X11_INFO("X11 event GravityNotify");
							break;
						case VisibilityNotify:
							X11_INFO("X11 event VisibilityNotify");
							break;
						case CirculateNotify:
							X11_INFO("X11 event CirculateNotify");
							break;
						case ReparentNotify:
							X11_INFO("X11 event ReparentNotify");
							break;
						case PropertyNotify:
							specialEventThatNeedARedraw=true;
							X11_INFO("X11 event PropertyNotify");
							break;
						case ConfigureNotify:
							specialEventThatNeedARedraw=true;
							X11_INFO("X11 event ConfigureNotify");
							if (m_display == event.xconfigure.display) {
								//GALE_INFO("X11 event ConfigureNotify event=" << (int32_t)event.xconfigure.event << "  Window=" << (int32_t)event.xconfigure.window << "  above=" << (int32_t)event.xconfigure.above << "  border_width=" << (int32_t)event.xconfigure.border_width );
								m_originX = event.xconfigure.x;
								m_originY = event.xconfigure.y;
								OS_Move(vec2(event.xconfigure.x,event.xconfigure.y));
								X11_INFO("X11 configure windows position : (" << m_originX << "," << m_originY << ")");
								m_currentHeight = event.xconfigure.height;
								m_currentWidth = event.xconfigure.width;
								X11_INFO("X11 configure windows size : (" << m_currentHeight << "," << m_currentWidth << ")");
								OS_Resize(vec2(event.xconfigure.width, event.xconfigure.height));
							}
							break;
						case ButtonPress:
							X11_INFO("X11 event ButtonPress");
							m_cursorEventX = event.xbutton.x;
							m_cursorEventY = (m_currentHeight-event.xbutton.y);
							if (event.xbutton.button < MAX_MANAGE_INPUT) {
								m_inputIsPressed[event.xbutton.button] = true;
							}
							OS_SetInput(gale::key::type::mouse,
							            gale::key::status::down,
							            event.xbutton.button,
							            vec2(event.xbutton.x, m_cursorEventY));
							break;
						case ButtonRelease:
							X11_INFO("X11 event ButtonRelease");
							m_cursorEventX = event.xbutton.x;
							m_cursorEventY = (m_currentHeight-event.xbutton.y);
							if (event.xbutton.button < MAX_MANAGE_INPUT) {
								m_inputIsPressed[event.xbutton.button] = false;
							}
							OS_SetInput(gale::key::type::mouse,
							            gale::key::status::up,
							            event.xbutton.button,
							            vec2(event.xbutton.x, m_cursorEventY));
							break;
						case EnterNotify:
							X11_INFO("X11 event EnterNotify");
							m_cursorEventX = event.xcrossing.x;
							m_cursorEventY = (m_currentHeight-event.xcrossing.y);
							//GALE_DEBUG("X11 event : " << event.type << " = \"EnterNotify\" (" << (float)event.xcrossing.x << "," << (float)event.xcrossing.y << ")");
							//gui_uniqueWindows->GenEventInput(0, gale::EVENT_INPUT_TYPE_ENTER, (float)event.xcrossing.x, (float)event.xcrossing.y);
							m_curentGrabDelta -= vec2(m_originX, -m_originY);
							GALE_VERBOSE("change grab delta of : " << vec2(m_originX, m_originY) );
							break;
						case LeaveNotify:
							X11_INFO("X11 event LeaveNotify");
							m_cursorEventX = event.xcrossing.x;
							m_cursorEventY = (m_currentHeight-event.xcrossing.y);
							//GALE_DEBUG("X11 event : " << event.type << " = \"LeaveNotify\" (" << (float)event.xcrossing.x << "," << (float)event.xcrossing.y << ")");
							m_curentGrabDelta += vec2(m_originX, -m_originY);
							GALE_VERBOSE("change grab delta of : " << vec2(m_originX, m_originY) );
							break;
						case MotionNotify:
							X11_INFO("X11 event MotionNotify");
							if(    m_grabAllEvent == true
							    && event.xmotion.x == int32_t(m_forcePos.x())
							    && event.xmotion.y == int32_t(m_forcePos.y()) ) {
								X11_VERBOSE("X11 reject mouse move (grab mode)");
								// we get our requested position...
								m_positionChangeRequested = false;
								m_curentGrabDelta = vec2(0,0);
							} else {
								m_cursorEventX = event.xmotion.x;
								m_cursorEventY = (m_currentHeight-event.xmotion.y);
								if(m_grabAllEvent == true) {
									m_cursorEventX -= m_forcePos.x();
									m_cursorEventY -= (m_currentHeight-m_forcePos.y());
								}
								vec2 newDelta = vec2(m_cursorEventX, m_cursorEventY);
								if(m_grabAllEvent == true) {
									m_cursorEventX -= m_curentGrabDelta.x();
									m_cursorEventY -= m_curentGrabDelta.y();
								}
								m_curentGrabDelta = newDelta;
								// For compatibility of the Android system : 
								bool findOne = false;
								for (int32_t iii=0; iii<MAX_MANAGE_INPUT ; iii++) {
									if (m_inputIsPressed[iii] == true) {
										X11_DEBUG("X11 event: bt=" << iii << " " << event.type << " = \"MotionNotify\" (" << m_cursorEventX << "," << m_cursorEventY << ")");
										OS_SetInput(gale::key::type::mouse,
										            gale::key::status::move,
										            iii,
										            vec2(m_cursorEventX, m_cursorEventY));
										findOne = true;
									}
								}
								if (findOne == false) {
									X11_DEBUG("X11 event: bt=" << 0 << " " << event.type << " = \"MotionNotify\" (" << m_cursorEventX << "," << m_cursorEventY << ")");
										OS_SetInput(gale::key::type::mouse,
										            gale::key::status::move,
										            0,
										            vec2(m_cursorEventX, m_cursorEventY));
								}
								if (m_grabAllEvent == true) {
									if (m_positionChangeRequested == false) {
										X11_DEBUG("X11 set pointer position : " << m_forcePos);
										XWarpPointer(m_display, None, m_WindowHandle, 0,0, 0, 0, m_forcePos.x(), m_forcePos.y());
										XFlush(m_display);
										m_positionChangeRequested = true;
									}
								}
							}
							break;
						case FocusIn:
							X11_INFO("X11 event focusIn");
							specialEventThatNeedARedraw=true;
							break;
						case FocusOut:
							X11_INFO("X11 event : focusOut");
							specialEventThatNeedARedraw=true;
							break;
						case KeyPress:
						case KeyRelease: {
							bool thisIsAReapeateKey = false;
							// filter repeate key : 
							if(    event.type == KeyRelease
							    && XEventsQueued(m_display, QueuedAfterReading)) {
								XEvent nev;
								XPeekEvent(m_display, &nev);
								if(    nev.type == KeyPress
								    && nev.xkey.time == event.xkey.time
								    && nev.xkey.keycode == event.xkey.keycode) {
									// Key was not actually released
									// remove next event too ...
									
									//This remove repeate key
									XNextEvent(m_display, &nev);
									//break;
									
									thisIsAReapeateKey = true;
								}
							}
							X11_INFO("X11 event : " << event.type << " = 'KeyPress/KeyRelease' ");
							{
								X11_DEBUG("eventKey : " << event.xkey.keycode << " state : " << event.xkey.state);
								if (event.xkey.state & (1<<0) ) {
									//GALE_DEBUG("    Special Key : SHIFT");
									m_guiKeyBoardMode.setShift(true);
								} else {
									m_guiKeyBoardMode.setShift(false);
								}
								if (event.xkey.state & (1<<1) ) {
									//GALE_DEBUG("    Special Key : CAPS_LOCK");
									m_guiKeyBoardMode.setCapsLock(true);
								} else {
									m_guiKeyBoardMode.setCapsLock(false);
								}
								if (event.xkey.state & (1<<2) ) {
									//GALE_DEBUG("    Special Key : Ctrl");
									m_guiKeyBoardMode.setCtrl(true);
								} else {
									m_guiKeyBoardMode.setCtrl(false);
								}
								if (event.xkey.state & (1<<3) ) {
									//GALE_DEBUG("    Special Key : Alt");
									m_guiKeyBoardMode.setAlt(true);
								} else {
									m_guiKeyBoardMode.setAlt(false);
								}
								if (event.xkey.state & (1<<4) ) {
									//GALE_DEBUG("    Special Key : VER_num");
									m_guiKeyBoardMode.setNumLock(true);
								} else {
									m_guiKeyBoardMode.setNumLock(false);
								}
								if (event.xkey.state & (1<<5) ) {
									GALE_DEBUG("    Special Key : MOD");
								}
								if (event.xkey.state & (1<<6) ) {
									//GALE_DEBUG("    Special Key : META");
									m_guiKeyBoardMode.setMeta(true);
								} else {
									m_guiKeyBoardMode.setMeta(false);
								}
								if (event.xkey.state & (1<<7) ) {
									//GALE_DEBUG("    Special Key : ALT_GR");
									m_guiKeyBoardMode.setAltGr(true);
								} else {
									m_guiKeyBoardMode.setAltGr(false);
								}
								bool find = true;
								enum gale::key::keyboard keyInput;
								switch (event.xkey.keycode) {
									//case 80: // keypad
									case 111:	keyInput = gale::key::keyboard::up;            break;
									//case 83: // keypad
									case 113:	keyInput = gale::key::keyboard::left;          break;
									//case 85: // keypad
									case 114:	keyInput = gale::key::keyboard::right;         break;
									//case 88: // keypad
									case 116:	keyInput = gale::key::keyboard::down;          break;
									//case 81: // keypad
									case 112:	keyInput = gale::key::keyboard::pageUp;        break;
									//case 89: // keypad
									case 117:	keyInput = gale::key::keyboard::pageDown;      break;
									//case 79: // keypad
									case 110:	keyInput = gale::key::keyboard::start;         break;
									//case 87: // keypad
									case 115:	keyInput = gale::key::keyboard::end;           break;
									case 78:	keyInput = gale::key::keyboard::stopDefil;     break;
									case 127:	keyInput = gale::key::keyboard::wait;          break;
									//case 90: // keypad
									case 118:
										keyInput = gale::key::keyboard::insert;
										if(event.type == KeyRelease) {
											if (m_guiKeyBoardMode.getInsert() == true) {
												m_guiKeyBoardMode.setInsert(false);
											} else {
												m_guiKeyBoardMode.setInsert(true);
											}
										}
										break;
									//case 84:  keyInput = gale::key::keyboardCenter; break; // Keypad
									case 67:    keyInput = gale::key::keyboard::f1; break;
									case 68:    keyInput = gale::key::keyboard::f2; break;
									case 69:    keyInput = gale::key::keyboard::f3; break;
									case 70:    keyInput = gale::key::keyboard::f4; break;
									case 71:    keyInput = gale::key::keyboard::f5; break;
									case 72:    keyInput = gale::key::keyboard::f6; break;
									case 73:    keyInput = gale::key::keyboard::f7; break;
									case 74:    keyInput = gale::key::keyboard::f8; break;
									case 75:    keyInput = gale::key::keyboard::f9; break;
									case 76:    keyInput = gale::key::keyboard::f10; break;
									case 95:    keyInput = gale::key::keyboard::f11; break;
									case 96:    keyInput = gale::key::keyboard::f12; break;
									case 66:    keyInput = gale::key::keyboard::capLock;     m_guiKeyBoardMode.setCapsLock( (event.type == KeyPress) ? true : false); break;
									case 50:    keyInput = gale::key::keyboard::shiftLeft;   m_guiKeyBoardMode.setShift   ( (event.type == KeyPress) ? true : false); break;
									case 62:    keyInput = gale::key::keyboard::shiftRight;  m_guiKeyBoardMode.setShift   ( (event.type == KeyPress) ? true : false); break;
									case 37:    keyInput = gale::key::keyboard::ctrlLeft;    m_guiKeyBoardMode.setCtrl    ( (event.type == KeyPress) ? true : false); break;
									case 105:   keyInput = gale::key::keyboard::ctrlRight;   m_guiKeyBoardMode.setCtrl    ( (event.type == KeyPress) ? true : false); break;
									case 133:   keyInput = gale::key::keyboard::metaLeft;    m_guiKeyBoardMode.setMeta    ( (event.type == KeyPress) ? true : false); break;
									case 134:   keyInput = gale::key::keyboard::metaRight;   m_guiKeyBoardMode.setMeta    ( (event.type == KeyPress) ? true : false); break;
									case 64:    keyInput = gale::key::keyboard::alt;         m_guiKeyBoardMode.setAlt     ( (event.type == KeyPress) ? true : false); break;
									case 108:   keyInput = gale::key::keyboard::altGr;       m_guiKeyBoardMode.setAltGr   ( (event.type == KeyPress) ? true : false); break;
									case 135:   keyInput = gale::key::keyboard::contextMenu; break;
									case 77:    keyInput = gale::key::keyboard::numLock;     m_guiKeyBoardMode.setNumLock ( (event.type == KeyPress) ? true : false); break;
									case 91: // Suppr on keypad
										find = false;
										if(m_guiKeyBoardMode.getNumLock() == true){
											OS_setKeyboard(m_guiKeyBoardMode,
											               gale::key::keyboard::character,
											               (event.type==KeyPress?gale::key::status::down:gale::key::status::up),
											               thisIsAReapeateKey,
											               '.');
											if (thisIsAReapeateKey == true) {
												OS_setKeyboard(m_guiKeyBoardMode,
												               gale::key::keyboard::character,
												               (event.type!=KeyPress?gale::key::status::down:gale::key::status::up),
												               thisIsAReapeateKey,
												               '.');
											}
										} else {
											OS_setKeyboard(m_guiKeyBoardMode,
											               gale::key::keyboard::character,
											               (event.type==KeyPress?gale::key::status::down:gale::key::status::up),
											               thisIsAReapeateKey,
											               0x7F);
											if (thisIsAReapeateKey == true) {
												OS_setKeyboard(m_guiKeyBoardMode,
												               gale::key::keyboard::character,
												               (event.type!=KeyPress?gale::key::status::down:gale::key::status::up),
												               thisIsAReapeateKey,
												               0x7F);
											}
										}
										break;
									case 23: // special case for TAB
										find = false;
										OS_setKeyboard(m_guiKeyBoardMode,
										               gale::key::keyboard::character,
										               (event.type==KeyPress?gale::key::status::down:gale::key::status::up),
										               thisIsAReapeateKey,
										               0x09);
										if (thisIsAReapeateKey == true) {
											OS_setKeyboard(m_guiKeyBoardMode,
											               gale::key::keyboard::character,
											               (event.type!=KeyPress?gale::key::status::down:gale::key::status::up),
											               thisIsAReapeateKey,
											               0x09);
										}
										break;
									default:
										find = false;
										{
											char buf[11];
											//GALE_DEBUG("Keycode: " << event.xkey.keycode);
											// change keystate for simple reson of the ctrl error...
											int32_t keyStateSave = event.xkey.state;
											if (event.xkey.state & (1<<2) ) {
												event.xkey.state = event.xkey.state & 0xFFFFFFFB;
											}
											KeySym keysym;
											Status status = 0;
											//int count = Xutf8LookupString(m_xic, (XKeyPressedEvent*)&event, buf, 10, &keysym, &status);
											int count = Xutf8LookupString(m_xic, &event.xkey, buf, 10, &keysym, &status);
											// retreave real keystate
											event.xkey.state = keyStateSave;
											buf[count] = '\0';
											// Replace \r error ...
											if (buf[0] == '\r') {
												buf[0] = '\n';
												buf[1] = '\0';
											}
											if (count >= 0) {
												// repeated kay from previous element :
												if (count > 0) {
													// transform it in unicode
													m_lastKeyPressed = utf8::convertChar32(buf);
												}
												X11_INFO("event Key : " << event.xkey.keycode << " char=\"" << buf << "\"'len=" << strlen(buf) << " unicode=" << m_lastKeyPressed);
												OS_setKeyboard(m_guiKeyBoardMode,
												               gale::key::keyboard::character,
												               (event.type==KeyPress?gale::key::status::down:gale::key::status::up),
												               thisIsAReapeateKey,
												               m_lastKeyPressed);
												if (thisIsAReapeateKey == true) {
													OS_setKeyboard(m_guiKeyBoardMode,
													               gale::key::keyboard::character,
													               (event.type!=KeyPress?gale::key::status::down:gale::key::status::up),
													               thisIsAReapeateKey,
													               m_lastKeyPressed);
												}
											} else {
												GALE_WARNING("Unknow event Key : " << event.xkey.keycode << " res='" << buf << "' repeate=" << thisIsAReapeateKey);
											}
										}
										break;
								}
								if (find == true) {
									//GALE_DEBUG("eventKey Move type : " << getCharTypeMoveEvent(keyInput) );
									OS_setKeyboard(m_guiKeyBoardMode,
									               keyInput,
									               (event.type==KeyPress?gale::key::status::down:gale::key::status::up),
									               thisIsAReapeateKey);
									if (thisIsAReapeateKey == true) {
										OS_setKeyboard(m_guiKeyBoardMode,
										               keyInput,
										               (event.type!=KeyPress?gale::key::status::down:gale::key::status::up),
										               thisIsAReapeateKey);
									}
								}
							}
							break;
						}
						//case DestroyNotify:
						//	break;
						case MapNotify:
							X11_INFO("X11 event : MapNotify");
							specialEventThatNeedARedraw=true;
							OS_Show();
							break;
						case UnmapNotify:
							X11_INFO("X11 event : UnmapNotify");
							specialEventThatNeedARedraw=true;
							OS_Hide();
							break;
						default:
							X11_INFO("X11 event : " << event.type << " = \"???\"");
							break;
					}
				}
				if(m_run == true) {
					if (m_doubleBuffered && hasDisplay) {
						glXSwapBuffers(m_display, m_WindowHandle);
						XSync(m_display,0);
					}
					// draw after switch the previous windows ...
					//GALE_DEBUG("specialEventThatNeedARedraw"<<specialEventThatNeedARedraw);
					hasDisplay = OS_Draw(specialEventThatNeedARedraw);
					specialEventThatNeedARedraw=false;
				}
			}
			return 0;
		}
		/****************************************************************************************/
		virtual void stop() {
			X11_INFO("X11-API: Stop");
			m_run = false;
		}
		/****************************************************************************************/
		virtual void setSize(const vec2& _size) {
			X11_INFO("X11-API: changeSize=" << _size);
			m_currentHeight = _size.y();
			m_currentWidth = _size.x();
			XResizeWindow(m_display, m_WindowHandle, _size.x(), _size.y());
		}
		/****************************************************************************************/
		void setFullScreen(bool _status) {
			X11_INFO("X11-API: changeFullscreen=" << _status);
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
		}
		/****************************************************************************************/
		virtual void grabKeyboardEvents(bool _status) {
			if (_status == true) {
				X11_INFO("X11-API: Grab Keyboard Events");
				XGrabKeyboard(m_display, m_WindowHandle,
				              False,
				              GrabModeAsync,
				              GrabModeAsync,
				              CurrentTime);
			} else {
				X11_INFO("X11-API: Un-Grab Keyboard Events");
				XUngrabKeyboard(m_display, CurrentTime);
			}
		}
		/****************************************************************************************/
		virtual void setWindowsDecoration(bool _status) {
			X11_INFO("X11-API: setWindows Decoration :" << _status);
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
		};
		/****************************************************************************************/
		virtual void setPos(const vec2& _pos) {
			X11_INFO("X11-API: changePos=" << _pos);
			m_windowsPos = _pos;
			XMoveWindow(m_display, m_WindowHandle, _pos.x(), _pos.y());
			m_originX = _pos.x();
			m_originY = _pos.y();
		}
		/****************************************************************************************/
		/*
		virtual void getAbsPos(ivec2& pos) {
			X11_INFO("X11-API: getAbsPos");
			int tmp;
			unsigned int tmp2;
			Window fromroot, tmpwin;
			XQueryPointer(m_display, m_WindowHandle, &fromroot, &tmpwin, &pos.m_floats[0], &pos.m_floats[1], &tmp, &tmp, &tmp2);
		}
		*/
		/****************************************************************************************/
		virtual void setCursor(enum gale::context::cursor _newCursor) {
			if (_newCursor != m_currentCursor) {
				X11_DEBUG("X11-API: set New Cursor : " << _newCursor);
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
		}
		/****************************************************************************************/
		void grabPointerEvents(bool _status, const vec2& _forcedPosition) {
			if (_status == true) {
				X11_DEBUG("X11-API: Grab Events");
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
				X11_DEBUG("X11-API: set pointer position : " << m_forcePos);
				XWarpPointer(m_display, None, m_WindowHandle, 0,0, 0, 0, m_forcePos.x(), m_forcePos.y());
				XFlush(m_display);
				m_positionChangeRequested = true;
				m_curentGrabDelta = vec2(0,0);
			} else {
				X11_DEBUG("X11-API: Un-Grab Events");
				XUngrabPointer(m_display, CurrentTime);
				m_grabAllEvent = false;
				m_forcePos = vec2(0,0);
				m_curentGrabDelta = vec2(0,0);
			}
		}
		/****************************************************************************************/
		bool createX11Context() {
			X11_INFO("X11: CreateX11Context");
			int x,y, attr_mask;
			XSizeHints hints;
			XWMHints *StartupState;
			XTextProperty textprop;
			XSetWindowAttributes attr;
			// basic title of the windows ...
			static char *title = (char*)"Gale";
			
			// Connect to the X server
			m_display = XOpenDisplay(nullptr);
			if(m_display == nullptr) {
				GALE_CRITICAL("Could not open display X.");
				exit(-1);
			} else {
				GALE_INFO("Display opened.");
			}
			int Xscreen = DefaultScreen(m_display);
			// set the DPI for the current screen :
			gale::Dimension::setPixelRatio(vec2((float)DisplayWidth(m_display, Xscreen)/(float)DisplayWidthMM(m_display, Xscreen),
			                                    (float)DisplayHeight(m_display, Xscreen)/(float)DisplayHeightMM(m_display, Xscreen)),
			                               gale::distance::millimeter);
			// get an appropriate visual
			m_visual = glXChooseVisual(m_display, Xscreen, attrListDbl);
			if (m_visual == nullptr) {
				m_visual = glXChooseVisual(m_display, Xscreen, attrListSgl);
				m_doubleBuffered = false;
				GALE_INFO("GL-X singlebuffered rendering will be used, no doublebuffering available");
			} else {
				m_doubleBuffered = true;
				GALE_INFO("GL-X doublebuffered rendering available");
			}
			{
				int32_t glxMajor, glxMinor;
				glXQueryVersion(m_display, &glxMajor, &glxMinor);
				GALE_INFO("GLX-Version " << glxMajor << "." << glxMinor);
			}
			// Create a colormap - only needed on some X clients, eg. IRIX
			Window Xroot = RootWindow(m_display, Xscreen);
			attr.colormap = XCreateColormap(m_display, Xroot, m_visual->visual, AllocNone);
			
			attr.border_pixel = 0;
			attr.event_mask =   StructureNotifyMask
			                  | SubstructureNotifyMask
			                  | EnterWindowMask
			                  | LeaveWindowMask
			                  | ExposureMask
			                  | ButtonPressMask
			                  | ButtonReleaseMask
			                  | OwnerGrabButtonMask
			                  | KeyPressMask
			                  | KeyReleaseMask
			                  | PointerMotionMask
			                  | FocusChangeMask
			                  | SubstructureRedirectMask;
			
			// set no background at the gui
			attr.background_pixmap = None;
			
			// select internal attribute
			attr_mask = CWBackPixmap | CWColormap | CWBorderPixel | CWEventMask;
			// Create the window
			int32_t tmp_width = 640;//DisplayWidth(m_display, DefaultScreen(m_display))/2;
			int32_t tmp_height = 480;//DisplayHeight(m_display, DefaultScreen(m_display))/2;
			OS_Resize(vec2(tmp_width, tmp_height));
			x=20;
			y=20;
			m_originX = x;
			m_originY = y;
			GALE_INFO("X11 request creating windows at pos=(" << m_originX << "," << m_originY << ") size=(" << tmp_width << "," << tmp_height << ")" );
			// Real create of the window
			m_WindowHandle = XCreateWindow(m_display,
			                               Xroot,
			                               x, y, tmp_width, tmp_height,
			                               1,
			                               m_visual->depth,
			                               InputOutput,
			                               m_visual->visual,
			                               attr_mask, &attr);
			
			if(!m_WindowHandle) {
				GALE_CRITICAL("Couldn't create the window");
				exit(-1);
			}
			
			/* Configure it...  (ok, ok, this next bit isn't "minimal") */
			textprop.value = (unsigned char*)title;
			textprop.encoding = XA_STRING;
			textprop.format = 8;
			textprop.nitems = strlen(title);
			
			hints.x = x;
			hints.y = y;
			hints.width = tmp_width;
			hints.height = tmp_height;
			hints.flags = USPosition|USSize;
			
			StartupState = XAllocWMHints();
			StartupState->initial_state = NormalState;
			StartupState->flags = StateHint;
			
			XSetWMProperties(m_display, m_WindowHandle,&textprop, &textprop,/* Window title/icon title*/
			                 nullptr, 0,/* Argv[], argc for program*/
			                 &hints, /* Start position/size*/
			                 StartupState,/* Iconised/not flag   */
			                 nullptr);
			
			XFree(StartupState);
			
			/* open it, wait for it to appear */
			XMapWindow(m_display, m_WindowHandle);
			//XIfEvent(m_display, &event, WaitForMapNotify, (char*)&m_WindowHandle);
			
			
			m_xim = XOpenIM(m_display, nullptr, NULL, NULL);
			if (m_xim == nullptr) {
				GALE_ERROR("Could not open input method");
				return false;
			}
			/*
			XIMStyles *styles=nullptr;
			char* failed_arg = XGetIMValues(m_xim, XNQueryInputStyle, &styles, nullptr);
			if (failed_arg != nullptr) {
				GALE_ERROR("XIM Can't get styles");
				return false;
			}
			for (int32_t iii=0; iii<styles->count_styles; ++iii) {
				GALE_INFO("style " << styles->supported_styles[iii]);
			}
			*/
			m_xic = XCreateIC(m_xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, m_WindowHandle, nullptr);
			if (m_xic == nullptr) {
				GALE_ERROR("Could not open IC");
				return false;
			}
			
			XSetICFocus(m_xic);
			
			// set the kill atom so we get a message when the user tries to close the window
			if ((m_delAtom = XInternAtom(m_display, "WM_DELETE_WINDOW", 0)) != None) {
				XSetWMProtocols(m_display, m_WindowHandle, &m_delAtom, 1);
			}
			
			vec2 tmpSize(400, 300);
			OS_Resize(tmpSize);
			
			return true;
		}
		/****************************************************************************************/
		void setIcon(const std::string& _inputFile) {
			#ifdef GALE_BUILD_EGAMI
				egami::Image dataImage = egami::load(_inputFile);
				// load data
				if (dataImage.exist() == false) {
					GALE_ERROR("Error when loading Icon");
					return;
				}
				int32_t depth = DefaultDepth(m_display, DefaultScreen(m_display) );
				GALE_DEBUG("X11 Create icon size=(" << dataImage.getWidth() << "," << dataImage.getHeight() << ") depth=" << depth);
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
						GALE_ERROR("X11: BadAlloc");
						break;
					case BadDrawable:
						GALE_ERROR("X11: BadDrawable");
						break;
					case BadValue:
						GALE_ERROR("X11: BadValue");
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
						GALE_ERROR("X11: BadDrawable");
						break;
					case BadGC:
						GALE_ERROR("X11: BadGC");
						break;
					case BadMatch:
						GALE_ERROR("X11: BadMatch");
						break;
					case BadValue:
						GALE_ERROR("X11: BadValue");
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
		}
		/****************************************************************************************/
		static void setVSync(bool _sync) {
			// Function pointer for the wgl extention function we need to enable/disable
			typedef int32_t (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
			PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
			const char *extensions = (char*)glGetString( GL_EXTENSIONS );
			if (strstr( extensions, "WGL_EXT_swap_control" ) == 0) {
				GALE_ERROR("Can not set the vertical synchronisation status" << _sync << "   (1)");
				return;
			} else {
				wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)glXGetProcAddress( (const GLubyte *)"wglSwapIntervalEXT" );
				if(wglSwapIntervalEXT) {
					wglSwapIntervalEXT(_sync);
				} else {
					GALE_ERROR("Can not set the vertical synchronisation status" << _sync << "   (2)");
				}
			}
		}
		/****************************************************************************************/
		bool createOGlContext() {
			X11_INFO("X11:CreateOGlContext");
			/* create a GLX context */
			GLXContext RenderContext = glXCreateContext(m_display, m_visual, 0, GL_TRUE);
			/* connect the glx-context to the window */
			glXMakeCurrent(m_display, m_WindowHandle, RenderContext);
			if (glXIsDirect(m_display, RenderContext)) {
				GALE_INFO("XF86 DRI enabled\n");
			} else {
				GALE_INFO("XF86 DRI NOT available\n");
			}
			// enable vertical synchronisation : (some computer has synchronisation disable)
			setVSync(true);
			return true;
		}
		/****************************************************************************************/
		void setTitle(const std::string& _title) {
			X11_INFO("X11: set Title (START)");
			XTextProperty tp;
			tp.value = (unsigned char *)_title.c_str();
			tp.encoding = XA_WM_NAME;
			tp.format = 8;
			tp.nitems = strlen((const char*)tp.value);
			XSetWMName(m_display, m_WindowHandle, &tp);
			XStoreName(m_display, m_WindowHandle, (const char*)tp.value);
			XSetIconName(m_display, m_WindowHandle, (const char*)tp.value);
			XSetWMIconName(m_display, m_WindowHandle, &tp);
			X11_INFO("X11: set Title (END)");
		}
		void openURL(const std::string& _url) {
			std::string req = "xdg-open ";
			req += _url;
			system(req.c_str());
			return;
		}
		/****************************************************************************************/
		void clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					if (m_clipBoardOwnerPrimary == false) {
						m_clipBoardRequestPrimary = true;
						// generate a request on X11
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
						// generate a request on X11
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
		}
		/****************************************************************************************/
		void clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
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
		}
};


/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	etk::init(_argc, _argv);
	X11Interface* interface = new X11Interface(_application, _argc, _argv);
	if (interface == nullptr) {
		GALE_CRITICAL("Can not create the X11 interface ... MEMORY allocation error");
		return -2;
	}
	int32_t retValue = interface->run();
	delete(interface);
	interface = nullptr;
	return retValue;
}
