/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

/*
 notes : 
	sudo edn /etc/udev/rules.d/framebuffer.rules
		KERNEL == "fb0", OWNER="root", MODE="0660"
	sudo usermod -a -G video username
	sudo usermod -a -G tty username
	sudo fbset -i
	sudo chmod o+wr /dev/fb0
	
	
	http://mail.directfb.org/pipermail/directfb-users/2010-February/002115.html
	on X11 :
	http://stackoverflow.com/questions/521957/how-to-develop-a-directfb-app-without-leaving-x-11-environment
	$ sudo apt-get install libdirectfb-extra  # for Debian and Ubuntu, anyhow
	$ cat ~/.directfbrc
		system=x11
		force-windowed
*/

#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/key.hpp>
#include <gale/config.hpp>
#include <gale/commandLine.hpp>
#include <etk/types.hpp>
#include <etk/unicode.hpp>
#include <gale/widget/Manager.hpp>

#include <gale/renderer/ResourceManager.hpp>
#include <gale/renderer/eSystem.hpp>
#include <gale/openGL/openGL.hpp>

#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <directfb.h>
#include <directfbgl.h>


int64_t guiInterface::getTime() {
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


static gale::SpecialKey guiKeyBoardMode;

bool inputIsPressed[20];
bool m_run = true;
bool m_grabAllEvent = false;


// the super interface
IDirectFB *dfb = nullptr;

// the primary surface (surface of primary layer)
IDirectFBSurface *primary = nullptr;

// the GL context
IDirectFBGL *primary_gl = nullptr;

// event buffer
IDirectFBEventBuffer *events = nullptr;

static int screen_width =800;
static int screen_height = 600;


/**
 * @brief set the new title of the windows
 * @param title New desired title
 * @return ---
 */
void guiInterface::setTitle(std::string& title) {
	// TODO : ...
}


void guiInterface::setIcon(std::string inputFile) {
	// TODO : ...
}


void DirectFB_Init(int argc, const char *argv[]) {
	GALE_INFO("DirectFB init (START)");
	DFBResult err;
	DFBSurfaceDescription dsc;

	GALE_INFO("call DirectFBInit");
	int argc2 = 1;
	err = DirectFBInit(&argc2, (char***)&argv);
	if (DFB_OK!=err) {
		GALE_ERROR("DirectFBInit");
		DirectFBErrorFatal("DirectFBInit", err);
		exit(-1);
	}
	GALE_INFO("call DirectFBCreate");
	// create the super interface
	err = DirectFBCreate(&dfb);
	if (DFB_OK!=err) {
		GALE_ERROR("DirectFBCreate");
		DirectFBErrorFatal("DirectFBCreate", err);
		exit(-1);
	}
	
	GALE_INFO("call setCooperativeLevel");
	// set our cooperative level to DFSCL_FULLSCREEN for exclusive access to the primary layer
	dfb->setCooperativeLevel(dfb, DFSCL_FULLSCREEN);
	//dfb->setCooperativeLevel(dfb, DFSCL_NORMAL);

	// get the primary surface, i.e. the surface of the primary layer we have exclusive access to
	dsc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS);// | DSDESC_PIXELFORMAT);
	//dsc.caps  = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY | DSCAPS_DOUBLE | DSCAPS_DEPTH); // DSCAPS_SHARED
	dsc.caps  = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY | DSCAPS_DOUBLE | DSCAPS_GL);// | DSCAPS_FLIPPING);
	//dsc.caps  = (DFBSurfaceCapabilities) (DSCAPS_SUBSURFACE | DSCAPS_VIDEOONLY | DSCAPS_PREMULTIPLIED | DSCAPS_FLIPPING);

	
	dsc.pixelformat = DSPF_ARGB;
	dsc.width = 600;
	dsc.height = 400;
	
	GALE_INFO("call CreateSurface");
	err = dfb->CreateSurface(dfb, &dsc, &primary);
	if (DFB_OK!=err) {
		GALE_ERROR("dfb->CreateSurface");
		DirectFBErrorFatal("dfb->CreateSurface", err);
		exit(-1);
	}
	
	primary->setBlittingFlags(primary, DSBLIT_BLEND_ALPHACHANNEL);
	primary->setPorterDuff( primary, DSPD_ADD );
	primary->setDstBlendFunction(primary, DSBF_SRCALPHA);
	primary->setDrawingFlags(primary, DSDRAW_BLEND);
	
	primary->Blit(primary, primary, nullptr, 0, 0);
	
	
	GALE_INFO("call getSize");
	// get the size of the surface and fill it
	err = primary->getSize(primary, &screen_width, &screen_height);
	if (DFB_OK!=err) {
		GALE_ERROR("primary->getSize");
		DirectFBErrorFatal("primary->getSize", err);
		exit(-1);
	}
	

	GALE_INFO("call CreateInputEventBuffer");
	// create an event buffer for all devices with these caps
	err = dfb->CreateInputEventBuffer(dfb, (DFBInputDeviceCapabilities)(DICAPS_KEYS | DICAPS_AXES), DFB_FALSE, &events);
	if (DFB_OK!=err) {
		GALE_ERROR("dfb->CreateInputEventBuffer");
		DirectFBErrorFatal("CreateInputEventBuffer", err);
		exit(-1);
	}
	
	GALE_INFO("call Flip");
	primary->Flip(primary, nullptr, (DFBSurfaceFlipFlags)0);//DSFLIP_ONSYNC);

	// NOTE : we need to force it on X11 display ...
	GALE_INFO("call getGL");
	// get the GL context
	err = primary->getGL(primary, &primary_gl);
	if (DFB_OK!=err) {
		GALE_ERROR("primary->getGL");
		DirectFBErrorFatal("GetGL", err);
		exit(-1);
	}

	GALE_INFO("DirectFB init (STOP)");
}

void DirectFB_UnInit() {
	// release our interfaces to shutdown DirectFB
	primary_gl->release(primary_gl);
	primary->release(primary);
	events->release(events);
	dfb->release(dfb);
}

void DirectFB_Run() {
	GALE_INFO("X11 configure windows size : (" << screen_height << "," << screen_width << ")");
	eSystem::Resize(screen_width, screen_height);
	
	DFBResult err;
	int32_t position = 0;
	while (true == m_run) {
		DFBInputEvent evt;
		unsigned long t;
	/*
	primary->setColor (primary, 0x00, 0x00, 0x00, 0xFF);
	primary->FillRectangle(primary, 0, 0, screen_width, screen_height);
	primary->setColor (primary, 0xFF, (uint8_t)position, 0x00, 0xFF);
	primary->FillRectangle(primary, position, position, 300, 300);
	primary->Flip(primary, nullptr, (DFBSurfaceFlipFlags)0);//DSFLIP_ONSYNC);
		position++;
		if (position>600) {
			position = 0;
		}
		*/
		if(true == m_run) {
			err = primary_gl->Lock(primary_gl);
			if (DFB_OK!=err) {
				GALE_ERROR("primary_gl->Lock");
				DirectFBErrorFatal("primary_gl->Lock", err);
			}
			// TODO : set at false
			bool hasDisplay = eSystem::draw(true);
			if (true == hasDisplay) {
				glFinish();
			}
			err = primary_gl->Unlock(primary_gl);
			if (DFB_OK!=err) {
				GALE_ERROR("primary_gl->Unlock");
				DirectFBErrorFatal("primary_gl->Unlock", err);
			}
			primary->Flip(primary, nullptr, (DFBSurfaceFlipFlags)0);//DSFLIP_ONSYNC);
		}
		
		while (events->getEvent(events, DFB_EVENT(&evt)) == DFB_OK) {
			switch (evt.type) {
				default:
				case DIET_UNKNOWN: /* unknown event */
					GALE_DEBUG("event unknown type : " << evt.type << " symbole=\"" << (char)evt.key_symbol << "\"=" << ((int32_t)evt.key_symbol) << " ...");
					break;
				case DIET_KEYPRESS: /* a key is been pressed */
				case DIET_KEYRELEASE: /* a key is been released */
					{
						bool isPressed = (evt.type == DIET_KEYPRESS);
						//GALE_DEBUG("event KeyBoard isPressed : " << isPressed << " symbole=\"" << (char)evt.key_symbol << "\"=" << ((int32_t)evt.key_symbol) << " ...");
						if( 1 <= evt.key_symbol && evt.key_symbol <= 0x7F ) {
							eSystem::setKeyboard(guiKeyBoardMode, evt.key_symbol, isPressed, false);
						} else {
							GALE_DEBUG("event KeyBoard isPressed : " << isPressed << " symbole=\"" << (char)evt.key_symbol << "\"=" << ((int32_t)evt.key_symbol) << "  == > not managed key");
						}
					}
					break;
				case DIET_AXISMOTION: /* mouse/joystick movement */
					{
						/*
						if (evt.flags & DIEF_AXISREL) {
							switch (evt.axis) {
								case DIAI_X:
									//view_roty += evt.axisrel / 2.0;
									break;
								case DIAI_Y:
									//view_rotx += evt.axisrel / 2.0;
									break;
								case DIAI_Z:
									//view_rotz += evt.axisrel / 2.0;
									break;
								default:
									;
							}
						}
						*/
						GALE_DEBUG("event mouse motion flag" << (char)evt.flags << " axis=" << evt.axis << " value=" << evt.axisrel);
					}
					break;
				case DIET_BUTTONPRESS:   /* a (mouse) button is been pressed */
				case DIET_BUTTONRELEASE: /* a (mouse) button is been released */
					{
						bool isPressed = (evt.type == DIET_KEYPRESS);
						GALE_DEBUG("event mouse event pressed=" << isPressed << " flag" << (char)evt.flags << " axis=" << evt.axis << " value=" << evt.axisrel);
					}
					break;
				/*
				case DIET_KEYPRESS:
					switch (evt.key_symbol) {
						case DIKS_ESCAPE:
							m_run = false;
							break;
						case DIKS_CURSOR_UP:
							//inc_rotx = 5.0;
							break;
						case DIKS_CURSOR_DOWN:
							//inc_rotx = -5.0;
							break;
						case DIKS_CURSOR_LEFT:
							//inc_roty = 5.0;
							break;
						case DIKS_CURSOR_RIGHT:
							//inc_roty = -5.0;
							break;
						case DIKS_PAGE_UP:
							//inc_rotz = 5.0;
							break;
						case DIKS_PAGE_DOWN:
							//inc_rotz = -5.0;
							break;
						default:
							;
					}
					break;
				case DIET_KEYRELEASE:
					switch (evt.key_symbol) {
						case DIKS_CURSOR_UP:
							//inc_rotx = 0;
							break;
						case DIKS_CURSOR_DOWN:
							//inc_rotx = 0;
							break;
						case DIKS_CURSOR_LEFT:
							//inc_roty = 0;
							break;
						case DIKS_CURSOR_RIGHT:
							//inc_roty = 0;
							break;
						case DIKS_PAGE_UP:
							//inc_rotz = 0;
							break;
						case DIKS_PAGE_DOWN:
							//inc_rotz = 0;
							break;
						default:
							;
					}
					break;
				case DIET_AXISMOTION:
					if (evt.flags & DIEF_AXISREL) {
						switch (evt.axis) {
							case DIAI_X:
								//view_roty += evt.axisrel / 2.0;
								break;
							case DIAI_Y:
								//view_rotx += evt.axisrel / 2.0;
								break;
							case DIAI_Z:
								//view_rotz += evt.axisrel / 2.0;
								break;
							default:
								;
						}
					}
					break;
				default:
					;
				*/
			}
		}
	}
	// Note : if we not stop like this the system break ...
	exit(-1);
};


// -------------------------------------------------------------------------
//         ClipBoard AREA :
// -------------------------------------------------------------------------


void guiInterface::ClipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	// TODO : ...
}


void guiInterface::ClipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	// TODO : ...
}

void guiInterface::Stop() {
	GALE_INFO("X11-API: Stop");
	m_run = false;
}

void guiInterface::KeyboardShow() {
	// nothing to do : No keyboard on computer ...
}


void guiInterface::KeyboardHide() {
	// nothing to do : No keyboard on computer ...
}


void guiInterface::changeSize(ivec2 _size) {
	// TODO : ...
}


void guiInterface::changePos(ivec2 _pos) {
	// TODO : ...
}


void guiInterface::getAbsPos(ivec2& _pos) {
	// TODO : ...
}


void guiInterface::setCursor(enum gale::cursorDisplay _newCursor) {
	// TODO : ...
}

void guiInterface::GrabPointerEvents(bool _isGrabbed, vec2 _forcedPosition) {
	// TODO : ...
}


/**
 * @brief Main of the program
 * @param std IO
 * @return std IO
 */
int guiInterface::main(int argc, const char *argv[]) {
	GALE_INFO("Main (START)");
	for (int32_t iii=0; iii<NB_MAX_INPUT; iii++) {
		inputIsPressed[iii] = false;
	}
	m_grabAllEvent = false;
	// start X11 thread ...
	DirectFB_Init(argc, argv);
	//start the basic thread : 
	eSystem::init();
	// Run ...
	DirectFB_Run();
	// UnInit:
	DirectFB_UnInit();
	// close X11 :
	guiInterface::Stop();
	// uninit ALL :
	eSystem::UnInit();
	GALE_INFO("Main (STOP)");
	return 0;
}


void guiInterface::forceOrientation(enum gale::orientation _orientation) {
	// nothing to do ...
}


/*
static void init(int argc, char *argv[]) {

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);

	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	for (i=1; i<argc; i++)
	{
		if (strcmp(argv[i],"-info") == 0)
		{
			printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
			printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
			printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
			printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
		}
		else if (strcmp(argv[i],"-exit") == 0)
		{
			autoexit = 30;
			printf("Auto Exit after %i seconds.\n", autoexit );
		}
	}

	// make the gears
	gear1 = gear(1.0, 4.0, 1.0, 20, 0.7, red);
	gear2 = gear(0.5, 2.0, 2.0, 10, 0.7, green);
	gear3 = gear(1.3, 2.0, 0.5, 10, 0.7, blue);
}
*/
