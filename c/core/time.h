#if !defined(__AQUA_LIB__CORE_TIME)
#define __AQUA_LIB__CORE_TIME

#include <root.h>

static device_t time_device = -1;

uint64_t get_unix_time(void) {
	if (time_device == -1) {
		time_device = query_device("core.time");
	}

	return send_device(time_device, 0x7578, NULL);
}

#endif
