#if !defined(__AQUA_LIB__AQUABSD_ALPS_UI)
#define __AQUA_LIB__AQUABSD_ALPS_UI

#include "../../root.h"

#include "../../aquabsd/alps/svg.h"
#include "../../aquabsd/alps/mouse.h"
#include "../../aquabsd/alps/kbd.h"

// TODO find some way to create a generic image object between the library & devices
//      see also ui_add_image, where png_load could be replaced by a generic img_load, which would use the correct format to load the image in question

#include "../../aquabsd/alps/png.h"

// support macros

#if defined(__AQUA_LIB__AQUABSD_ALPS_WIN)
	#define UI_WIN_SUPPORT
#endif

#if defined(__AQUA_LIB__AQUABSD_ALPS_WM)
	#define UI_WM_SUPPORT
#endif

#if defined(__AQUA_LIB__OGL_OGL)
	#define UI_OGL_SUPPORT
#endif

// compound support macros

#if defined(UI_WIN_SUPPORT) && defined(UI_OGL_SUPPORT)
	#define UI_OGL_WIN_SUPPORT
#endif

#if defined(UI_WM_SUPPORT) && defined(UI_OGL_SUPPORT)
	#define UI_OGL_WM_SUPPORT
#endif

// TODO error handling

typedef enum {
	UI_ELEMENT_ROOT = 0,
	UI_ELEMENT_SECTION,

	UI_ELEMENT_TITLE,
	UI_ELEMENT_SUBTITLE,
	UI_ELEMENT_PARAGRAPH,
	UI_ELEMENT_LOG,

	UI_ELEMENT_BUTTON,
	UI_ELEMENT_RADIO,
	UI_ELEMENT_ENTRY,

	UI_ELEMENT_SVG,
	UI_ELEMENT_IMAGE,
} ui_element_type_t;

typedef enum {
	UI_UNIT_RWF, UI_UNIT_RHF,
	UI_UNIT_WF,  UI_UNIT_HF,
	UI_UNIT_FWF, UI_UNIT_FHF,
	UI_UNIT_VX,  UI_UNIT_VY,
	UI_UNIT_PX,  UI_UNIT_PY,
	UI_UNIT_MM,
	UI_UNIT_AUTO,
} ui_unit_t;

typedef struct {
	ui_unit_t unit;
	float val;
} ui_value_t;

#define UI_VALUE(_unit, _val) ((ui_value_t) { .unit = (_unit), .val = (_val) })
#define UI_ZERO (UI_VALUE(0, 0))
#define UI_AUTO (UI_VALUE(UI_UNIT_AUTO, 0))

typedef enum {
	UI_CORNER_TOP_LEFT     = 0b0001,
	UI_CORNER_TOP_RIGHT    = 0b0010,
	UI_CORNER_BOTTOM_RIGHT = 0b0100,
	UI_CORNER_BOTTOM_LEFT  = 0b1000,
} ui_corner_t;

#define UI_CORNER_NONE 0

#define UI_CORNER_ALL (      \
	UI_CORNER_TOP_LEFT     | \
	UI_CORNER_TOP_RIGHT    | \
	UI_CORNER_BOTTOM_RIGHT | \
	UI_CORNER_BOTTOM_LEFT    \
)

typedef uint64_t ui_element_t;

typedef enum {
	UI_DISPLAY_NONE,
	UI_DISPLAY_FB_WIN,
	UI_DISPLAY_OGL_WIN,
	UI_DISPLAY_OGL_WM,
} ui_display_type_t;

typedef struct {
	ui_display_type_t type;

	uint64_t context;
	ui_element_t root;

	mouse_t mouse;
	kbd_t kbd;

	void* fb;

#if defined(UI_WIN_SUPPORT)
	win_t* win;
#endif

#if defined(UI_WM_SUPPORT)
	wm_t wm;
#endif

#if defined(UI_OGL_SUPPORT)
	ogl_context_t* ogl_context;
	gl_funcs_t* gl; // this can be found in 'context' ('ogl_context_t'); it's just here to make accessing it easier
#endif
} ui_context_t;

static device_t ui_device = -1;

// display setup functions

#define UI_VALIDATE_SETUP_FUNC(context) ((int64_t) (context) <= 0 ? -1 : 0)

typedef int (*ui_setup_func_t) (ui_context_t* context);

static int ui_setup_fb_win(ui_context_t* context) {
#if defined(UI_WIN_SUPPORT)
	context->type = UI_DISPLAY_FB_WIN;

	uint32_t const x_res = 800;
	uint32_t const y_res = 600;

	uint8_t const bpp = 32;

	context->win = win_create(x_res, y_res);
	context->fb = win_get_fb(context->win, bpp);

	context->context = send_device(ui_device, 0x6366, (uint64_t[]) { (uint64_t) context->fb, x_res, y_res, bpp });
	return UI_VALIDATE_SETUP_FUNC(context->context);
#else
	fprintf(stderr, "[UI lib] Framebuffer contexts require 'aquabsd/alps/win.h' to be included to function\n");
	return -1;
#endif
}

#if defined(UI_OGL_SUPPORT)
static int ui_setup_ogl(ui_context_t* context) { // generic function used by 'ui_setup_ogl_win' & 'ui_setup_ogl_wm'
	if (!context->ogl_context) {
		return -1;
	}

	context->win = context->ogl_context->target.win;
	context->gl = &context->ogl_context->gl;

	context->context = send_device(ui_device, 0x6367, (uint64_t[]) { (uint64_t) context->ogl_context->context });
	return UI_VALIDATE_SETUP_FUNC(context->context);
}
#endif

static int ui_setup_ogl_win(ui_context_t* context) {
#if defined(UI_OGL_WIN_SUPPORT)
	context->type = UI_DISPLAY_OGL_WIN;

	context->ogl_context = ogl_create_win_context(800, 600);
	return ui_setup_ogl(context);
#else
	fprintf(stderr, "[UI lib] OGL_WIN contexts require 'aquabsd/alps/win.h' & 'ogl/ogl.h' to be included in that order to function\n");
	return -1;
#endif
}

static int ui_setup_ogl_wm(ui_context_t* context) {
#if defined(UI_OGL_WM_SUPPORT)
	context->type = UI_DISPLAY_OGL_WM;

	context->ogl_context = ogl_create_wm_context();

	if (context->ogl_context) {
		context->wm = context->ogl_context->target.wm;
	}

	return ui_setup_ogl(context);
#else
	fprintf(stderr, "[UI lib] OGL_WM contexts require 'aquabsd/alps/wm.h' & 'ogl/ogl.h' to be included in that order to function\n");
	return -1;
#endif
}

// this function should choose and setup an appropriate display
// the 'hint' argument tells it what exactly to choose if necessary

ui_context_t* ui_create(ui_display_type_t hint) {
	if (ui_device == -1) {
		ui_device = query_device("aquabsd.alps.ui");
	}

	ui_context_t* context = (ui_context_t*) calloc(1, sizeof *context);

	// find best display type to use following the 'hint' argument
	// do this by attempting to setup each one and breaking (i.e. going to 'found') upon success

	int setup_func_count = 0;
	ui_setup_func_t* setup_funcs;

	if (hint == UI_DISPLAY_FB_WIN) {
		setup_func_count = 1;
		setup_funcs = (ui_setup_func_t[]) { ui_setup_fb_win };
	}

	else if (hint == UI_DISPLAY_OGL_WIN) {
		setup_func_count = 2;
		setup_funcs = (ui_setup_func_t[]) { ui_setup_ogl_win, ui_setup_fb_win };
	}

	else if (hint == UI_DISPLAY_OGL_WM) {
		setup_func_count = 3;
		setup_funcs = (ui_setup_func_t[]) { ui_setup_ogl_wm, ui_setup_ogl_win, ui_setup_fb_win };
	}

	for (int i = 0; i < setup_func_count; i++) {
		if (setup_funcs[i](context) == 0) {
			goto found;
		}
	}

	// error finding display setup function

	free(context);
	return NULL;

found:

	// handle input (give mouse & keyboard)

	context->mouse = mouse_get_default();
	send_device(ui_device, 0x676D, (uint64_t[]) { context->context, context->mouse });

	context->kbd = kbd_get_default();
	send_device(ui_device, 0x676B, (uint64_t[]) { context->context, context->kbd });

	// do any generic things that still need to be done

	context->root = send_device(ui_device, 0x6772, (uint64_t[]) { context->context });

	return context;
}

void ui_render(ui_context_t* context, float delta) {
	mouse_update(context->mouse);
	send_device(ui_device, 0x7263, (uint64_t[]) { (uint64_t) context->context, *(uint64_t*) &delta });
}

void ui_free(ui_context_t* context) {
	send_device(ui_device, 0x6663, (uint64_t[]) { context->context });
	free(context);

#if defined(UI_WIN_SUPPORT)
	if (context->type == UI_DISPLAY_FB_WIN) {
		win_delete(context->win);
	} else
#endif

#if defined(UI_OGL_SUPPORT)
	if (context->type == UI_DISPLAY_OGL_WIN || context->type == UI_DISPLAY_OGL_WM) {
		ogl_delete_context(context->ogl_context);
	} else
#endif

	{} // close off last else
}

// element creation functions

ui_element_t ui_add_section(ui_element_t parent, unsigned scrollable, ui_value_t x, ui_value_t y) {
	return send_device(ui_device, 0x6165, (uint64_t[]) { parent, UI_ELEMENT_SECTION, scrollable, x.unit, *(uint64_t*) &x.val, y.unit, *(uint64_t*) &y.val });
}

ui_element_t ui_add_text(ui_element_t parent, ui_element_type_t type, const char* text) {
	return send_device(ui_device, 0x6165, (uint64_t[]) { parent, type, (uint64_t) text });
}

#define TEXT_FUNC(name, CAPITAL) \
	ui_element_t ui_add_##name(ui_element_t parent, const char* text) { \
		return ui_add_text(parent, UI_ELEMENT_##CAPITAL, text); \
	}

TEXT_FUNC(title,     TITLE)
TEXT_FUNC(subtitle,  SUBTITLE)
TEXT_FUNC(paragraph, PARAGRAPH)
TEXT_FUNC(log,       LOG)
TEXT_FUNC(button,    BUTTON)
TEXT_FUNC(entry,     ENTRY)

#undef TEXT_FUNC

ui_element_t ui_add_radio(ui_element_t parent, unsigned default_selection, unsigned count, const char* entries[count]) {
	return send_device(ui_device, 0x6165, (uint64_t[]) { parent, UI_ELEMENT_RADIO, default_selection, count, (uint64_t) entries });
}

// TODO goes for both SVG's and images, but make sure the UI device ends up freeing those objects to prevent memory leaks

ui_element_t _ui_add_svg(ui_element_t parent, svg_t svg, ui_value_t height, unsigned themeable) {
	// XXX this function should probably be avoided for now util I find (perhaps) a better solution if you can
	//     for *most* use cases just 'ui_add_svg' should be sufficient
	//     anyway, this function is a bit peculiar because the ui device frees 'svg', not the client

	return send_device(ui_device, 0x6165, (uint64_t[]) { parent, UI_ELEMENT_SVG, svg, height.unit, *(uint64_t*) &height.val, themeable });
}

ui_element_t ui_add_svg(ui_element_t parent, const char* drive, const char* path, ui_value_t height, unsigned themeable) {
	// wrapper around _ui_add_svg to be able to pass in a path instead of SVG

	svg_t svg = svg_load(drive, path);
	return _ui_add_svg(parent, svg, height, themeable);
}

ui_element_t _ui_add_image(ui_element_t parent, png_t image, ui_value_t width, ui_value_t height) {
	// XXX cf. _ui_add_svg

	return send_device(ui_device, 0x6165, (uint64_t[]) { parent, UI_ELEMENT_IMAGE, image, width.unit, *(uint64_t*) &width.val, height.unit, *(uint64_t*) &height.val });
}

ui_element_t ui_add_image(ui_element_t parent, const char* drive, const char* path, ui_value_t width, ui_value_t height) {
	// wrapper around _ui_add_image to be able to pass in a path instead of image object

	png_t png = png_load(drive, path);
	return _ui_add_image(parent, png, width, height);
}

// element manipulation functions

void ui_rem_element(ui_element_t element) {
	send_device(ui_device, 0x7265, (uint64_t[]) { element });
}

void ui_move_element_before(ui_element_t element, ui_element_t target) {
	send_device(ui_device, 0x7662, (uint64_t[]) { element, target });
}

void ui_move_element_after(ui_element_t element, ui_element_t target) {
	send_device(ui_device, 0x7661, (uint64_t[]) { element, target });
}

void ui_move_element_head(ui_element_t element) {
	ui_move_element_after(element, 0);
}

void ui_move_element_tail(ui_element_t element) {
	ui_move_element_before(element, 0);
}

void ui_clear_children(ui_element_t element) {
	send_device(ui_device, 0x6363, (uint64_t[]) { element });
}

// element defaults-setting functions

void ui_set_section(ui_element_t section, unsigned scrollable, ui_value_t x, ui_value_t y) {
	send_device(ui_device, 0x7364, (uint64_t[]) { section, scrollable, x.unit, *(uint64_t*) &x.val, y.unit, *(uint64_t*) &y.val });
}

void ui_set_text(ui_element_t text_element, const char* text) {
	send_device(ui_device, 0x7364, (uint64_t[]) { text_element, (uint64_t) text });
}

void ui_set_radio(ui_element_t radio, unsigned default_selection, unsigned count, const char* entries[count]) {
	send_device(ui_device, 0x7364, (uint64_t[]) { radio, default_selection, count, (uint64_t) entries });
}

void ui_set_svg(ui_element_t svg_element, const char* drive, const char* path, ui_value_t height) {
	svg_t svg = svg_load(drive, path);
	send_device(ui_device, 0x7364, (uint64_t[]) { svg_element, svg, height.unit, *(uint64_t*) &height.val });
}

// element property setting functions

void ui_set_visibility(ui_element_t element, unsigned visibility, float windup_dir) {
	send_device(ui_device, 0x7376, (uint64_t[]) { element, visibility, *(uint64_t*) &windup_dir });
}

void ui_set_windup(ui_element_t element, float scale, ui_value_t x, ui_value_t y) {
	send_device(ui_device, 0x7377, (uint64_t[]) { element, *(uint64_t*) &scale, x.unit, *(uint64_t*) &x.val, y.unit, *(uint64_t*) &y.val });
}

void ui_set_grid(ui_element_t section, unsigned grid_x, unsigned grid_y) {
	send_device(ui_device, 0x7367, (uint64_t[]) { section, grid_x, grid_y });
}

void ui_set_sticky(ui_element_t element, unsigned sticky) {
	send_device(ui_device, 0x7373, (uint64_t[]) { element, sticky });
}

void ui_set_gobbler(ui_element_t element, unsigned gobbler) {
	send_device(ui_device, 0x6762, (uint64_t[]) { element, gobbler });
}

void ui_set_abs(ui_element_t element, unsigned abs, ui_value_t x, ui_value_t y) {
	send_device(ui_device, 0x7365, (uint64_t[]) { element, abs, x.unit, *(uint64_t*) &x.val, y.unit, *(uint64_t*) &y.val });
}

void ui_set_anchor(ui_element_t element, float x, float y) {
	send_device(ui_device, 0x656E, (uint64_t[]) { element, *(uint64_t*) &x, *(uint64_t*) &y });
}

void ui_set_prop(ui_element_t element, const char* name, uint64_t value) {
	send_device(ui_device, 0x7370, (uint64_t[]) { element, (uint64_t) name, value });
}

static inline void ui_set_ptr_prop(ui_element_t element, const char* name, void* ptr) {
	ui_set_prop(element, name, (uint64_t) ptr);
}

void ui_set_margin(ui_element_t element, ui_value_t left, ui_value_t right, ui_value_t top, ui_value_t bottom) {
	send_device(ui_device, 0x6D65, (uint64_t[]) { element, left.unit, *(uint64_t*) &left.val, right.unit, *(uint64_t*) &right.val, top.unit, *(uint64_t*) &top.val, bottom.unit, *(uint64_t*) &bottom.val });
}

void ui_set_padding(ui_element_t element, ui_value_t left, ui_value_t right, ui_value_t top, ui_value_t bottom) {
	send_device(ui_device, 0x7065, (uint64_t[]) { element, left.unit, *(uint64_t*) &left.val, right.unit, *(uint64_t*) &right.val, top.unit, *(uint64_t*) &top.val, bottom.unit, *(uint64_t*) &bottom.val });
}

void ui_set_scroll(ui_element_t element, ui_value_t x, ui_value_t y) {
	send_device(ui_device, 0x737A, (uint64_t[]) { element, x.unit, *(uint64_t*) &x.val, y.unit, *(uint64_t*) &y.val });
}

// state functions

unsigned ui_clicked(ui_element_t element) {
	return (unsigned) send_device(ui_device, 0x636B, (uint64_t[]) { element });
}

unsigned ui_pressed(ui_element_t element) {
	return (unsigned) send_device(ui_device, 0x7072, (uint64_t[]) { element });
}

unsigned ui_get_selection(ui_element_t element) {
	return (unsigned) send_device(ui_device, 0x6773, (uint64_t[]) { element });
}

// misc functions

void ui_set_theme(ui_context_t* context, const char* theme) {
	send_device(ui_device, 0x7374, (uint64_t[]) { context->context, (uint64_t) theme });
}

void ui_set_base(ui_context_t* context, const char* colour) {
	send_device(ui_device, 0x7362, (uint64_t[]) { context->context, (uint64_t) colour });
}

int ui_snap(ui_element_t element) {
	return send_device(ui_device, 0x736E, (uint64_t[]) { element });
}

#endif
