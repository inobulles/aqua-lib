// this file is very much analogous to the 'umber.h' header file:
// https://github.com/inobulles/umber

#undef LOG_COMPONENT // force user to define this after header inclusion

#if !defined(__AQUA_LIB__CORE_LOG)
#define __AQUA_LIB__CORE_LOG

#include <stdio.h>

#include "../root.h"
#include <stdarg.h>

typedef enum {
	LOG_LVL_FATAL,
	LOG_LVL_ERROR,
	LOG_LVL_WARN,
	LOG_LVL_SUCCESS,
	LOG_LVL_INFO,
	LOG_LVL_VERBOSE,
} log_lvl_t;

static device_t log_device = -1;

int log_init(void) {
	if (log_device == -1) {
		log_device = query_device("core.log");
	}

	if (log_device == -1) {
		return -1; // failed to query device
	}

	return 0;
}

// this weird name is to not conflict with 'log' in 'math.h'

void __core_log(log_lvl_t const lvl, char const* const component, char const* const path, char const* const func, uint32_t const line, char const* const msg) {
	log_init();
	send_device(log_device, 0x6C67, (uint64_t[]) { lvl, (uint64_t) component, (uint64_t) path, (uint64_t) func, line, (uint64_t) msg });
}

__attribute__((__format__(__printf__, 6, 0)))
void vlog(log_lvl_t const lvl, char const* const component, char const* const path, char const* const func, uint32_t const line, char const* const fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char* msg;
	vasprintf(&msg, fmt, args);

	if (!msg) {
		__core_log(LOG_LVL_FATAL, "__AQUA_LIB__CORE_LOG", __FILE__, __func__, __LINE__, "Failed to allocate space for log message");
		return;
	}

	va_end(args);

	__core_log(lvl, component, path, func, line, msg);
	free(msg);
}

// helper macros

#define LOG_FATAL(...)   vlog(LOG_LVL_FATAL,   LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_ERROR(...)   vlog(LOG_LVL_ERROR,   LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_WARN(...)    vlog(LOG_LVL_WARN,    LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_SUCCESS(...) vlog(LOG_LVL_SUCCESS, LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_INFO(...)    vlog(LOG_LVL_INFO,    LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_VERBOSE(...) vlog(LOG_LVL_VERBOSE, LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);

#endif
