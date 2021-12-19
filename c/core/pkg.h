#if !defined(__AQUA_LIB__CORE_PKG)
#define __AQUA_LIB__CORE_PKG

#include <root.h>

static device_t pkg_device = -1;
typedef uint64_t pkg_t;

int pkg_init(void) {
	if (pkg_device == -1) {
		pkg_device = query_device("core.pkg");
	}

	if (pkg_device == -1) { // failed to query device
		return -1;
	}

	return 0;
}

// in AQUA terminology, an "app" is simply an installed application package
// yeah blah blah blah it's not very clear I know shut up 🐦

int pkg_app_count(void) {
	if (pkg_init() < 0) {
		return -1;
	}

	return send_device(pkg_device, 0x6163, NULL);
}

char** pkg_app_list(void) {
	if (pkg_init() < 0) {
		return NULL;
	}

	// client's duty to free this memory and all the blocks it points to 👮
	return (char**) send_device(pkg_device, 0x6C73, NULL);
}

// package information functions

pkg_t pkg_load(const char* path) {
	if (pkg_init() < 0) {
		return 0;
	}

	return send_device(pkg_device, 0x6C70, (uint64_t[]) { (uint64_t) path });
}

void pkg_free(pkg_t pkg) {
	send_device(pkg_device, 0x6670, (uint64_t[]) { pkg });
}

#endif