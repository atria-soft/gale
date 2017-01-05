/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#import <UIKit/UIKit.h>
@class OpenglView;

@interface AppDelegate : UIResponder <UIApplicationDelegate> {
    UIWindow *window;
    OpenglView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet OpenglView *glView;

@end
