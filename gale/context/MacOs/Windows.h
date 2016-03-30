/** @file
 * @author Edouard DUPIN
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license APACHE v2.0 (see license file)
 */

#import <Cocoa/Cocoa.h>
#import <gale/context/MacOs/OpenglView.h>

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
@end


