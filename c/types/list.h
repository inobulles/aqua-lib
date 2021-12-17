#if !defined(__AQUA_LIB__TYPES_LIST)
#define __AQUA_LIB__TYPES_LIST

#include <types.h>

typedef struct {
	object_t obj;

	uint64_t len;
	object_t** elems;
} list_t;

static type_t list_type; // forward declaration

static list_t* list_new(uint64_t len, void* elems[len]) {
	if (len < 0) {
		return NULL; // invalid length
	}
	
	list_t* list = calloc(1, sizeof *list);
	list->obj.type = &list_type;

	list->len = len;

	if (elems) {
		uint64_t bytes = list->len * sizeof *list->elems;
		
		list->elems = malloc(bytes);
		memcpy(list->elems, elems, bytes);
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
		return NULL; // cannot multiply a list a negative amount of tiems
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
};

#endif