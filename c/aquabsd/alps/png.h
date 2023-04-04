#if !defined(__AQUA_LIB__AQUABSD_ALPS_PNG)
#define __AQUA_LIB__AQUABSD_ALPS_PNG

#include "../../root.h"
#include "../../core/fs.h"

// TODO error handling

static device_t png_device = -1;
typedef uint64_t png_t;

png_t png_load_ptr(void* ptr) {
	if (png_device == -1) {
		png_device = query_device("aquabsd.alps.png");
	}

	return send_device(png_device, 0x6C64, (uint64_t[]) { (uint64_t) ptr });
}

png_t png_load(const char* drive, const char* path) {
	fs_descr_t file = fs_open(drive, path, FS_FLAGS_READ);
	png_t png = png_load_ptr(fs_mmap(file));

	// fs_close(file);
	return png;
}

int png_draw(png_t png, uint8_t** bitmap_reference, uint64_t* bpp_reference, uint64_t* width_reference, uint64_t* height_reference) {
	return (int) send_device(png_device, 0x6477, (uint64_t[]) { png, (uint64_t) bitmap_reference, (uint64_t) bpp_reference, (uint64_t) width_reference, (uint64_t) height_reference });
}

void png_free(png_t png) {
	send_device(png_device, 0x6665, (uint64_t[]) { png });
}

#endif
