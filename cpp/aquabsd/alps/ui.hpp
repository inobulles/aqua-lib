#pragma once

#include <root.hpp>

namespace aqua_libc {
	extern "C" {
		#include <../c/aquabsd/alps/ui.h>
	}
}

namespace aqua::aquabsd::alps::ui {
	enum Element_type {
		ELEMENT_ROOT      = aqua_libc::UI_ELEMENT_ROOT,
		ELEMENT_SECTION   = aqua_libc::UI_ELEMENT_SECTION,

		ELEMENT_TITLE     = aqua_libc::UI_ELEMENT_TITLE,
		ELEMENT_SUBTITLE  = aqua_libc::UI_ELEMENT_SUBTITLE,
		ELEMENT_PARAGRAPH = aqua_libc::UI_ELEMENT_PARAGRAPH,
		ELEMENT_LOG       = aqua_libc::UI_ELEMENT_LOG,

		ELEMENT_BUTTON    = aqua_libc::UI_ELEMENT_BUTTON,
		ELEMENT_RADIO     = aqua_libc::UI_ELEMENT_RADIO,
		ELEMENT_ENTRY     = aqua_libc::UI_ELEMENT_ENTRY,

		ELEMENT_SVG       = aqua_libc::UI_ELEMENT_SVG,
		ELEMENT_IMAGE     = aqua_libc::UI_ELEMENT_IMAGE,
	};

	enum Unit {
		UNIT_RWF  = aqua_libc::UI_UNIT_RWF,
		UNIT_RHF  = aqua_libc::UI_UNIT_RHF,

		UNIT_WF   = aqua_libc::UI_UNIT_WF,
		UNIT_HF   = aqua_libc::UI_UNIT_HF,

		UNIT_FWF  = aqua_libc::UI_UNIT_FWF,
		UNIT_FHF  = aqua_libc::UI_UNIT_FHF,

		UNIT_VX   = aqua_libc::UI_UNIT_VX,
		UNIT_VY   = aqua_libc::UI_UNIT_VY,

		UNIT_PX   = aqua_libc::UI_UNIT_PX,
		UNIT_PY   = aqua_libc::UI_UNIT_PY,

		UNIT_MM   = aqua_libc::UI_UNIT_MM,
		UNIT_AUTO = aqua_libc::UI_UNIT_AUTO,
	};

	enum Corner {
		CORNER_TOP_LEFT     = aqua_libc::UI_CORNER_TOP_LEFT,
		CORNER_TOP_RIGHT    = aqua_libc::UI_CORNER_TOP_RIGHT,
		CORNER_BOTTOM_LEFT  = aqua_libc::UI_CORNER_BOTTOM_LEFT,
		CORNER_BOTTOM_RIGHT = aqua_libc::UI_CORNER_BOTTOM_RIGHT,
	};

	inline Corner operator |(Corner a, Corner b) {
		return static_cast<Corner>(static_cast<int>(a) | static_cast<int>(b));
	}

	const Corner CORNER_NONE = static_cast<Corner>(0);
	const Corner CORNER_ALL = CORNER_TOP_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_LEFT | CORNER_BOTTOM_RIGHT;

	enum Display_type {
		DISPLAY_NONE    = aqua_libc::UI_DISPLAY_NONE,
		DISPLAY_VGA     = aqua_libc::UI_DISPLAY_VGA,
		DISPLAY_OGL_WIN = aqua_libc::UI_DISPLAY_OGL_WIN,
		DISPLAY_OGL_WM  = aqua_libc::UI_DISPLAY_OGL_WM,
	};

	class Context {
		aqua_libc::ui_context_t* context;

	public:

		win::Win win;

		Context(Display_type hint) {
			auto c_hint = static_cast<aqua_libc::ui_display_type_t>(hint);
			context = aqua_libc::ui_create(c_hint);

			win = win::Win(context->ogl.win);
		}

		~Context() {
			aqua_libc::ui_free(context);
		}

		void render(float delta) {
			aqua_libc::ui_render(context, delta);
		}
	};
}
