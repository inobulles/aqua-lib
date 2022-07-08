#if !defined(__AQUA_LIB__CORE_FS)
#define __AQUA_LIB__CORE_FS

#include <root.h>

typedef uint64_t fs_descr_t;

typedef enum {
	FS_FLAGS_READ   = 0b00001,
	FS_FLAGS_WRITE  = 0b00010,
	FS_FLAGS_EXEC   = 0b00100,
	FS_FLAGS_APPEND = 0b01000,
	FS_FLAGS_CREATE = 0b10000,
} fs_flags_t;

typedef enum {
	FS_ERR_SUCCESS = 0,
	FS_ERR_GENERIC = -1,
} fs_err_t;

static device_t fs_device = -1;

int fs_init(void) {
	if (fs_device == -1) {
		fs_device = query_device("core.fs");
	}

	if (fs_device == -1) {
		return -1; // failed to query device
	}

	return 0;
}

// command wrapper functions

fs_descr_t fs_open(const char* drive, const char* path, fs_flags_t flags) {
	if (fs_init() < 0) {
		return 0;
	}

	return (fs_descr_t) send_device(fs_device, 0x6F70, (uint64_t[]) { (uint64_t) drive, (uint64_t) path, flags });
}

fs_err_t fs_close(fs_descr_t descr) {
	return send_device(fs_device, 0x636C, (uint64_t[]) { descr });
}

uint64_t fs_size(fs_descr_t descr) {
	return send_device(fs_device, 0x737A, (uint64_t[]) { descr });
}

void* fs_mmap(fs_descr_t descr) {
	return (void*) send_device(fs_device, 0x6D6D, (uint64_t[]) { descr });
}

fs_err_t fs_read(fs_descr_t descr, void* buf, size_t len) {
	return send_device(fs_device, 0x7264, (uint64_t[]) { descr, (uint64_t) buf, len });
}

fs_err_t fs_write(fs_descr_t descr, const void* buf, size_t len) {
	return send_device(fs_device, 0x7772, (uint64_t[]) { descr, (uint64_t) buf, len });
}

// helper functions

fs_err_t fs_write_str(fs_descr_t descr, const char* str) {
	return fs_write(descr, str, strlen(str));
}

#endif
