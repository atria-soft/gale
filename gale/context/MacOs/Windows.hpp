/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#import <Cocoa/Cocoa.h>
#import <gale/context/MacOs/OpenglView.hpp>
#include <gale/key/key.hpp>

@interface GaleMainWindows : NSWindow {
	OpenGLView* _view;
}
+ (id)alloc;
- (id)init;
+ (void)dealloc;
+ (void)performClose:(id)sender;
// All mouse events:
- (void)mouseDown:(NSEvent *) event;
- (void)mouseDragged:(NSEvent *) event;
- (void)mouseUp:(NSEvent *)event;
- (void)mouseMoved:(NSEvent *)event;
- (void)mouseEntered:(NSEvent *)event;
- (void)mouseExited:(NSEvent *)event;
- (void)rightMouseDown:(NSEvent *)event;
- (void)rightMouseDragged:(NSEvent *)event;
- (void)rightMouseUp:(NSEvent *)event;
- (void)otherMouseDown:(NSEvent *)event;
- (void)otherMouseDragged:(NSEvent *)event;
- (void)otherMouseUp:(NSEvent *)event;
// keyboard eevnts:
- (void)keyDown:(NSEvent *)theEvent;
- (void)flagsChanged:(NSEvent *)theEvent;
- (void)closeRequestGale;
- (void)UpdateScreenRequested;
/**
 * @brief Check if we have the need to change the internal flag of shift, ctrl ...
 * @param[in] bitField MacOs bit field
 * @param[in] flags Patern to check
 * @param[in] key Gale key
 */
- (void)flagsUpdate:(uint32_t) bitField
                   :(uint32_t) flags
                   :(enum gale::key::keyboard) key;
@end


