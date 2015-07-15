/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */


#import <ewol/context/MacOs/OpenglView.h>
#include <OpenGL/gl.h>
#include <ewol/context/MacOS/Context.h>
#include <ewol/debug.h>
#include <ewol/Dimension.h>


@implementation OpenGLView


- (void) prepareOpenGL {
	EWOL_INFO("prepare");
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	// set system dpi size : 
	NSScreen *screen = [NSScreen mainScreen];
	NSDictionary *description = [screen deviceDescription];
	NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
	CGSize displayPhysicalSize = CGDisplayScreenSize([[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);
	
	ewol::Dimension::setPixelRatio(vec2((float)displayPixelSize.width/(float)displayPhysicalSize.width,
	                                    (float)displayPixelSize.height/(float)displayPhysicalSize.height),
	                                    ewol::Dimension::Millimeter);
	_refreshTimer=[ [ NSTimer scheduledTimerWithTimeInterval:0.017 target:self selector:@selector(animationTimerFired:) userInfo:nil repeats:YES ] retain ] ;
	_redraw = true;
	
}
- (void)UpdateScreenRequested {
	_redraw = true;
}

-(void) drawRect: (NSRect) bounds {
	if ( ! _refreshTimer ) {
		_refreshTimer=[ [ NSTimer scheduledTimerWithTimeInterval:0.017 target:self selector:@selector(animationTimerFired:) userInfo:nil repeats:YES ] retain ] ;
		EWOL_WARNING("create timer ... ");
	}
	MacOs::draw(false);
}

/**
 * Service the animation timer.
 */
- (void) animationTimerFired: (NSTimer *) timer {
	if (_redraw == true) {
		//_redraw = false;
		[self setNeedsDisplay:YES];
		//EWOL_WARNING("view refresh ..." );
	}
}

-(void)reshape {
	EWOL_INFO("view reshape (" << [self frame].size.width << "," << [self frame].size.height << ")" );
	// window resize; width and height are in pixel coordinates
	// but they are floats
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	MacOs::resize(width,height);
}

@end

