/**
 * @author Edouard DUPIN
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license APACHE v2.0 (see license file)
 */


#import <ewol/context/MacOs/Windows.h>
#include <ewol/context/MacOS/Context.h>
#include <ewol/key/key.h>

#include <ewol/debug.h>

@implementation EwolMainWindows

+ (id)alloc {
	id windowsID = [super alloc];
	EWOL_DEBUG("ALLOCATE ...");
	return windowsID;
}

- (id)init {
	id windowsID = [super init];
	
	//[NSApp setDelegate: self];
	
	EWOL_DEBUG("INIT ...");
	// set the windows at a specific position :
	[windowsID cascadeTopLeftFromPoint:NSMakePoint(50,50)];
	EWOL_DEBUG("ALLOCATE ...");
	// set the windows resizable
	[windowsID setStyleMask:[windowsID styleMask] | NSResizableWindowMask];
	EWOL_DEBUG("ALLOCATE ...");
	// oposite : [window setStyleMask:[window styleMask] & ~NSResizableWindowMask];
	// set the title
	id appName = [[NSProcessInfo processInfo] processName];
	EWOL_DEBUG("ALLOCATE ...");
	[windowsID setTitle:appName];
	EWOL_DEBUG("ALLOCATE ...");
	
	[windowsID setAcceptsMouseMovedEvents:YES];
	EWOL_DEBUG("ALLOCATE ...");
	// ???
	[windowsID makeKeyAndOrderFront:nil];
	EWOL_DEBUG("ALLOCATE ...");
	[NSApp activateIgnoringOtherApps:YES];
	EWOL_DEBUG("ALLOCATE ...");
	
	NSRect window_frame = [windowsID frame];
	EWOL_DEBUG("ALLOCATE ...");
	
	_view=[[OpenGLView alloc]initWithFrame:window_frame]; //NSMakeRect(0, 0, 800, 600)];
	EWOL_DEBUG("ALLOCATE ...");
	[windowsID setContentView:_view];
	EWOL_DEBUG("ALLOCATE ...");
	[_view setAutoresizesSubviews:YES];
	EWOL_DEBUG("ALLOCATE ...");
	
	// Override point for customization after application launch.
	//[window addSubview:view];
	//[window addChildWindow:view];
	//[window makeKeyAndVisible];
	
	//[windowsID setDelegate:view];
	EWOL_DEBUG("ALLOCATE ...");
	return windowsID;
}


+ (void)dealloc {
	EWOL_ERROR("FREE ...");
	//[_window release];
	[super dealloc];
}

+ (void)performClose:(id)sender {
	EWOL_ERROR("perform close ...");
}

static ewol::key::Special guiKeyBoardMode;


-(void)localKeyEvent:(NSEvent*)theEvent isDown:(bool)_isDown {
	bool thisIsAReapeateKey = false;
	if ([theEvent isARepeat]) {
		thisIsAReapeateKey = true;
	}
	NSString *str = [theEvent charactersIgnoringModifiers];
	// TODO : set if for every char in the string !!!
	unichar c = [str characterAtIndex:0];
	EWOL_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown);
	bool find = true;
	enum ewol::key::keyboard keyInput;
	switch (c) {
		case 63232:	keyInput = ewol::key::keyboardUp;            break;
		case 63233:	keyInput = ewol::key::keyboardDown;          break;
		case 63234:	keyInput = ewol::key::keyboardLeft;          break;
		case 63235:	keyInput = ewol::key::keyboardRight;         break;
		case 63276:	keyInput = ewol::key::keyboardPageUp;        break;
		case 63277:	keyInput = ewol::key::keyboardPageDown;      break;
		case 63273:	keyInput = ewol::key::keyboardStart;         break;
		case 63275:	keyInput = ewol::key::keyboardEnd;           break;
		/*
		case 78:	keyInput = ewol::key::keyboardStopDefil;     break;
		case 127:	keyInput = ewol::key::keyboardWait;          break;
		*/
		case 63302:
			find = false;
			keyInput = ewol::key::keyboardInsert;
			if(_isDown == false) {
				if (true == guiKeyBoardMode.getInsert()) {
					guiKeyBoardMode.setInsert(false);
				} else {
					guiKeyBoardMode.setInsert(true);
				}
			}
			EWOL_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown);
			MacOs::setKeyboardMove(guiKeyBoardMode, keyInput, true, thisIsAReapeateKey);
			EWOL_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << !_isDown);
			MacOs::setKeyboardMove(guiKeyBoardMode, keyInput, false, thisIsAReapeateKey);
			break;
			//case 84:  keyInput = ewol::key::keyboardCenter; break; // Keypad
		case 63236:    keyInput = ewol::key::keyboardF1; break;
		case 63237:    keyInput = ewol::key::keyboardF2; break;
		case 63238:    keyInput = ewol::key::keyboardF3; break;
		case 63239:    keyInput = ewol::key::keyboardF4; break;
		case 63240:    keyInput = ewol::key::keyboardF5; break;
		case 63241:    keyInput = ewol::key::keyboardF6; break;
		case 63242:    keyInput = ewol::key::keyboardF7; break;
		case 63243:    keyInput = ewol::key::keyboardF8; break;
		case 63244:    keyInput = ewol::key::keyboardF9; break;
		case 63245:    keyInput = ewol::key::keyboardF10; break;
		case 63246:    keyInput = ewol::key::keyboardF11; break;
		case 63247:    keyInput = ewol::key::keyboardF12; break;
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
				EWOL_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown);
				MacOs::setKeyboard(guiKeyBoardMode, c, _isDown, thisIsAReapeateKey);
				if (true==thisIsAReapeateKey) {
					MacOs::setKeyboard(guiKeyBoardMode, c, !_isDown, thisIsAReapeateKey);
				}
			}
			break;
	}
	if (find == true) {
		EWOL_VERBOSE("eventKey Move type : " << keyInput );
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
	if (([theEvent modifierFlags] & NSAlphaShiftKeyMask) != 0) {
		EWOL_VERBOSE("NSAlphaShiftKeyMask");
		if (guiKeyBoardMode.getCapsLock() == false) {
			guiKeyBoardMode.setCapsLock(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardCapLock, true, false);
		}
	} else {
		if (guiKeyBoardMode.getCapsLock() == true) {
			guiKeyBoardMode.setCapsLock(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardCapLock, false, false);
		}
	}
	
	if (([theEvent modifierFlags] & NSShiftKeyMask) != 0) {
		EWOL_VERBOSE("NSShiftKeyMask");
		if (guiKeyBoardMode.getShift() == false) {
			guiKeyBoardMode.setShift(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardShiftLeft, true, false);
		}
	} else {
		if (guiKeyBoardMode.getShift() == true) {
			guiKeyBoardMode.setShift(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardShiftLeft, false, false);
		}
	}
	
	if (([theEvent modifierFlags] & NSControlKeyMask) != 0) {
		//EWOL_VERBOSE("NSControlKeyMask");
		if (guiKeyBoardMode.getCtrl() == false) {
			EWOL_VERBOSE("NSControlKeyMask DOWN");
			guiKeyBoardMode.setCtrl(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardCtrlLeft, true, false);
		}
	} else {
		if (guiKeyBoardMode.getCtrl() == true) {
			EWOL_VERBOSE("NSControlKeyMask UP");
			guiKeyBoardMode.setCtrl(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardCtrlLeft, false, false);
		}
	}
	
	if (([theEvent modifierFlags] & NSAlternateKeyMask) != 0) {
		EWOL_VERBOSE("NSAlternateKeyMask");
		if (guiKeyBoardMode.getAlt() == false) {
			guiKeyBoardMode.setAlt(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardAlt, true, false);
		}
	} else {
		if (guiKeyBoardMode.getAlt() == true) {
			guiKeyBoardMode.setAlt(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardAlt, false, false);
		}
	}
	
	if (([theEvent modifierFlags] & NSCommandKeyMask) != 0) {
		EWOL_VERBOSE("NSCommandKeyMask");
		if (guiKeyBoardMode.getMeta() == false) {
			guiKeyBoardMode.setMeta(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardMetaLeft, true, false);
		}
	} else {
		if (guiKeyBoardMode.getMeta() == true) {
			guiKeyBoardMode.setMeta(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardMetaLeft, false, false);
		}
	}
	
	if (([theEvent modifierFlags] & NSNumericPadKeyMask) != 0) {
		EWOL_VERBOSE("NSNumericPadKeyMask");
		if (guiKeyBoardMode.getNumLock() == false) {
			guiKeyBoardMode.setNumLock(true);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardNumLock, true, false);
		}
	} else {
		if (guiKeyBoardMode.getNumLock() == true) {
			guiKeyBoardMode.setNumLock(false);
			MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardNumLock, false, false);
		}
	}
	if (([theEvent modifierFlags] & NSHelpKeyMask) != 0) {
		EWOL_VERBOSE("NSHelpKeyMask");
	}
	if (([theEvent modifierFlags] & NSFunctionKeyMask) != 0) {
		EWOL_VERBOSE("NSFunctionKeyMask");
		MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardContextMenu, true, false);
		MacOs::setKeyboardMove(guiKeyBoardMode, ewol::key::keyboardContextMenu, false, false);
	}
	EWOL_VERBOSE("EVENT : " << int32_t([theEvent modifierFlags]));
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
	EWOL_VERBOSE("mouseMoved : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(0, point.x, point.y);
}
-(void)mouseEntered:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_INFO("mouseEntered : " << (float)point.x << " " << (float)point.y);
}
-(void)mouseExited:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_INFO("mouseExited : " << (float)point.x << " " << (float)point.y);
}

-(void)mouseDown:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_VERBOSE("mouseDown : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(1, true, point.x, point.y);
}
-(void)mouseDragged:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_VERBOSE("mouseDragged : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(1, point.x, point.y);
}
-(void)mouseUp:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_VERBOSE("mouseUp : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(1, false, point.x, point.y);
}
-(void)rightMouseDown:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_VERBOSE("rightMouseDown : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseState(3, true, point.x, point.y);
}
-(void)rightMouseDragged:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_VERBOSE("rightMouseDragged : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(3, point.x, point.y);
}
-(void)rightMouseUp:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_VERBOSE("rightMouseUp : " << (float)point.x << " " << (float)point.y);
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
	EWOL_VERBOSE("otherMouseDown : " << (float)point.x << " " << (float)point.y);
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
	EWOL_VERBOSE("otherMouseDragged : " << (float)point.x << " " << (float)point.y);
	MacOs::setMouseMotion(btNumber, point.x, point.y);
}
-(void)otherMouseUp:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	int32_t btNumber = [event buttonNumber];
	EWOL_VERBOSE("otherMouseUp: id=" << btNumber );
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
	EWOL_VERBOSE("otherMouseUp : " << (float)point.x << " " << (float)point.y << " bt id=" << btNumber );
	MacOs::setMouseState(btNumber, false, point.x, point.y);
}
- (void)scrollWheel:(NSEvent *)event {
	NSPoint point = [event locationInWindow];
	EWOL_VERBOSE("scrollWheel : " << (float)point.x << " " << (float)point.y << " delta(" << (float)([event deltaX]) << "," << (float)([event deltaY]) << ")");
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

- (void)closeRequestEwol {
	EWOL_VERBOSE("closeRequestEwol: BEGIN");
	[super close];
	EWOL_VERBOSE("closeRequestEwol: END");
}

- (void)close {
	EWOL_VERBOSE("close:");
	MacOs::stopRequested();
}

- (void)UpdateScreenRequested {
	[_view UpdateScreenRequested];
}
@end



