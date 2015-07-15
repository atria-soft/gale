/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#import <Cocoa/Cocoa.h>

@interface MacOsAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
- (void)sendEvent:(NSEvent *)event;
- (void)applicationWillResignActive:(MacOsAppDelegate *)application;
@end
