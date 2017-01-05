/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/debug.hpp>

int32_t gale::getLogId() {
	static int32_t g_val = elog::registerInstance("gale");
	return g_val;
}
