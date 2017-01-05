/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int mm_main(int _argc, const char *_argv[]);
void mm_exit();
void mm_openURL(const char *_url);

#ifdef __cplusplus
}
#endif

