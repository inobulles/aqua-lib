#if !defined(__AQUA_LIB__AQUABSD_ALPS_SVG)
#define __AQUA_LIB__AQUABSD_ALPS_SVG

#include <root.h>

static device_t svg_device = -1;
typedef uint64_t svg_t;

int svg_init(void) {
	if (svg_device == -1) {
		svg_device = query_device("aquabsd.alps.svg");
	}

	if (svg_device == -1) {
		return -1; // failed to query device
	}

	return 0;
}

svg_t svg_load(const char* path) {
	if (svg_init() < 0) {
		return 0;
	}

	return send_device(svg_device, 0x6C73, (uint64_t[]) { (uint64_t) path });
}

svg_t svg_load_cstr(const char* cstr) {
	if (svg_init() < 0) {
		return 0;
	}

	return send_device(svg_device, 0x6C64, (uint64_t[]) { (uint64_t) cstr });
}

int svg_draw(svg_t svg, uint64_t size, uint8_t** bitmap_reference, uint64_t* width_reference, uint64_t* height_reference) {
	return (int) send_device(svg_device, 0x6473, (uint64_t[]) { svg, size, (uint64_t) bitmap_reference, (uint64_t) width_reference, (uint64_t) height_reference });
}

void svg_free(svg_t svg) {
	send_device(svg_device, 0x6673, (uint64_t[]) { svg });
}

#endif