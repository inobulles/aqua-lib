#pragma once

#include <root.hpp>
#include <vector>

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

	struct Val {
		Unit unit;
		float val;

		Val(Unit unit, float val) :
			unit(unit),
			val(val)
		{
		}

		operator aqua_libc::ui_value_t() {
			auto c_unit = static_cast<aqua_libc::ui_unit_t>(unit);
			return { .unit = c_unit, .val = val };
		}
	};

	// elements

	class Element {
	public: // XXX I don't understand why this can't be 'protected' but whatever

		aqua_libc::ui_element_t element;

	public:

		// element manipulation methods

		void rem(void) {
			aqua_libc::ui_rem_element(element);
		}

		void move_before(Element& target) {
			aqua_libc::ui_move_element_before(element, target.element);
		}

		void move_after(Element& target) {
			aqua_libc::ui_move_element_after(element, target.element);
		}

		void move_head(void) {
			aqua_libc::ui_move_element_head(element);
		}

		void move_tail(void) {
			aqua_libc::ui_move_element_tail(element);
		}

		void clear_children(void) {
			aqua_libc::ui_clear_children(element);
		}

		// element property setting functions

		void set_visibility(bool visibility, float windup_dir) {
			aqua_libc::ui_set_visibility(element, visibility, windup_dir);
		}

		void set_windup(float scale, Val x, Val y) {
			auto c_x = static_cast<aqua_libc::ui_value_t>(x);
			auto c_y = static_cast<aqua_libc::ui_value_t>(y);

			aqua_libc::ui_set_windup(element, scale, c_x, c_y);
		}

		void set_sticky(bool sticky) {
			aqua_libc::ui_set_sticky(element, sticky);
		}
	};

	struct Section : Element {
		Section(Element& parent, bool scrollable, Val x, Val y) {
			auto c_x = static_cast<aqua_libc::ui_value_t>(x);
			auto c_y = static_cast<aqua_libc::ui_value_t>(y);

			element = aqua_libc::ui_add_section(parent.element, scrollable, c_x, c_y);
		}

		~Section() {
			aqua_libc::ui_rem_element(element);
		}

		void set(bool scrollable, Val x, Val y) {
			auto c_x = static_cast<aqua_libc::ui_value_t>(x);
			auto c_y = static_cast<aqua_libc::ui_value_t>(y);

			aqua_libc::ui_set_section(element, scrollable, c_x, c_y);
		}
	};

	struct Text : Element {
		Text(Element& parent, Element_type type, std::string text) {
			auto c_type = static_cast<aqua_libc::ui_element_type_t>(type);
			element = aqua_libc::ui_add_text(parent.element, c_type, text.c_str());
		}

		void set(std::string text) {
			aqua_libc::ui_set_text(element, text.c_str());
		}
	};

	#define TEXT_CLASS(Name, CAPITAL) \
		struct Name : Text { \
			Name(Element& parent, std::string text) : Text(parent, ELEMENT_##CAPITAL, text) { \
			} \
		};

	TEXT_CLASS(Title,     TITLE)
	TEXT_CLASS(Subtitle,  SUBTITLE)
	TEXT_CLASS(Paragraph, PARAGRAPH)
	TEXT_CLASS(Log,       LOG)
	TEXT_CLASS(Button,    BUTTON)
	TEXT_CLASS(Entry,     ENTRY)

	#undef TEXT_CLASS

	struct Radio : Element {
		Radio(Element& parent, unsigned default_selection, std::vector<std::string>& entries) {
			size_t count = entries.size();
			const char* c_entries[count];

			for (size_t i = 0; i < count; i++) {
				c_entries[i] = entries[i].c_str();
			}

			element = aqua_libc::ui_add_radio(parent.element, default_selection, count, c_entries);
		}

		void set(unsigned default_selection, std::vector<std::string>& entries) {
			size_t count = entries.size();
			const char* c_entries[count];

			for (size_t i = 0; i < count; i++) {
				c_entries[i] = entries[i].c_str();
			}

			aqua_libc::ui_set_radio(element, default_selection, count, c_entries);
		}
	};

	// context stuff

	enum Display_type {
		DISPLAY_NONE    = aqua_libc::UI_DISPLAY_NONE,
		DISPLAY_VGA     = aqua_libc::UI_DISPLAY_VGA,
		DISPLAY_OGL_WIN = aqua_libc::UI_DISPLAY_OGL_WIN,
		DISPLAY_OGL_WM  = aqua_libc::UI_DISPLAY_OGL_WM,
	};

	class Context {
		aqua_libc::ui_context_t* context;

	public:

		Element root;
		win::Win win;

		Context(Display_type hint) {
			auto c_hint = static_cast<aqua_libc::ui_display_type_t>(hint);
			context = aqua_libc::ui_create(c_hint);

			root.element = context->root;
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
