/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#import <gale/context/MacOs/Windows.hpp>
#include <gale/context/MacOs/Context.hpp>
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
	// set the windows resizable
	#ifdef __MAC_10_12
		[windowsID setStyleMask:[windowsID styleMask] | NSWindowStyleMaskResizable];
	#else
		[windowsID setStyleMask:[windowsID styleMask] | NSResizableWindowMask];
	#endif
	// oposite : [window setStyleMask:[window styleMask] & ~NSResizableWindowMask];
	// set the title
	id appName = [[NSProcessInfo processInfo] processName];
	[windowsID setTitle:appName];
	
	[windowsID setAcceptsMouseMovedEvents:YES];
	// ???
	[windowsID makeKeyAndOrderFront:nil];
	[NSApp activateIgnoringOtherApps:YES];
	
	NSRect window_frame = [windowsID frame];
	
	_view=[[OpenGLView alloc]initWithFrame:window_frame]; //NSMakeRect(0, 0, 800, 600)];
	[windowsID setContentView:_view];
	[_view setAutoresizesSubviews:YES];
	
	// Override point for customization after application launch.
	//[window addSubview:view];
	//[window addChildWindow:view];
	//[window makeKeyAndVisible];
	
	//[windowsID setDelegate:view];
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

static std::vector<std::pair<uint16_t,unichar>> g_listlasteventDown;

-(void)localKeyEvent:(NSEvent*)theEvent isDown:(bool)_isDown {
	bool thisIsAReapeateKey = false;
	if ([theEvent isARepeat]) {
		thisIsAReapeateKey = true;
	}
	//[self flagsChanged:theEvent];
	NSString *str = [theEvent charactersIgnoringModifiers];
	// TODO : set if for every char in the string !!!
	unichar c = [str characterAtIndex:0];
	uint16_t keycode = [theEvent keyCode];
	// special case for \t + shift:
	if (    guiKeyBoardMode.getShift() == true
	     && c == 25) {
		// We remap it to the correct tabulation.
		c = 9;
	}
	GALE_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown << " keycode=" << keycode);
	bool find = true;
	enum gale::key::keyboard keyInput;
	switch (c) {
		case NSUpArrowFunctionKey:     keyInput = gale::key::keyboard::up;            break;
		case NSDownArrowFunctionKey:   keyInput = gale::key::keyboard::down;          break;
		case NSLeftArrowFunctionKey:   keyInput = gale::key::keyboard::left;          break;
		case NSRightArrowFunctionKey:  keyInput = gale::key::keyboard::right;         break;
		case NSPageUpFunctionKey:      keyInput = gale::key::keyboard::pageUp;        break;
		case NSPageDownFunctionKey:    keyInput = gale::key::keyboard::pageDown;      break;
		case NSBeginFunctionKey:
		case NSHomeFunctionKey:        keyInput = gale::key::keyboard::start;         break;
		case NSEndFunctionKey:         keyInput = gale::key::keyboard::end;           break;
		case NSScrollLockFunctionKey:  keyInput = gale::key::keyboard::stopDefil;     break;
		case NSPauseFunctionKey:       keyInput = gale::key::keyboard::wait;          break;
		case NSPrintScreenFunctionKey: keyInput = gale::key::keyboard::print;         break;
		case 16:                       keyInput = gale::key::keyboard::contextMenu;   break;
		
		case 63302:
		case NSInsertFunctionKey:
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
		case NSF1FunctionKey:    keyInput = gale::key::keyboard::f1; break;
		case NSF2FunctionKey:    keyInput = gale::key::keyboard::f2; break;
		case NSF3FunctionKey:    keyInput = gale::key::keyboard::f3; break;
		case NSF4FunctionKey:    keyInput = gale::key::keyboard::f4; break;
		case NSF5FunctionKey:    keyInput = gale::key::keyboard::f5; break;
		case NSF6FunctionKey:    keyInput = gale::key::keyboard::f6; break;
		case NSF7FunctionKey:    keyInput = gale::key::keyboard::f7; break;
		case NSF8FunctionKey:    keyInput = gale::key::keyboard::f8; break;
		case NSF9FunctionKey:    keyInput = gale::key::keyboard::f9; break;
		case NSF10FunctionKey:    keyInput = gale::key::keyboard::f10; break;
		case NSF11FunctionKey:    keyInput = gale::key::keyboard::f11; break;
		case NSF12FunctionKey:    keyInput = gale::key::keyboard::f12; break;
		case NSF13FunctionKey:
		case NSF14FunctionKey:
		case NSF15FunctionKey:
		case NSF16FunctionKey:
		case NSF17FunctionKey:
		case NSF18FunctionKey:
		case NSF19FunctionKey:
		case NSF20FunctionKey:
		case NSF21FunctionKey:
		case NSF22FunctionKey:
		case NSF23FunctionKey:
		case NSF24FunctionKey:
		case NSF25FunctionKey:
		case NSF26FunctionKey:
		case NSF27FunctionKey:
		case NSF28FunctionKey:
		case NSF29FunctionKey:
		case NSF30FunctionKey:
		case NSF31FunctionKey:
		case NSF32FunctionKey:
		case NSF33FunctionKey:
		case NSF34FunctionKey:
		case NSF35FunctionKey:
			find = false;
			break;
		case NSDeleteFunctionKey: // Suppress
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
					GALE_VERBOSE("Key Event ALT " << c << "  = '" << char(c) << "' isDown=" << _isDown << " nb in list=" << g_listlasteventDown.size());
				} else {
					GALE_VERBOSE("Key Event     " << c << "  = '" << char(c) << "' isDown=" << _isDown);
				}
				// MacOs have a problem of synchronizing the correct key with the modifier...
				if (_isDown == false) {
					for (auto it = g_listlasteventDown.begin();
					     it != g_listlasteventDown.end();
					     ++it) {
						if (it->first == keycode) {
							c = it->second;
							g_listlasteventDown.erase(it);
							GALE_VERBOSE("Key Event " << c << "  = '" << char(c) << "' isDown=" << _isDown << " (override)");
							break;
						}
					}
				} else {
					// remove a previous occurence of this element (case where Macos does not send the UP)...
					for (auto it = g_listlasteventDown.begin();
					     it != g_listlasteventDown.end();
					     ++it) {
						if (it->first == keycode) {
							g_listlasteventDown.erase(it);
							break;
						}
					}
					g_listlasteventDown.push_back(std::make_pair(keycode, c));
				}
				MacOs::setKeyboard(guiKeyBoardMode, c, _isDown, thisIsAReapeateKey);
				if (thisIsAReapeateKey == true) {
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

- (void)flagsUpdate:(uint32_t) bitField
                   :(uint32_t) flags
                   :(enum gale::key::keyboard) key {
	if ((bitField & flags) != 0) {
		GALE_VERBOSE("Flag change: " << key);
		if (guiKeyBoardMode.get(key) == false) {
			GALE_DEBUG("    " << key << " DOWN");
			guiKeyBoardMode.update(key, true);
			MacOs::setKeyboardMove(guiKeyBoardMode, key, true, false);
		}
	} else {
		if (guiKeyBoardMode.get(key) == true) {
			GALE_DEBUG("    " << key << " UP");
			guiKeyBoardMode.update(key, false);
			MacOs::setKeyboardMove(guiKeyBoardMode, key, false, false);
		}
	}
}

- (void)flagsChanged:(NSEvent *)theEvent {
	uint32_t bitField = [theEvent modifierFlags];
	GALE_VERBOSE("flagsChanged : " << std::hex << [theEvent modifierFlags]);
	#ifdef __MAC_10_12
		[self flagsUpdate:bitField: NSEventModifierFlagCapsLock: gale::key::keyboard::capLock];
	#else
		[self flagsUpdate:bitField: NSAlphaShiftKeyMask: gale::key::keyboard::capLock];
	#endif
	#ifdef __MAC_10_12
		//[self flagsUpdate:bitField: NSEventModifierFlagShift: gale::key::keyboard::shiftLeft];
		[self flagsUpdate:bitField: 0x02: gale::key::keyboard::shiftLeft];
		[self flagsUpdate:bitField: 0x04: gale::key::keyboard::shiftRight];
	#else
		[self flagsUpdate:bitField: NSShiftKeyMask: gale::key::keyboard::shiftLeft];
	#endif
	#ifdef __MAC_10_12
		//[self flagsUpdate:bitField: NSEventModifierFlagControl: gale::key::keyboard::ctrlLeft];
		[self flagsUpdate:bitField: 0x0001: gale::key::keyboard::ctrlLeft];
		[self flagsUpdate:bitField: 0x2000: gale::key::keyboard::ctrlRight];
	#else
		[self flagsUpdate:bitField: NSControlKeyMask: gale::key::keyboard::ctrlLeft];
	#endif
	#ifdef __MAC_10_12
		//[self flagsUpdate:bitField: NSEventModifierFlagOption: gale::key::keyboard::altLeft];
		[self flagsUpdate:bitField: 0x0020: gale::key::keyboard::altLeft];
		[self flagsUpdate:bitField: 0x0040: gale::key::keyboard::altRight];
	#else
		[self flagsUpdate:bitField: NSAlternateKeyMask: gale::key::keyboard::altLeft];
	#endif
	#ifdef __MAC_10_12
		//[self flagsUpdate:bitField: NSEventModifierFlagCommand: gale::key::keyboard::metaLeft];
		[self flagsUpdate:bitField: 0x0008: gale::key::keyboard::metaLeft];
		[self flagsUpdate:bitField: 0x0010: gale::key::keyboard::metaRight];
	#else
		[self flagsUpdate:bitField: NSCommandKeyMask: gale::key::keyboard::metaLeft];
	#endif
	#ifdef __MAC_10_12
		[self flagsUpdate:bitField: NSEventModifierFlagNumericPad: gale::key::keyboard::numLock];
	#else
		[self flagsUpdate:bitField: NSNumericPadKeyMask: gale::key::keyboard::numLock];
	#endif
	#ifdef __MAC_10_12
		if ((bitField & NSEventModifierFlagFunction) != 0) {
	#else
		if ((bitField & NSFunctionKeyMask) != 0) {
	#endif
		GALE_VERBOSE("NSEventModifierFlagFunction");
		MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::contextMenu, true, false);
		MacOs::setKeyboardMove(guiKeyBoardMode, gale::key::keyboard::contextMenu, false, false);
	}
	GALE_VERBOSE("        ==> new state special: " << etk::to_string(guiKeyBoardMode));
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



