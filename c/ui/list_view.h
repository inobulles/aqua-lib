#if !defined(__AQUA_LIB__UI_LIST_VIEW)
#define __AQUA_LIB__UI_LIST_VIEW

#include <aquabsd/alps/ui.h>

#include <types/str.h>
#include <types/stack.h>

// list items

typedef struct {
	object_t obj;
	
	str_t* str;

	ui_element_t section;
	ui_element_t text;
} ui_list_item_t;

static type_t ui_list_item_type; // forward declaration

static ui_list_item_t* ui_list_item_new(ui_element_t section, ui_element_t text, const char* cstr) {
	// there isn't really a way to check this currently, but it is assumed 'text' is a descendant of 'section'

	if (!cstr) {
		return NULL;
	}

	ui_list_item_t* item = batch_alloc(&ui_list_item_type);
	item->str = str_new(cstr);

	item->section = section;
	item->text = text;

	return item;
}

static void ui_list_item_print(ui_list_item_t* item) {
	_print(item->str);
}

static void ui_list_item_del(ui_list_item_t* item) {
	// don't remove item->section & item->text; this is something that should be handled by the user
	// the rationale behind this is that, when deleting a UIListView, every element will individually be removed, which is inefficient
	// also, although this is assumed to be the case (cf. ui_list_item_new), there's a chance that item->text is not actually a descendant of item->section, and thus will be... just no don't do it or I'll get mad 😡

	del(item->str);
}

static type_t ui_list_item_type = {
	.name = "UIListViewItem",
	.size = sizeof(ui_list_item_t),

	// unary operators

	.print = (void*) ui_list_item_print,
	.del = (void*) ui_list_item_del,
};

// list view itself

typedef struct {
	object_t obj;
	stack_t* items;
} ui_list_view_t;

static type_t ui_list_view_type; // forward declaration

static ui_list_view_t* ui_list_view_new(void) {
	ui_list_view_t* view = batch_alloc(&ui_list_view_type);
	view->items = stack_new(0, NULL);

	return view;
}

static uint64_t ui_list_view_len(ui_list_view_t* view) {
	return len(view->items);
}

static void ui_list_view_print(ui_list_view_t* view) {
	_print(view->items);
}

static void ui_list_view_del(ui_list_view_t* view) {
	del(view->items);
}

static int ui_list_view_push(ui_list_view_t* x, ui_list_item_t* item) {
	if (item->obj.type != &ui_list_item_type) {
		return -1; // don't try to add anything other than a UIListItem to a UIListView or I'll get extra angery again 😡😡😡
	}

	return push(x->items, item);
}

static object_t* ui_list_view_pop(ui_list_view_t* x) {
	return pop(x->items);
}

static object_t* ui_list_view_iget(ui_list_view_t* x, int64_t index) {
	return iget(x->items, index);
}

static type_t ui_list_view_type = {
	.name = "UIListView",
	.size = sizeof(ui_list_view_t),

	// unary operators

	.len = (void*) ui_list_view_len,
	.print = (void*) ui_list_view_print,
	.del = (void*) ui_list_view_del,

	// list-like type operators

	.push = (void*) ui_list_view_push,
	.pop = (void*) ui_list_view_pop,
	.iget = (void*) ui_list_view_iget,
};

// special UIListView-specific operations

#if !defined(UI_LIST_VIEW_HL_START)
	#define UI_LIST_VIEW_HL_START "<span background='yellow' foreground='black'>"
#endif

#if !defined(UI_LIST_VIEW_HL_END)
	#define UI_LIST_VIEW_HL_END "</span>"
#endif

static int ui_list_view_search(ui_list_view_t* view, str_t* needle, unsigned hl /* highlight */) {
	batch_push();
	
	str_t* hl_start, *hl_end;

	if (hl) {
		hl_start = str_new(UI_LIST_VIEW_HL_START);
		hl_end = str_new(UI_LIST_VIEW_HL_END);
	}

	unsigned empty_needle = !len(needle);

	for (int i = 0; i < len(view->items); i++) {
		ui_list_item_t* item = iget(view->items, i);

		if (empty_needle) {
			ui_set_visibility(item->section, 1, 0);
			ui_set_text(item->text, item->str->cstr);
			
			continue;
		}

		// needle contains something? shame, we'll have to actually do work 🙄

		batch_push();

		str_t* haystack = item->str;
		stack_t* components = split(haystack, needle);

		unsigned found = len(components) > 1;

		if (!found) {
			goto end;
		}

		if (!hl) {
			goto end;
		}

		// render highlight

		str_t* markup = str_new(NULL);

		for (int j = 0; j < len(components); j++) {
			str_t* bit = iget(components, j);

			if (j) {
				markup = add(add(add(markup, hl_start), needle), hl_end);
			}

			markup = add(markup, bit);
		}

		ui_set_text(item->text, markup->cstr); // if you're wondering, it's totally safe to free markup->cstr after, as ui_set_text duplicated the string

	end:

		ui_set_visibility(item->section, found, 0);
		batch_pop();
	}

	batch_pop();
	return 0;
}

#endif