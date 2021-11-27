#if !defined(__AQUA_LIB__OGL_OGL)
#define __AQUA_LIB__OGL_OGL

#include <root.h>

#include "gl/gl.h"

typedef enum {
	OGL_CONTEXT_TARGET_WIN,
	OGL_CONTEXT_TARGET_WM,
	OGL_CONTEXT_TARGET_LEN
} ogl_context_target_t;

typedef struct {
	ogl_context_target_t target_type;

	union {
#if defined(__AQUA_LIB__AQUABSD_ALPS_WIN)
		win_t* win;
#endif
	} target;

	uint64_t context;
	gl_funcs_t gl;
} ogl_context_t;

static device_t ogl_device = -1;

static void ogl_func_not_loaded(void) {
	fprintf(stderr, "[OGL lib] Function not yet loaded. Use 'OGL_REQUIRE'.\n");
}

#if defined(__AQUA_LIB__AQUABSD_ALPS_WIN)
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

	context->context = send_device(ogl_device, 0x6363, (uint64_t[]) { OGL_CONTEXT_TARGET_WIN, win->win });

	if (!context->context) { // failed to create OpenGL context
		free(context);
		return NULL;
	}

	for (int i = 0; i < sizeof(context->gl) / sizeof(context->gl.Accum); i++) {
		((void**) &context->gl)[i] = ogl_func_not_loaded;
	}

	return context;
}

void* ogl_get_function(ogl_context_t* context, const char* name) {
	return (void*) send_device(ogl_device, 0x6766, (uint64_t[]) { context->context, (uint64_t) name });
}
#endif

#define OGL_REQUIRE(context, name) (context)->gl.name = ogl_get_function((context), "gl"#name);

void ogl_delete_context(ogl_context_t* context) {
	send_device(ogl_device, 0x6463, (uint64_t[]) { context->context });

#if defined(__AQUA_LIB__AQUABSD_ALPS_WIN)
	if (context->target_type == OGL_CONTEXT_TARGET_WIN) win_delete(context->target.win);
#endif

	free(context);
}

#endif
