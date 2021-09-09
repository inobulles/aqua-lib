#if !defined(__AQUA_LIB__AQUABSD_ALPS_MOUSE)
#define __AQUA_LIB__AQUABSD_ALPS_MOUSE

#include <root.h>

// TODO error handling

#define MOUSE_BUTTON_COUNT 3
#define MOUSE_AXIS_COUNT 3

typedef enum {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
} mouse_button_t;

typedef enum {
	MOUSE_AXIS_X,
	MOUSE_AXIS_Y,
	MOUSE_AXIS_Z, // also known as the 'SCROLL' axis
} mouse_axis_t;

static device_t mouse_device = -1;
typedef uint64_t mouse_t;

mouse_t mouse_get_default(void) {
	if (mouse_device == -1) {
		mouse_device = query_device("aquabsd.alps.mouse");
	}

	return send_device(mouse_device, 0x646D, NULL);
}

int mouse_update(mouse_t mouse) {
	return (int) send_device(mouse_device, 0x756D, (uint64_t[]) { mouse });
}

unsigned mouse_poll_button(mouse_t mouse, mouse_button_t button) {
	return send_device(mouse_device, 0x7062, (uint64_t[]) { mouse, button });
}

float mouse_poll_axis(mouse_t mouse, mouse_axis_t axis) {
	uint64_t result = send_device(mouse_device, 0x7061, (uint64_t[]) { mouse, axis });
	return *(float*) &result;
}

#endif
