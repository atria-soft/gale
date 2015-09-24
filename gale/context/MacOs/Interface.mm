/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#include "Context.h"
#import <Cocoa/Cocoa.h>
#include "gale/context/MacOs/Interface.h"

#import <gale/context/MacOs/OpenglView.h>
#import <gale/context/MacOs/Windows.h>
#import <gale/context/MacOs/AppDelegate.h>
#import <gale/debug.h>

id window = nil;

void callbackSomeThingToDo() {
	//GALE_CRITICAL("ksdjlkqjsdlfkjsqdlkfjslqkdjflqksjdf");
	[window UpdateScreenRequested];
}

int mm_main(int _argc, const char* _argv[]) {
	[NSAutoreleasePool new];
	
	[NSApplication sharedApplication];
	// set the quit policy and all stadard for Mac
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	
	// ---------------------------------------------------------------
	// -- basic menu bar creation :
	// ---------------------------------------------------------------
	// set the basic menu bar (stadard on Mac OSX)
	id menubar = [[NSMenu new] autorelease];
		//add an item
		id appMenuItem = [[NSMenuItem new] autorelease];
		// add the item to the menu bar: 
		[menubar addItem:appMenuItem];
	// set the main menu in the menu bar ...
	[NSApp setMainMenu:menubar];
	
	id appMenu = [[NSMenu new] autorelease];
	id appName = [[NSProcessInfo processInfo] processName];
	// create the label to qui the application :
	id quitTitle = [@"Quit " stringByAppendingString:appName];
	// create the item to quit the appllication with META+q at shortCut
	id quitMenuItem = [ [ [NSMenuItem alloc] initWithTitle:quitTitle
	                      action:@selector(stop:) keyEquivalent:@"q"] autorelease];
	
	// add the item to the menu:
	[appMenu addItem:quitMenuItem];
	// set the application menu to the main app menu ...
	[appMenuItem setSubmenu:appMenu];
	
	// ---------------------------------------------------------------
	// -- basic windows creation :
	// ---------------------------------------------------------------
	// create a windows of size 800/600
	window = [ [ [GaleMainWindows alloc] initWithContentRect:NSMakeRect(0, 0, 800, 600)
	           styleMask:(NSTitledWindowMask|NSMiniaturizableWindowMask|NSClosableWindowMask) backing:NSBackingStoreBuffered defer:NO]
	           autorelease];
	[window setAcceptsMouseMovedEvents:YES];
	//id window = [ [MacOsAppDelegate alloc] autorelease];
	
	 // set the windows at a specific position :
	[window cascadeTopLeftFromPoint:NSMakePoint(50,50)];
	// set the windows resizable
	[window setStyleMask:[window styleMask] | NSResizableWindowMask];
	// oposite : [window setStyleMask:[window styleMask] & ~NSResizableWindowMask];
	// set the title
	[window setTitle:appName];
	
	[window setAcceptsMouseMovedEvents:YES];
	// ???
	[window makeKeyAndOrderFront:nil];
	[NSApp activateIgnoringOtherApps:YES];
	
	NSRect window_frame = [window frame];
	
	OpenGLView* view=[[OpenGLView alloc]initWithFrame:window_frame]; //NSMakeRect(0, 0, 800, 600)];
	NSTrackingArea *track = [[NSTrackingArea alloc] initWithRect:window_frame options: NSTrackingMouseMoved | NSTrackingActiveWhenFirstResponder | NSTrackingActiveInKeyWindow
														   owner:window userInfo:nil];
	[view addTrackingArea:track];
	[window setContentView:view];
	[view setAutoresizesSubviews:YES];
	return 0;
}

int mm_run(void) {
	//MacOs::setRedrawCallback(std::bind(callbackSomeThingToDo));
	[NSApp run];
	GALE_DEBUG("END of application");
	// return no error
	return 0;
}

void mm_stopApplication() {
	GALE_DEBUG("NSApp terminate start.");
	[window closeRequestGale];
	[NSApp stop:nil];
	GALE_DEBUG("NSApp terminate done");
}

