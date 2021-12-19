#if !defined(__AQUA_LIB__AQUABSD_ALPS_WIN)
#define __AQUA_LIB__AQUABSD_ALPS_WIN

#include <root.h>

typedef enum {
	WIN_CB_DRAW,
	WIN_CB_LEN
} win_cb_t;

typedef struct {
	int x_pos, y_pos;
	unsigned x_res, y_res;

	uint64_t win;
} win_t;

static device_t win_device = -1;

int win_init(void) {
	if (win_device == -1) {
		win_device = query_device("aquabsd.alps.win");
	}

	if (win_device == -1) {
		return -1; // failed to query device
	}

	return 0;
}

win_t* win_create(unsigned x_res, unsigned y_res) {
	if (win_init() < 0) {
		return NULL;
	}
	
	win_t* win = calloc(1, sizeof *win);

	win->x_res = x_res;
	win->y_res = y_res;

	win->win = send_device(win_device, 0x6377, (uint64_t[]) { x_res, y_res });

	if (!win->win) { // failed to create window for some reason
		free(win);
		return NULL;
	}

	return win;
}

void win_set_caption(win_t* win, const char* caption) {
	send_device(win_device, 0x7363, (uint64_t[]) { win->win, (uint64_t) caption });
}

char* win_get_caption(win_t* win) {
	return (char*) send_device(win_device, 0x6763, (uint64_t[]) { win->win });
}

int win_register_cb(win_t* win, win_cb_t type, int (*cb) (uint64_t context, uint64_t param), void* param) {
	return send_device(win_device, 0x7263, (uint64_t[]) { win->win, type, (uint64_t) cb, (uint64_t) param });
}

int win_loop(win_t* win) {
	return send_device(win_device, 0x6C6F, (uint64_t[]) { win->win });
}

void win_sync(win_t* win) {
	win->x_pos = send_device(win_device, 0x7870, (uint64_t[]) { win->win });
	win->y_pos = send_device(win_device, 0x7970, (uint64_t[]) { win->win });

	win->x_res = send_device(win_device, 0x7872, (uint64_t[]) { win->win });
	win->y_res = send_device(win_device, 0x7972, (uint64_t[]) { win->win });
}

void win_grab_focus(win_t* win) {
	send_device(win_device, 0x6667, (uint64_t[]) { win->win });
}

void win_delete(win_t* win) {
	send_device(win_device, 0x6463, (uint64_t[]) { win->win });
	free(win);
}

#endif
