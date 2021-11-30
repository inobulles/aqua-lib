#if !defined(__AQUA_LIB__AQUABSD_ALPS_WM)
#define __AQUA_LIB__AQUABSD_ALPS_WM

#include <aquabsd/alps/win.h>

typedef enum {
	WM_CB_CREATE,
	WM_CB_MODIFY,
	WM_CB_DELETE,
	WM_CB_LEN
} wm_cb_t;

typedef uint64_t wm_t;

device_t wm_device = -1;

int wm_init(void) {
	if (wm_device == -1) {
		wm_device = query_device("aquabsd.alps.wm");
	}

	if (wm_device == -1) { // failed to query device
		return -1;
	}

	return 0;
}

wm_t wm_create(void) {
	if (wm_init() < 0) {
		return 0;
	}

	return send_device(wm_device, 0x6377, NULL);
}

void wm_delete(wm_t wm) {
	send_device(wm_device, 0x6477, (uint64_t[]) { wm });
}

win_t* wm_get_root(wm_t wm) {
	if (win_init() < 0) {
		return NULL;
	}

	// get root window information

	uint64_t _win = send_device(wm_device, 0x7277, (uint64_t[]) { wm });

	uint64_t x_res = send_device(wm_device, 0x7872, (uint64_t[]) { wm });
	uint64_t y_res = send_device(wm_device, 0x7972, (uint64_t[]) { wm });

	// create window object

	win_t* win = calloc(1, sizeof *win);
	win->win = _win;

	win->x_res = x_res;
	win->y_res = y_res;

	return win;
}

int wm_register_cb(wm_t wm, wm_cb_t type, int (*cb) (uint64_t wm, uint64_t win, uint64_t param), void* param) {
	return send_device(wm_device, 0x7263, (uint64_t[]) {wm, type, (uint64_t) cb, (uint64_t) param});
}

win_t* wm_get_overlay(wm_t wm) {
	uint64_t _win = send_device(wm_device, 0x6F77, (uint64_t[]) { wm });
	
	win_t* win = wm_get_root(wm); // base ourselves off the root window
	win->win = _win;

	return win;
}

#endif
