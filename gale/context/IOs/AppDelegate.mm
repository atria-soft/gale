/**
 * @author Edouard DUPIN
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license APACHE v2.0 (see license file)
 */

#import <UIKit/UIKit.h>
#include "ewol/context/IOs/Interface.h"

#import <ewol/context/IOs/OpenglView.h>
#import <ewol/context/IOs/AppDelegate.h>
#include <ewol/context/IOs/Context.h>
#include <ewol/debug.h>

@implementation AppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	CGRect screenBounds = [[UIScreen mainScreen] bounds];
	CGFloat screenScale = [[UIScreen mainScreen] scale];
	NSLog(@"Start with screeen bounds : %fx%f\n", screenBounds.size.width, screenBounds.size.height);
	CGSize currentSize = CGSizeMake(screenBounds.size.width * screenScale, screenBounds.size.height * screenScale);
	//screenBounds.size.width *= screenScale;
	//screenBounds.size.height *= screenScale;
	NSLog(@"Start with screeen bounds : %fx%f\n", screenBounds.size.width, screenBounds.size.height);
	window = [[UIWindow alloc] initWithFrame:screenBounds];
	window.contentMode = UIViewContentModeRedraw;
	glView = [[OpenglView alloc] initWithFrame:window.bounds];
	glView.contentMode = UIViewContentModeRedraw;
	[window addSubview:glView];
	[window makeKeyAndVisible];
	// Create interface of ewol here ....
	NSLog(@"CREATE EWOL interface creation\n");
	IOs::createInterface();
	IOs::resize(currentSize.width, currentSize.height);
	IOs::start();
}
/*
 - (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
 {
 // Override point for customization after application launch.
 return YES;
 }
 */

- (void)applicationWillResignActive:(UIApplication *)application {
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	EWOL_INFO("move windows in applicationWillResignActive");
	[glView speedSlow];
	IOs::background();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
	EWOL_INFO("move windows in applicationDidEnterBackground");
	[glView stopDisplayLink];
	IOs::suspend();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
	EWOL_INFO("move windows in applicationWillEnterForeground");
	IOs::resume();
	[glView startDisplayLink];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	EWOL_INFO("move windows in applicationDidBecomeActive");
	[glView speedNormal];
	IOs::foreground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
	// Create interface of ewol here ....
	EWOL_INFO("move windows in applicationWillTerminate");
	IOs::stop();
	IOs::releaseInterface();
}


/*
 - (void)dealloc {
 [window release];
 [glView release];
 [super dealloc];
 }
 */

@end
