#if !defined(__AQUA_LIB__AQUABSD_ALPS_VGA)
#define __AQUA_LIB__AQUABSD_ALPS_VGA

#include <root.h>

// TODO error handling

static device_t vga_device = -1;

typedef struct {
	uint64_t id;

	uint64_t text;

	uint64_t width, height;
	uint64_t bpp, fps;
} vga_mode_t;

int vga_get_mode_count(void) {
	return (int) send_device(vga_device, 0x6D63, NULL);
}

vga_mode_t* vga_get_modes(void) {
	return (vga_mode_t*) send_device(vga_device, 0x6D64, NULL);
}

int vga_set_mode(vga_mode_t* mode) {
	return (int) send_device(vga_device, 0x736D, (uint64_t[]) { (uint64_t) mode });
}

void* vga_get_framebuffer(void) {
	return (void*) send_device(vga_device, 0x6662, NULL);
}

int vga_flip(void) {
	return (int) send_device(vga_device, 0x666C, NULL);
}

int vga_init(void) {
	if (vga_device == -1) {
		vga_device = query_device("aquabsd.alps.vga");
	}

	return vga_get_mode_count() == -1 ? -1 : 0;
}

#endif
