/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <elog/log.h>

namespace gale {
	int32_t getLogId();
};
#define GALE_BASE(info,data) ELOG_BASE(gale::getLogId(),info,data)

#define GALE_PRINT(data)         GALE_BASE(-1, data)
#define GALE_CRITICAL(data)      GALE_BASE(1, data)
#define GALE_ERROR(data)         GALE_BASE(2, data)
#define GALE_WARNING(data)       GALE_BASE(3, data)
#ifdef DEBUG
	#define GALE_INFO(data)          GALE_BASE(4, data)
	#define GALE_DEBUG(data)         GALE_BASE(5, data)
	#define GALE_VERBOSE(data)       GALE_BASE(6, data)
	#define GALE_TODO(data)          GALE_BASE(4, "TODO : " << data)
#else
	#define GALE_INFO(data)          do { } while(false)
	#define GALE_DEBUG(data)         do { } while(false)
	#define GALE_VERBOSE(data)       do { } while(false)
	#define GALE_TODO(data)          do { } while(false)
#endif

#define GALE_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			GALE_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

