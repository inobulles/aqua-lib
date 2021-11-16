#if !defined(__AQUA_LIB__OGL_OGL)
#define __AQUA_LIB__OGL_OGL

#include <root.h>

#include <aquabsd/alps/win.h>

typedef enum {
	OGL_CONTEXT_TARGET_WINDOW,
	OGL_CONTEXT_TARGET_WM,
	OGL_CONTEXT_TARGET_LEN
} ogl_context_target_t;

typedef struct {
	ogl_context_target_t target_type;

	union {
		win_t* win;
	} target;

	uint64_t context;
} ogl_context_t;

static device_t ogl_device = -1;

ogl_context_t* ogl_create_win_context(unsigned x_res, unsigned y_res) {
	if (ogl_device == -1) {
		ogl_device = query_device("aquabsd.alps.ogl");
	}

	if (ogl_device == -1) {
		return NULL;
	}

	// create window

	win_t* win = win_create(x_res, y_res);

	if (!win) { // failed to create window
		return NULL;
	}

	ogl_context_t* context = calloc(1, sizeof *context);
	context->target.win = win;

	context->context = send_device(ogl_device, 0x6363, (uint64_t[]) { OGL_CONTEXT_TARGET_WINDOW, win->win });

	if (!context->context) { // failed to create OpenGL context
		free(context);
		return NULL;
	}

	return context;
}

void* ogl_get_function(ogl_context_t* context, const char* name) {
	return (void*) send_device(ogl_device, 0x6766, (uint64_t[]) { context->context, (uint64_t) name });
}

void ogl_delete_context(ogl_context_t* context) {
	send_device(ogl_device, 0x6463, (uint64_t[]) { context->context });

	if (context->target_type == OGL_CONTEXT_TARGET_WINDOW) win_delete(context->target.win);

	free(context);
}

#endif
