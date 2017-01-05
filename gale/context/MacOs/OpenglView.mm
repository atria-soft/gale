/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#import <gale/context/MacOs/OpenglView.hpp>
#include <OpenGL/gl.h>
#include <gale/context/MacOS/Context.hpp>
#include <gale/debug.hpp>
#include <gale/Dimension.hpp>


@implementation OpenGLView


- (void) prepareOpenGL {
	GALE_INFO("prepare");
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	// set system dpi size : 
	NSScreen *screen = [NSScreen mainScreen];
	NSDictionary *description = [screen deviceDescription];
	NSSize displayPixelSize = [[description objectForKey:NSDeviceSize] sizeValue];
	CGSize displayPhysicalSize = CGDisplayScreenSize([[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);
	
	gale::Dimension::setPixelRatio(vec2((float)displayPixelSize.width/(float)displayPhysicalSize.width,
	                                    (float)displayPixelSize.height/(float)displayPhysicalSize.height),
	                                    gale::distance::millimeter);
	_refreshTimer=[ [ NSTimer scheduledTimerWithTimeInterval:0.017 target:self selector:@selector(animationTimerFired:) userInfo:nil repeats:YES ] retain ] ;
	_redraw = true;
	
}
- (void)UpdateScreenRequested {
	_redraw = true;
}

-(void) drawRect: (NSRect) bounds {
	if ( ! _refreshTimer ) {
		_refreshTimer=[ [ NSTimer scheduledTimerWithTimeInterval:0.017 target:self selector:@selector(animationTimerFired:) userInfo:nil repeats:YES ] retain ] ;
		GALE_WARNING("create timer ... ");
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
		//GALE_WARNING("view refresh ..." );
	}
}

-(void)reshape {
	GALE_INFO("view reshape (" << [self frame].size.width << "," << [self frame].size.height << ")" );
	// window resize; width and height are in pixel coordinates
	// but they are floats
	float width = [self frame].size.width;
	float height = [self frame].size.height;
	MacOs::resize(width,height);
}

@end

