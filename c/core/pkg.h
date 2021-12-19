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

// package manipulation functions

pkg_t pkg_load(const char* path) {
	if (pkg_init() < 0) {
		return 0;
	}

	return send_device(pkg_device, 0x6C70, (uint64_t[]) { (uint64_t) path });
}

void pkg_free(pkg_t pkg) {
	send_device(pkg_device, 0x6670, (uint64_t[]) { pkg });
}

#define PKG_GEN_INFO_FUNC(key, cmd) \
	char* pkg_##key(pkg_t pkg) { \
		return (char*) send_device(pkg_device, (cmd), (uint64_t[]) { pkg }); \
	}

PKG_GEN_INFO_FUNC(name,   0x676E)
PKG_GEN_INFO_FUNC(unique, 0x6775)

char* pkg_generic(pkg_t pkg, const char* key) {
	return (char*) send_device(pkg_device, 0x6767, (uint64_t[]) { pkg, (uint64_t) key });
}

int pkg_boot(pkg_t pkg) {
	return send_device(pkg_device, 0x626F, (uint64_t[]) { pkg });
}

#endif