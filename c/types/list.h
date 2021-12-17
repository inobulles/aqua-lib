// "list" implies a doubly-linked list

#if !defined(__AQUA_LIB__TYPES_LIST)
#define __AQUA_LIB__TYPES_LIST

#include <types.h>

typedef struct list_node_t list_node_t; // forward declaration

struct list_node_t {
	object_t* obj;

	list_node_t* prev;
	list_node_t* next;
};

typedef struct {
	object_t obj;

	uint64_t len;

	list_node_t* head;
	list_node_t* tail;
} list_t;

static type_t list_type; // forward declaration

static inline int _list_insert(list_t* list, int64_t index, object_t* obj) {
	list_node_t* node = calloc(1, sizeof *node);
	node->obj = obj;


}

static list_t* list_new(uint64_t len, void* elems[len]) {
	if (len < 0) {
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
		_list_insert(list, i, elems[i]);
	}

	return list;
}

// unary operators

static uint64_t list_len(list_t* list) {
	return list->len;
}

static void list_print(list_t* list) {
	printf("[");
	
	for (int i = 0; i < list->len; i++) {
		if (i) {
			printf(", ");
		}

		printf("<%s ", list->elems[i]->type->name);
		print(list->elems[i]);
		printf(">");
	}

	printf("]");
}

static void list_del(list_t* list) {
	if (list->elems) {
		free(list->elems);
	}

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

	for (int i = 0; i < x->len; i++) {
		object_t* x_elem = x->elems[i];
		object_t* y_elem = y->elems[i];

		if (!eq(x_elem, y_elem)) {
			return 0; // found one differing element
		}
	}

	return 1; // all elements are equal
}

static list_t* list_add(list_t* x, list_t* y) {
	if (y->obj.type != &list_type) {
		return NULL; // adding invalid type
	}

	list_t* list = calloc(1, sizeof *list);
	list->obj.type = &list_type;

	list->len = x->len + y->len;
	list->elems = malloc(list->len * sizeof *list->elems);

	memcpy(list->elems, x->elems, x->len * sizeof *list->elems);
	memcpy(list->elems + x->len, y->elems, y->len * sizeof *list->elems);

	return list;
}

static list_t* list_mul(list_t* x, int64_t fac) {
	if (fac < 0) {
		return NULL; // cannot multiply a list a negative amount of times
	}

	list_t* list = calloc(1, sizeof *list);
	list->obj.type = &list_type;

	list->len = x->len * fac;
	list->elems = malloc(list->len * sizeof *list->elems);

	for (int i = 0; i < fac; i++) {
		memcpy(list->elems + x->len * i, x->elems, x->len * sizeof *list->elems);
	}

	return list;
}

// indexing operators



static object_t* list_iget(list_t* x, int64_t index) {
	index = list_process_index(x, index);

	if (index < 0) {
		return NULL; // invalid index
	}

	return x->elems[index];
}

static int list_iset(list_t* x, int64_t index, object_t* y) {
	if (!y) {
		return -1; // NULL can't be an element
	}

	index = list_process_index(x, index);

	if (index < 0) {
		return -1; // invalid index
	}

	x->elems[index] = y;
	return 0;
}

// list-like type operators

static inline int64_t list_process_index(list_t* x, int64_t index) {
	index += x->len * (index < 0); // wrap negative values
	
	if (index < 0 || index >= x->len) {
		return -1; // index out of bounds
	}

	return index;
}

static int list_push(list_t* x, object_t* y) {
	if (!y) {
		return -1; // NULL can't be an element
	}

	x->elems = realloc(x->elems, ++x->len * sizeof *x->elems);
	x->elems[x->len - 1] = y;

	return 0;
}

static object_t* list_pop(list_t* x) {
	object_t* res = x->elems[--x->len];
	x->elems = realloc(x->elems, x->len * sizeof *x->elems);

	return res;
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
	.add = (void*) list_add,
	.mul = (void*) list_mul,

	// indexing operators

	.iget = (void*) list_iget,
	.iset = (void*) list_iset,

	// list-like type operators

	.push = (void*) list_push,
	.pop = (void*) list_pop,
};

#endif