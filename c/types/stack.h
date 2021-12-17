#if !defined(__AQUA_LIB__TYPES_STACK)
#define __AQUA_LIB__TYPES_STACK

#include <types.h>

typedef struct {
	object_t obj;

	uint64_t len;
	object_t** elems;
} stack_t;

static type_t stack_type; // forward declaration

static stack_t* stack_new(uint64_t len, void* elems[len]) {
	if (len < 0) {
		return NULL; // invalid length
	}

	for (int i = 0; i < len; i++) {
		if (!elems[i]) {
			return NULL; // NULL can't be an element
		}
	}

	stack_t* stack = calloc(1, sizeof *stack);
	stack->obj.type = &stack_type;

	stack->len = len;

	if (elems) {
		uint64_t bytes = stack->len * sizeof *stack->elems;
		
		stack->elems = malloc(bytes);
		memcpy(stack->elems, elems, bytes);
	}

	return stack;
}

// unary operators

static uint64_t stack_len(stack_t* stack) {
	return stack->len;
}

static void stack_print(stack_t* stack) {
	printf("[");
	
	for (int i = 0; i < stack->len; i++) {
		if (i) {
			printf(", ");
		}

		printf("<%s ", stack->elems[i]->type->name);
		print(stack->elems[i]);
		printf(">");
	}

	printf("]");
}

static void stack_del(stack_t* stack) {
	if (stack->elems) {
		free(stack->elems);
	}

	free(stack);
}

// binary operators

static unsigned stack_eq(stack_t* x, stack_t* y) {
	if (y->obj.type != &stack_type) {
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

static stack_t* stack_add(stack_t* x, stack_t* y) {
	if (y->obj.type != &stack_type) {
		return NULL; // adding invalid type
	}

	stack_t* stack = calloc(1, sizeof *stack);
	stack->obj.type = &stack_type;

	stack->len = x->len + y->len;
	stack->elems = malloc(stack->len * sizeof *stack->elems);

	memcpy(stack->elems, x->elems, x->len * sizeof *stack->elems);
	memcpy(stack->elems + x->len, y->elems, y->len * sizeof *stack->elems);

	return stack;
}

static stack_t* stack_mul(stack_t* x, int64_t fac) {
	if (fac < 0) {
		return NULL; // cannot multiply a stack a negative amount of times
	}

	stack_t* stack = calloc(1, sizeof *stack);
	stack->obj.type = &stack_type;

	stack->len = x->len * fac;
	stack->elems = malloc(stack->len * sizeof *stack->elems);

	for (int i = 0; i < fac; i++) {
		memcpy(stack->elems + x->len * i, x->elems, x->len * sizeof *stack->elems);
	}

	return stack;
}

// list-like type operators

static int stack_push(stack_t* x, object_t* y) {
	if (!y) {
		return -1; // NULL can't be an element
	}

	x->elems = realloc(x->elems, ++x->len * sizeof *x->elems);
	x->elems[x->len - 1] = y;

	return 0;
}

static object_t* stack_pop(stack_t* x) {
	object_t* res = x->elems[--x->len];
	x->elems = realloc(x->elems, x->len * sizeof *x->elems);

	return res;
}

// type object itself

static type_t stack_type = {
	.name = "stack",

	// unary operators

	.len = (void*) stack_len,
	.print = (void*) stack_print,
	.del = (void*) stack_del,

	// binary operators

	.eq = (void*) stack_eq,
	.add = (void*) stack_add,
	.mul = (void*) stack_mul,

	// list-like type operators

	.push = (void*) stack_push,
	.pop = (void*) stack_pop,
};

#endif