#if !defined(__AQUA_LIB__AQUABSD_ALPS_KBD)
#define __AQUA_LIB__AQUABSD_ALPS_KBD

#include <root.h>

// TODO error handling

#define KBD_BUTTON_COUNT 5

typedef enum {
	KBD_BUTTON_ESC,

	KBD_BUTTON_UP,
	KBD_BUTTON_DOWN,
	KBD_BUTTON_LEFT,
	KBD_BUTTON_RIGHT,
} kbd_button_t;

static device_t kbd_device = -1;
typedef uint64_t kbd_t;

kbd_t kbd_get_default(void) {
	if (kbd_device == -1) {
		kbd_device = query_device("aquabsd.alps.kbd");
	}

	return send_device(kbd_device, 0x646B, NULL);
}

int kbd_update(kbd_t kbd) {
	return (int) send_device(kbd_device, 0x756B, (uint64_t[]) { kbd });
}

unsigned kbd_poll_button(kbd_t kbd, kbd_button_t button) {
	return send_device(kbd_device, 0x7062, (uint64_t[]) { kbd, button });
}

#endif
