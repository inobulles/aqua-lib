// "list" implies a doubly-linked list

// TODO how are objects which are contained within multiple lists supposed to work?
// TODO maybe find a failsafe to not accidentally remove an element from a list which doesn't contain it

#if !defined(__AQUA_LIB__TYPES_LIST)
#define __AQUA_LIB__TYPES_LIST

#include <types.h>

typedef struct {
	object_t obj;

	uint64_t len;

	object_t* head;
	object_t* tail;
} list_t;

static type_t list_type; // forward declaration

static int list_push(list_t* x, object_t* y);

static list_t* list_new(uint64_t len, void* elems[len]) {
	if ((int64_t) len < 0) {
		return NULL; // invalid length
	}

	for (int i = 0; i < len; i++) {
		if (!elems[i]) {
			return NULL; // NULL can't be an element
		}
	}

	list_t* list = calloc(1, sizeof *list);
	list->obj.type = &list_type;

	list->len = len;

	list->head = NULL;
	list->tail = NULL;

	for (int i = 0; i < len; i++) {
		list_push(list, elems[i]);
	}

	return list;
}

// unary operators

static uint64_t list_len(list_t* list) {
	return list->len;
}

static void list_print(list_t* list) {
	printf("[");

	for (object_t* elem = list->head; elem; elem = elem->next) {
		if (elem->next) {
			printf(", ");
		}

		printf("<%s ", elem->type->name);
		print(elem);
		printf(">");
	}

	printf("]");
}

static void list_del(list_t* list) {
	free(list);
}

// binary operators

static unsigned list_eq(list_t* x, list_t* y) {
	if (y->obj.type != &list_type) {
		return 0; // comparing to invalid type
	}

	if (x->len != y->len) {
		return 0; // lengths are not equal
	}

	for (
		object_t* x_elem = x->head, *y_elem = y->head;
		x_elem && y_elem; // it should never be the case that one of these is NULL but not the other
		x_elem = x_elem->next, y_elem = y_elem->next
	) {
		if (!eq(x_elem, y_elem)) {
			return 0; // found one differing element
		}
	}

	return 1; // all elements are equal
}

// indexing operators

static inline object_t* _list_process_index_head(list_t* list, int64_t index) {
	object_t* elem = list->head;

	for (int64_t i = 0; elem && i != index; elem = elem->next, i++);

	return elem;
}

static inline object_t* _list_process_index_tail(list_t* list, int64_t index) {
	object_t* elem = list->tail;

	for (int64_t i = list->len - 1; elem && i != index; elem = elem->prev, i--);

	return elem;
}

static inline object_t* _list_process_index(list_t* list, int64_t index) {
	index += list->len * (index < 0); // wrap negative values

	if (index < 0 || index >= list->len) {
		return NULL; // index out of bounds
	}

	if (index < list->len / 2) {
		return _list_process_index_head(list, index);
	}

	else {
		return _list_process_index_tail(list, index);
	}
}

static object_t* list_iget(list_t* x, int64_t index) {
	return _list_process_index(x, index);
}

static int list_iset(list_t* x, int64_t index, object_t* y) {
	if (!y) {
		return -1; // NULL can't be an element
	}

	object_t* elem = _list_process_index(x, index);

	if (!elem) {
		return -1; // invalid index
	}

	if (elem->prev) {
		elem->prev->next = y;
		y->prev = elem->prev;
	}

	else {
		x->head = y;
		y->prev = NULL;
	}

	if (elem->next) {
		elem->next->prev = y;
		y->next = elem->next;
	}

	else {
		x->tail = y;
		y->next = NULL;
	}

	return 0;
}

// list-like type operators

static inline int _list_insert_check_first(list_t* list, object_t* elem) {
	if (list->head && list->tail) { // if head is NULL then tail should also be NULL in all cases but compare both just to make sure ...
		return 0;
	}

	// first element in the list

	list->head = elem;
	list->tail = elem;

	elem->prev = NULL;
	elem->next = NULL;

	return 1;
}

static inline int _list_insert_before(list_t* list, object_t* elem, object_t* target) {
	list->len++;

	if (_list_insert_check_first(list, elem)) {
		return 0;
	}

	elem->next = target;

	if (!target) { // insert at tail
		elem->prev = list->tail;
		list->tail->next = elem;
		list->tail = elem;
	}

	else {
		elem->prev = target->prev;
		target->prev = elem;
	}

	return 0;
}

static inline int _list_insert_after(list_t* list, object_t* elem, object_t* target) {
	list->len++;

	if (_list_insert_check_first(list, elem)) {
		return 0;
	}

	elem->prev = target;

	if (!target) { // insert at head
		elem->next = list->head;
		list->head->prev = elem;
		list->head = elem;
	}

	else {
		elem->next = target->next;
		target->next = elem;
	}

	return 0;
}

static int list_push(list_t* x, object_t* y) {
	if (!y) {
		return -1; // NULL can't be an element
	}

	return _list_insert_before(x, y, NULL);
}

static inline int _list_detach(list_t* list, object_t* elem) {
	if (!list->head || !list->tail) { // something is wrong here; elem clearly isn't part of list (cf. second TODO at the top of this file)
		return -1;
	}

	list->len--;

	// was the element at the head or tail of the list?
	// if not, stitch the previous/next element to the next/previous element in the list

	if (list->tail == elem) {
		list->tail = elem->prev;
	}

	else {
		elem->next->prev = elem->prev;
	}

	if (list->head == elem) {
		list->head = elem->next;
	}

	else {
		elem->prev->next = elem->next;
	}

	return 0;
}

static object_t* list_pop(list_t* x) {
	object_t* elem = x->tail;

	if (_list_detach(x, elem) < 0) {
		return NULL;
	}

	return elem;
}

// type object itself

static type_t list_type = {
	.name = "list",

	// unary operators

	.len = (void*) list_len,
	.print = (void*) list_print,
	.del = (void*) list_del,

	// binary operators

	.eq = (void*) list_eq,

	// indexing operators

	.iget = (void*) list_iget,
	.iset = (void*) list_iset,

	// list-like type operators

	.push = (void*) list_push,
	.pop = (void*) list_pop,
};

#endif