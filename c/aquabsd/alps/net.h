#if !defined(__AQUA_LIB__AQUABSD_ALPS_NET)
#define __AQUA_LIB__AQUABSD_ALPS_NET

#include <root.h>

static device_t net_device = -1;
typedef uint64_t net_resp_t;

int net_init(void) {
	if (net_device == -1) {
		net_device = query_device("aquabsd.alps.net");
	}

	if (net_device == -1) {
		return -1; // failed to query device
	}

	return 0;
}

net_resp_t net_get_resp(const char* url) {
	if (net_init() < 0) {
		return 0;
	}

	return send_device(net_device, 0x6872, (uint64_t[]) { (uint64_t) url });
}

void net_free_resp(net_resp_t resp) {
	send_device(net_device, 0x6672, (uint64_t[]) { resp });
}

char* net_read_resp(net_resp_t resp) {
	return (char*) send_device(net_device, 0x7272, (uint64_t[]) { resp });
}

#endif