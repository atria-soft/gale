/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#import <UIKit/UIKit.h>
#import <gale/context/IOs/AppDelegate.h>
//#import "AppDelegate.h"



int mm_main(int argc, const char *argv[]) {
	@autoreleasepool {
	    return UIApplicationMain(argc, (char**)argv, nil, NSStringFromClass([AppDelegate class]));
	}
	// return no error
	return 0;
}

void mm_exit(void) {
	[[NSThread mainThread] cancel];
}

void mm_openURL(const char *_url) {
	NSString* url = [[NSString alloc] initWithUTF8String:_url];
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
}

