/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <gale/debug.h>

int32_t gale::getLogId() {
	static int32_t g_val = elog::registerInstance("gale");
	return g_val;
}
