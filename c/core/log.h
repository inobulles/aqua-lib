#if !defined(__AQUA_LIB__CORE_LOG)
#define __AQUA_LIB__CORE_LOG

#include "../root.h"

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

void fs_write(fs_descr_t descr, const void* buf, size_t len) {
	log_init();
	return (fs_err_t) send_device(fs_device, 0x7772, (uint64_t[]) { descr, (uint64_t) buf, len });
}

#endif
