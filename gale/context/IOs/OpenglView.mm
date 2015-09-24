//
//  EAGLView.m
//  OpenGLBasics
//
//  Created by Charlie Key on 6/24/09.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>
#include <gale/context/IOs/Context.h>
#include <gale/Dimension.h>


#include <sys/types.h>
#include <sys/sysctl.h>

#import "OpenglView.h"
#include <gale/debug.h>


@interface OpenglView ()
@end


@implementation OpenglView

// You must implement this method (it does not work without it)
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (NSString *) platform {
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = (char*)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithUTF8String:machine];
    free(machine);
    return platform;
}

- (NSString *) platformString {
	NSString *platform = [self platform];
	if ([platform isEqualToString:@"iPhone1,1"])    return @"iPhone 1G";
	if ([platform isEqualToString:@"iPhone1,2"])    return @"iPhone 3G";
	if ([platform isEqualToString:@"iPhone2,1"])    return @"iPhone 3GS";
	if ([platform isEqualToString:@"iPhone3,1"])    return @"iPhone 4";
	if ([platform isEqualToString:@"iPhone3,3"])    return @"Verizon iPhone 4";
	if ([platform isEqualToString:@"iPhone4,1"])    return @"iPhone 4S";
	if ([platform isEqualToString:@"iPhone5,1"])    return @"iPhone 5 (GSM)";
	if ([platform isEqualToString:@"iPhone5,2"])    return @"iPhone 5 (GSM+CDMA)";
	if ([platform isEqualToString:@"iPhone5,3"])    return @"iPhone 5c (GSM)";
	if ([platform isEqualToString:@"iPhone5,4"])    return @"iPhone 5c (GSM+CDMA)";
	if ([platform isEqualToString:@"iPhone6,1"])    return @"iPhone 5s (GSM)";
	if ([platform isEqualToString:@"iPhone6,2"])    return @"iPhone 5s (GSM+CDMA)";
	if ([platform isEqualToString:@"iPod1,1"])      return @"iPod Touch 1G";
	if ([platform isEqualToString:@"iPod2,1"])      return @"iPod Touch 2G";
	if ([platform isEqualToString:@"iPod3,1"])      return @"iPod Touch 3G";
	if ([platform isEqualToString:@"iPod4,1"])      return @"iPod Touch 4G";
	if ([platform isEqualToString:@"iPod5,1"])      return @"iPod Touch 5G";
	if ([platform isEqualToString:@"iPad1,1"])      return @"iPad";
	if ([platform isEqualToString:@"iPad2,1"])      return @"iPad 2 (WiFi)";
	if ([platform isEqualToString:@"iPad2,2"])      return @"iPad 2 (GSM)";
	if ([platform isEqualToString:@"iPad2,3"])      return @"iPad 2 (CDMA)";
	if ([platform isEqualToString:@"iPad2,4"])      return @"iPad 2 (WiFi)";
	if ([platform isEqualToString:@"iPad2,5"])      return @"iPad Mini (WiFi)";
	if ([platform isEqualToString:@"iPad2,6"])      return @"iPad Mini (GSM)";
	if ([platform isEqualToString:@"iPad2,7"])      return @"iPad Mini (GSM+CDMA)";
	if ([platform isEqualToString:@"iPad3,1"])      return @"iPad 3 (WiFi)";
	if ([platform isEqualToString:@"iPad3,2"])      return @"iPad 3 (GSM+CDMA)";
	if ([platform isEqualToString:@"iPad3,3"])      return @"iPad 3 (GSM)";
	if ([platform isEqualToString:@"iPad3,4"])      return @"iPad 4 (WiFi)";
	if ([platform isEqualToString:@"iPad3,5"])      return @"iPad 4 (GSM)";
	if ([platform isEqualToString:@"iPad3,6"])      return @"iPad 4 (GSM+CDMA)";
	if ([platform isEqualToString:@"iPad4,1"])      return @"iPad Air (WiFi)";
	if ([platform isEqualToString:@"iPad4,2"])      return @"iPad Air (Cellular)";
	if ([platform isEqualToString:@"iPad4,4"])      return @"iPad mini 2G (WiFi)";
	if ([platform isEqualToString:@"iPad4,5"])      return @"iPad mini 2G (Cellular)";
	if ([platform isEqualToString:@"i386"])         return @"Simulator";
	if ([platform isEqualToString:@"x86_64"])       return @"Simulator";
	return platform;
}
- (float)getScreenPPP {
	NSString *platform = [self platform];
	if ([platform isEqualToString:@"iPhone1,1"])    return 326.0f; //@"iPhone 1G";
	if ([platform isEqualToString:@"iPhone1,2"])    return 326.0f; //@"iPhone 3G";
	if ([platform isEqualToString:@"iPhone2,1"])    return 326.0f; //@"iPhone 3GS";
	if ([platform isEqualToString:@"iPhone3,1"])    return 326.0f; //@"iPhone 4";
	if ([platform isEqualToString:@"iPhone3,3"])    return 326.0f; //@"Verizon iPhone 4";
	if ([platform isEqualToString:@"iPhone4,1"])    return 326.0f; //@"iPhone 4S";
	if ([platform isEqualToString:@"iPhone5,1"])    return 326.0f; //@"iPhone 5 (GSM)";
	if ([platform isEqualToString:@"iPhone5,2"])    return 326.0f; //@"iPhone 5 (GSM+CDMA)";
	if ([platform isEqualToString:@"iPhone5,3"])    return 326.0f; //@"iPhone 5c (GSM)";
	if ([platform isEqualToString:@"iPhone5,4"])    return 326.0f; //@"iPhone 5c (GSM+CDMA)";
	if ([platform isEqualToString:@"iPhone6,1"])    return 326.0f; //@"iPhone 5s (GSM)";
	if ([platform isEqualToString:@"iPhone6,2"])    return 326.0f; //@"iPhone 5s (GSM+CDMA)";
	if ([platform isEqualToString:@"iPod1,1"])      return 326.0f; //@"iPod Touch 1G";
	if ([platform isEqualToString:@"iPod2,1"])      return 326.0f; //@"iPod Touch 2G";
	if ([platform isEqualToString:@"iPod3,1"])      return 326.0f; //@"iPod Touch 3G";
	if ([platform isEqualToString:@"iPod4,1"])      return 326.0f; //@"iPod Touch 4G";
	if ([platform isEqualToString:@"iPod5,1"])      return 326.0f; //@"iPod Touch 5G";
	if ([platform isEqualToString:@"iPad1,1"])      return 264.0f; //@"iPad";
	if ([platform isEqualToString:@"iPad2,1"])      return 264.0f; //@"iPad 2 (WiFi)";
	if ([platform isEqualToString:@"iPad2,2"])      return 264.0f; //@"iPad 2 (GSM)";
	if ([platform isEqualToString:@"iPad2,3"])      return 264.0f; //@"iPad 2 (CDMA)";
	if ([platform isEqualToString:@"iPad2,4"])      return 264.0f; //@"iPad 2 (WiFi)";
	if ([platform isEqualToString:@"iPad2,5"])      return 163.0f; //@"iPad Mini (WiFi)";
	if ([platform isEqualToString:@"iPad2,6"])      return 163.0f; //@"iPad Mini (GSM)";
	if ([platform isEqualToString:@"iPad2,7"])      return 163.0f; //@"iPad Mini (GSM+CDMA)";
	if ([platform isEqualToString:@"iPad3,1"])      return 264.0f; //@"iPad 3 (WiFi)";
	if ([platform isEqualToString:@"iPad3,2"])      return 264.0f; //@"iPad 3 (GSM+CDMA)";
	if ([platform isEqualToString:@"iPad3,3"])      return 264.0f; //@"iPad 3 (GSM)";
	if ([platform isEqualToString:@"iPad3,4"])      return 264.0f; //@"iPad 4 (WiFi)";
	if ([platform isEqualToString:@"iPad3,5"])      return 264.0f; //@"iPad 4 (GSM)";
	if ([platform isEqualToString:@"iPad3,6"])      return 264.0f; //@"iPad 4 (GSM+CDMA)";
	if ([platform isEqualToString:@"iPad4,1"])      return 264.0f; //@"iPad Air (WiFi)";
	if ([platform isEqualToString:@"iPad4,2"])      return 264.0f; //@"iPad Air (Cellular)";
	if ([platform isEqualToString:@"iPad4,4"])      return 326.0f; //@"iPad mini 2G (WiFi)";
	if ([platform isEqualToString:@"iPad4,5"])      return 326.0f; //@"iPad mini 2G (Cellular)";
	if ([platform isEqualToString:@"i386"])         return 200.0f; //@"Simulator";
	if ([platform isEqualToString:@"x86_64"])       return 326.0f; //@"Simulator";
	return 326.0f;
}

- (void)setNeedsDisplay {
	GALE_INFO("**** setNeedsDisplay:" << vec2(self.frame.size.width, self.frame.size.height));
	// TODO : SIZE change ...

}
- (void)configureAspectRatio {
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
	CGFloat screenScale = [[UIScreen mainScreen] scale];
	m_currentSize = CGSizeMake(screenBounds.size.width * screenScale, screenBounds.size.height * screenScale);
	//self.frame.size = m_currentSize;
	
	GALE_INFO("**** screen size:" << vec2(m_currentSize.width, m_currentSize.height));
	float ratio = [self getScreenPPP];
	GALE_INFO("**** pixel ratio: " << ratio);
	gale::Dimension::setPixelRatio(vec2(1.0f/ratio, 1.0f/ratio), gale::Dimension::Inch);
	IOs::resize(m_currentSize.width, m_currentSize.height);
	CGRect localBounds = [self bounds];
	GALE_INFO("**** localBounds:" << vec2(localBounds.size.width, localBounds.size.height));

}
- (void)setupLayer {
    _eaglLayer = (CAEAGLLayer*) self.layer;
	CGFloat screenScale = [[UIScreen mainScreen] scale];
	_eaglLayer.contentsScale = screenScale;
    _eaglLayer.opaque = YES;
}

- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context) {
        NSLog(@"Failed to initialize OpenGLES 2.0 context");
        exit(1);
    }
    if (![EAGLContext setCurrentContext:_context]) {
        NSLog(@"Failed to set current OpenGL context");
        exit(1);
    }
}


- (void)setupRenderBuffer {
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}

- (void)setupDepthBuffer {
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
	CGFloat screenScale = [[UIScreen mainScreen] scale];
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, self.frame.size.width*screenScale, self.frame.size.height*screenScale);
}

- (void)setupFrameBuffer {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
}

- (void)render:(CADisplayLink*)displayLink {
	displayCounter++;
	if (displayCounter >= deltaDisplay) {
		displayCounter = 0;
		IOs::draw(true);
		[_context presentRenderbuffer:GL_RENDERBUFFER];
	}
}

- (void)speedSlow {
	deltaDisplay = 5;
}

- (void)speedNormal {
	deltaDisplay = 1;
}

- (void)startDisplayLink {
	if (displayLink != NULL) {
		return;
	}
	displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)stopDisplayLink {
	if (displayLink == NULL) {
		return;
	}
    [displayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	displayLink = NULL;
}

- (id)initWithFrame:(CGRect)frame {
	deltaDisplay = 1;
	displayCounter = 0;
	NSLog(@"INIT with size : %fx%f, %fx%f", frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
    self = [super initWithFrame:frame];
    self.contentScaleFactor = 1.0f;
	// TODO : Enable multi touch ==> but this generate many sub errors ... 3 touch can be appear in 1 event ...
	//self.multipleTouchEnabled = YES;
	if (self) {
		[self configureAspectRatio];
        [self setupLayer];
        [self setupContext];
        [self setupDepthBuffer];
        [self setupRenderBuffer];
        [self setupFrameBuffer];
		[self startDisplayLink];
    }
    return self;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint touchLocation = [touch locationInView:self];
	CGRect localBounds = [self bounds];
	CGFloat screenScale = [[UIScreen mainScreen] scale];
	vec2 positionEvent(touchLocation.x*screenScale, (localBounds.size.height - touchLocation.y)*screenScale);
	GALE_DEBUG(touches.count << " touchesBegan: " << positionEvent);
	IOs::setInputState(1, true, positionEvent.x(), positionEvent.y());
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint touchLocation = [touch locationInView:self];
	CGRect localBounds = [self bounds];
	CGFloat screenScale = [[UIScreen mainScreen] scale];
	vec2 positionEvent(touchLocation.x*screenScale, (localBounds.size.height - touchLocation.y)*screenScale);
	GALE_DEBUG(touches.count << " touchesEnded: " << positionEvent);
	IOs::setInputState(1, false, positionEvent.x(), positionEvent.y());
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
	UITouch *touch = [touches anyObject];
	CGPoint touchLocation = [touch locationInView:self];
	CGRect localBounds = [self bounds];
	CGFloat screenScale = [[UIScreen mainScreen] scale];
	vec2 positionEvent(touchLocation.x*screenScale, (localBounds.size.height - touchLocation.y)*screenScale);
	GALE_DEBUG(touches.count << " touchesMoved: " << positionEvent);
	IOs::setInputMotion(1, positionEvent.x(), positionEvent.y());
}

- (void)layoutSubviews {
    [EAGLContext setCurrentContext:_context];
}

- (void)dealloc {
    if ([EAGLContext currentContext] == _context) {
        [EAGLContext setCurrentContext:nil];
    }
}


@end
