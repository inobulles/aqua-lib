#if !defined(__AQUA_LIB__AQUABSD_ALPS_KBD)
#define __AQUA_LIB__AQUABSD_ALPS_KBD

#include <root.h>
#include <stdbool.h>

// TODO error handling

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

unsigned kbd_poll_button(kbd_t kbd, unsigned button) {
	return send_device(kbd_device, 0x7062, (uint64_t[]) { kbd, button });
}

unsigned kbd_buf_len(kbd_t kbd) {
	return send_device(kbd_device, 0x626C, (uint64_t[]) { kbd });
}

int kbd_read_buf(kbd_t kbd, char* buf) { // expecting 'buf' to be 'kbd_buf_len(kbd)' wide
	return send_device(kbd_device, 0x7262, (uint64_t[]) { kbd, (uint64_t) buf });
}

unsigned kbd_keys_len(kbd_t kbd) {
	return send_device(kbd_device, 0x6B6C, (uint64_t[]) { kbd });
}

int kbd_read_keys(kbd_t kbd, char* buf) { // expecting 'buf' to be 'kbd_buf_len(kbd)' wide
	return send_device(kbd_device, 0x726B, (uint64_t[]) { kbd, (uint64_t) buf });
}

// helper functions (which wrap regular device commands)

static bool kbd_state[256] = { false };

unsigned kbd_button_down(kbd_t kbd, unsigned button) {
	bool pressed = kbd_poll_button(kbd, button);

	if (pressed) {
		bool rv = !kbd_state[button];
		kbd_state[button] = true;

		return rv;
	}

	kbd_state[button] = false;
	return false;
}

#endif
