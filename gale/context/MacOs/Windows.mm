/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#import <gale/context/MacOs/Windows.hpp>
#include <gale/context/MacOS/Context.hpp>
#include <gale/key/key.hpp>

#include <gale/debug.hpp>
#include <etk/stdTools.hpp>

@implementation GaleMainWindows

+ (id)alloc {
	id windowsID = [super alloc];
	GALE_DEBUG("ALLOCATE ...");
	return windowsID;
}

- (id)init {
	id windowsID = [super init];
	
	//[NSApp setDelegate: self];
	
	GALE_DEBUG("INIT ...");
	// set the windows at a specific position :
	[windowsID cascadeTopLeftFromPoint:NSMakePoint(50,50)];
	GALE_DEBUG("ALLOCATE ...");
	// set the windows resizable
	#ifdef __MAC_10_12
		[windowsID setStyleMask:[windowsID styleMask] | NSWindowStyleMaskResizable];
	#else
		[windowsID setStyleMask:[windowsID styleMask] | NSResizableWindowMask];
	#endif
	GALE_DEBUG("ALLOCATE ...");
	// oposite : [window setStyleMask:[window styleMask] & ~NSResizableWindowMask];
	// set the title
	id appName = [[NSProcessInfo processInfo] processName];
	GALE_DEBUG("ALLOCATE ...");
	[windowsID setTitle:appName];
	GALE_DEBUG("ALLOCATE ...");
	
	[windowsID setAcceptsMouseMovedEvents:YES];
	GALE_DEBUG("ALLOCATE ...");
	// ???
	[windowsID makeKeyAndOrderFront:nil];
	GALE_DEBUG("ALLOCATE ...");
	[NSApp activateIgnoringOtherApps:YES];
	GALE_DEBUG("ALLOCATE ...");
	
	NSRect window_frame = [windowsID frame];
	GALE_DEBUG("ALLOCATE ...");
	
	_view=[[OpenGLView alloc]initWithFrame:window_frame]; //NSMakeRect(0, 0, 800, 600)];
	GALE_DEBUG("ALLOCATE ...");
	[windowsID setContentView:_view];
	GALE_DEBUG("ALLOCATE ...");
	[_view setAutoresizesSubviews:YES];
	GALE_DEBUG("ALLOCATE ...");
	
	// Override point for customization after application launch.
	//[window addSubview:view];
	//[window addChildWindow:view];
	//[window makeKeyAndVisible];
	
	//[windowsID setDelegate:view];
	GALE_DEBUG("ALLOCATE ...");
	return windowsID;
}


+ (void)dealloc {
	GALE_ERROR("FREE ...");
	//[_window release];
	[super dealloc];
}

+ (void)performClose:(id)sender {
	GALE_ERROR("perform close ...");
}

static gale::key::Special guiKeyBoardMode;


-(void)localKeyEvent:(NSEvent*)theEvent isDown:(bool)_isDown {
	bool thisIsAReapeateKey = false;
	if ([theEvent isARepeat]) {
		thisIsAReapeateKey = true;
	}
	NSString *str = [theEvent charactersIgnoringModifiers];
	// TODO : set if for every char in the string !!!
	unichar c = [str characterAtIndex:0];
	GALE_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown);
	bool find = true;
	enum gale::key::keyboard keyInput;
	switch (c) {
		case 63232:	keyInput = gale::key::keyboard::up;            break;
		case 63233:	keyInput = gale::key::keyboard::down;          break;
		case 63234:	keyInput = gale::key::keyboard::left;          break;
		case 63235:	keyInput = gale::key::keyboard::right;         break;
		case 63276:	keyInput = gale::key::keyboard::pageUp;        break;
		case 63277:	keyInput = gale::key::keyboard::pageDown;      break;
		case 63273:	keyInput = gale::key::keyboard::start;         break;
		case 63275:	keyInput = gale::key::keyboard::end;           break;
		/*
		case 78:	keyInput = gale::key::keyboard::stopDefil;     break;
		case 127:	keyInput = gale::key::keyboard::wait;          break;
		*/
		case 63302:
			find = false;
			keyInput = gale::key::keyboard::insert;
			if(_isDown == false) {
				if (true == guiKeyBoardMode.getInsert()) {
					guiKeyBoardMode.setInsert(false);
				} else {
					guiKeyBoardMode.setInsert(true);
				}
			}
			GALE_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown);
			MacOs::setKeyboardMove(guiKeyBoardMode, keyInput, true, thisIsAReapeateKey);
			GALE_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << !_isDown);
			MacOs::setKeyboardMove(guiKeyBoardMode, keyInput, false, thisIsAReapeateKey);
			break;
			//case 84:  keyInput = gale::key::keyboardCenter; break; // Keypad
		case 63236:    keyInput = gale::key::keyboard::f1; break;
		case 63237:    keyInput = gale::key::keyboard::f2; break;
		case 63238:    keyInput = gale::key::keyboard::f3; break;
		case 63239:    keyInput = gale::key::keyboard::f4; break;
		case 63240:    keyInput = gale::key::keyboard::f5; break;
		case 63241:    keyInput = gale::key::keyboard::f6; break;
		case 63242:    keyInput = gale::key::keyboard::f7; break;
		case 63243:    keyInput = gale::key::keyboard::f8; break;
		case 63244:    keyInput = gale::key::keyboard::f9; break;
		case 63245:    keyInput = gale::key::keyboard::f10; break;
		case 63246:    keyInput = gale::key::keyboard::f11; break;
		case 63247:    keyInput = gale::key::keyboard::f12; break;
		case 63272: // Suppress
			find = false;
			MacOs::setKeyboard(guiKeyBoardMode, u32char::Delete, _isDown, thisIsAReapeateKey);
			if (true == thisIsAReapeateKey) {
				MacOs::setKeyboard(guiKeyBoardMode, u32char::Delete, !_isDown, thisIsAReapeateKey);
			}
			break;
		default:
			find = false;
			{
				if (guiKeyBoardMode.getAlt() == true) {
					// special keyboard transcription ...
					str = [theEvent characters];
					c = [str characterAtIndex:0];
				}
				GALE_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown);
				MacOs::setKeyboard(guiKeyBoardMode, c, _isDown, thisIsAReapeateKey);
				if (true==thisIsAReapeateKey) {
					MacOs::setKeyboard(guiKeyBoardMode, c, !_isDown, thisIsAReapeateKey);
				}
			}
			break;
	}
	if (find == true) {
		GALE_VERBOSE("eventKey Move type : " << keyInput );
		MacOs::setKeyboardMove(guiKeyBoardMode, keyInput, _isDown, thisIsAReapeateKey);
		if (true == thisIsAReapeateKey) {
			MacOs::setKeyboardMove(guiKeyBoardMode, keyInput, !_isDown, thisIsAReapeateKey);
		}
	}
	
}

- (void)keyDown:(NSEvent *)theEvent {
	[self localKeyEvent:theEvent isDown:true];
}

- (void)keyUp:(NSEvent *)theEvent {
	[self localKeyEvent:theEvent isDown:false];
}

- (void)flagsChanged:(NSEvent *)theEvent {
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagCapsLock) != 0) {
	#else
		if (([theEvent modifierFlags] & NSAlphaShiftKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagCapsLock");
		if (guiKeyBoardMode.getCapsLock() == false) {
			guiKeyBoardMode.setCapsLock(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::capLock, true, false);
		}
	} else {
		if (guiKeyBoardMode.getCapsLock() == true) {
			guiKeyBoardMode.setCapsLock(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::capLock, false, false);
		}
	}
	
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagShift) != 0) {
	#else
		if (([theEvent modifierFlags] & NSShiftKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagShift");
		if (guiKeyBoardMode.getShift() == false) {
			guiKeyBoardMode.setShift(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::shiftLeft, true, false);
		}
	} else {
		if (guiKeyBoardMode.getShift() == true) {
			guiKeyBoardMode.setShift(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::shiftLeft, false, false);
		}
	}
	
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagControl) != 0) {
	#else
		if (([theEvent modifierFlags] & NSControlKeyMask) != 0) {
	#endif
		//GALE_VERBOSE("NSEventModifierFlagControl");
		if (guiKeyBoardMode.getCtrl() == false) {
			GALE_VERBOSE("NSEventModifierFlagControl DOWN");
			guiKeyBoardMode.setCtrl(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::ctrlLeft, true, false);
		}
	} else {
		if (guiKeyBoardMode.getCtrl() == true) {
			GALE_VERBOSE("NSEventModifierFlagControl UP");
			guiKeyBoardMode.setCtrl(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::ctrlLeft, false, false);
		}
	}
	
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagOption) != 0) {
	#else
		if (([theEvent modifierFlags] & NSAlternateKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagOption");
		if (guiKeyBoardMode.getAlt() == false) {
			guiKeyBoardMode.setAlt(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::alt, true, false);
		}
	} else {
		if (guiKeyBoardMode.getAlt() == true) {
			guiKeyBoardMode.setAlt(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::alt, false, false);
		}
	}
	
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagCommand) != 0) {
	#else
		if (([theEvent modifierFlags] & NSCommandKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagCommand");
		if (guiKeyBoardMode.getMeta() == false) {
			guiKeyBoardMode.setMeta(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::metaLeft, true, false);
		}
	} else {
		if (guiKeyBoardMode.getMeta() == true) {
			guiKeyBoardMode.setMeta(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::metaLeft, false, false);
		}
	}
	
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagNumericPad) != 0) {
	#else
		if (([theEvent modifierFlags] & NSNumericPadKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagNumericPad");
		if (guiKeyBoardMode.getNumLock() == false) {
			guiKeyBoardMode.setNumLock(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::numLock, true, false);
		}
	} else {
		if (guiKeyBoardMode.getNumLock() == true) {
			guiKeyBoardMode.setNumLock(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::numLock, false, false);
		}
	}
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagHelp) != 0) {
	#else
		if (([theEvent modifierFlags] & NSHelpKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagHelp");
	}
	#ifdef __MAC_10_12
		if (([theEvent modifierFlags] & NSEventModifierFlagFunction) != 0) {
	#else
		if (([theEvent modifierFlags] & NSFunctionKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagFunction");
		MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::contextMenu, true, false);
		MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::contextMenu, false, false);
	}
	GALE_VERBOSE("EVENT : " << int32_t([theEvent modifierFlags]));
}

// this generate all the event entry availlable ==> like a big keep focus ...
- (BOOL)acceptsFirstResponder {
	return YES;
}
- (BOOL)becomeFirstResponder {
	return YES;
}

-(void)mouseMoved:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("mouseMoved : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(0, point.x, point.y);
}
-(void)mouseEntered:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_INFO("mouseEntered : " << (float)point.x << " " << (float)point.y);
}
-(void)mouseExited:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_INFO("mouseExited : " << (float)point.x << " " << (float)point.y);
}

-(void)mouseDown:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("mouseDown : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(1, true, point.x, point.y);
}
-(void)mouseDragged:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("mouseDragged : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(1, point.x, point.y);
}
-(void)mouseUp:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("mouseUp : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(1, false, point.x, point.y);
}
-(void)rightMouseDown:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("rightMouseDown : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(3, true, point.x, point.y);
}
-(void)rightMouseDragged:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("rightMouseDragged : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(3, point.x, point.y);
}
-(void)rightMouseUp:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("rightMouseUp : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(3, false, point.x, point.y);
}
-(void)otherMouseDown:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	int32_t btNumber = [event buttonNumber];
	switch (btNumber) {
		case 2: // 2 : Middle button
			btNumber = 2;
			break;
		case 3: // 3 : border button DOWN
			btNumber = 8;
			break;
		case 4: // 4 : border button UP
			btNumber = 9;
			break;
		case 5: // 5 : horizontal scroll Right to left
			btNumber = 11;
			break;
		case 6: // 6 : horizontal scroll left to Right
			btNumber = 10;
			break;
		case 7: // 7 : Red button
			btNumber = 12;
			break;
		default:
			btNumber = 15;
			break;
	}
	GALE_VERBOSE("otherMouseDown : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(btNumber, true, point.x, point.y);
}
-(void)otherMouseDragged:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	int32_t btNumber = [event buttonNumber];
	switch (btNumber) {
		case 2: // 2 : Middle button
			btNumber = 2;
			break;
		case 3: // 3 : border button DOWN
			btNumber = 8;
			break;
		case 4: // 4 : border button UP
			btNumber = 9;
			break;
		case 5: // 5 : horizontal scroll Right to left
			btNumber = 11;
			break;
		case 6: // 6 : horizontal scroll left to Right
			btNumber = 10;
			break;
		case 7: // 7 : Red button
			btNumber = 12;
			break;
		default:
			btNumber = 15;
			break;
	}
	GALE_VERBOSE("otherMouseDragged : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(btNumber, point.x, point.y);
}
-(void)otherMouseUp:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	int32_t btNumber = [event buttonNumber];
	GALE_VERBOSE("otherMouseUp: id=" << btNumber );
	switch (btNumber) {
		case 2: // 2 : Middle button
			btNumber = 2;
			break;
		case 3: // 3 : border button DOWN
			btNumber = 8;
			break;
		case 4: // 4 : border button UP
			btNumber = 9;
			break;
		case 5: // 5 : horizontal scroll Right to left
			btNumber = 11;
			break;
		case 6: // 6 : horizontal scroll left to Right
			btNumber = 10;
			break;
		case 7: // 7 : Red button
			btNumber = 12;
			break;
		default:
			btNumber = 15;
			break;
	}
	GALE_VERBOSE("otherMouseUp : " << (float)point.x << " " << (float)point.y << " bt id=" << btNumber );
	MacOs::setMouseState(btNumber, false, point.x, point.y);
}
- (void)scrollWheel:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	GALE_VERBOSE("scrollWheel : " << (float)point.x << " " << (float)point.y << " delta(" << (float)([event deltaX]) << "," << (float)([event deltaY]) << ")");
	float deltaY = [event deltaY];
	int32_t idEvent = 4;
	if (deltaY < 0) {
		idEvent = 5;
	}
	if (fabs(deltaY) < 0.1f) {
		return;
	}
	for (float iii=fabs(deltaY) ; iii>=0.0f ; iii-=1.0f) {
		MacOs::setMouseState(idEvent, true , point.x, point.y);
		MacOs::setMouseState(idEvent, false, point.x, point.y);
	}
}

- (void)closeRequestGale {
	GALE_VERBOSE("closeRequestGale: BEGIN");
	[super close];
	GALE_VERBOSE("closeRequestGale: END");
}

- (void)close {
	GALE_VERBOSE("close:");
	MacOs::stopRequested();
}

- (void)UpdateScreenRequested {
	[_view UpdateScreenRequested];
}
@end



