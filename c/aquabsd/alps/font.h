#if !defined(__AQUA_LIB__AQUABSD_ALPS_FONT)
#define __AQUA_LIB__AQUABSD_ALPS_FONT

#include "../../root.h"

// TODO error handling

static device_t font_device = -1;
typedef uint64_t font_t;

font_t font_load(const char* path) {
	if (font_device == -1) {
		font_device = query_device("aquabsd.alps.font");
	}

	return send_device(font_device, 0x6C66, (uint64_t[]) { (uint64_t) path });
}

int font_draw(font_t font, const char* string, float red, float green, float blue, float alpha, uint64_t wrap_width, uint64_t wrap_height, uint8_t** bitmap_reference, uint64_t* width_reference, uint64_t* height_reference) {
	return (int) send_device(font_device, 0x6466, (uint64_t[]) { font, (uint64_t) string, *(uint64_t*) &red, *(uint64_t*) &green, *(uint64_t*) &blue, *(uint64_t*) &alpha, (uint64_t) wrap_width, (uint64_t) wrap_height, (uint64_t) bitmap_reference, (uint64_t) width_reference, (uint64_t) height_reference });
}

void font_free(font_t font) {
	send_device(font_device, 0x6666, (uint64_t[]) { font });
}

#endif
