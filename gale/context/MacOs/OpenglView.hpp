/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>


@interface OpenGLView : NSOpenGLView<NSWindowDelegate> {
    NSTimer* _refreshTimer;
    bool _redraw;
}
- (void)prepareOpenGL;
- (void)drawRect:(NSRect) bounds;
- (void)UpdateScreenRequested;
@end
