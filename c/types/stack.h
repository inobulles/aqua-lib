#include <types.h>

#if !defined(__AQUA_LIB__TYPES_STACK)
#define __AQUA_LIB__TYPES_STACK

#include <types/stack_t.h>

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

	stack_t* stack = batch_alloc(&stack_type);

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
		_print(stack->elems[i]);
		printf(">");
	}

	printf("]");
}

static void stack_del(stack_t* stack) {
	if (!stack->elems) {
		return;
	}

	free(stack->elems);
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

	stack_t* stack = batch_alloc(&stack_type);

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

	stack_t* stack = batch_alloc(&stack_type);

	stack->len = x->len * fac;
	stack->elems = malloc(stack->len * sizeof *stack->elems);

	for (int i = 0; i < fac; i++) {
		memcpy(stack->elems + x->len * i, x->elems, x->len * sizeof *stack->elems);
	}

	return stack;
}

// indexing operators

static inline int64_t stack_process_index(stack_t* x, int64_t index) {
	index += x->len * (index < 0); // wrap negative values
	
	if (index < 0 || index >= x->len) {
		return -1; // index out of bounds
	}

	return index;
}

static object_t* stack_iget(stack_t* x, int64_t index) {
	index = stack_process_index(x, index);

	if (index < 0) {
		return NULL; // invalid index
	}

	return x->elems[index];
}

static int stack_iset(stack_t* x, int64_t index, object_t* y) {
	if (!y) {
		return -1; // NULL can't be an element
	}

	index = stack_process_index(x, index);

	if (index < 0) {
		return -1; // invalid index
	}

	x->elems[index] = y;
	return 0;
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
	if (!x->len) {
		return NULL; // no more elements left to pop
	}

	object_t* res = x->elems[--x->len];
	x->elems = realloc(x->elems, x->len * sizeof *x->elems);

	return res;
}

// type object itself

static type_t stack_type = {
	.name = "stack",
	.size = sizeof(stack_t),

	// unary operators

	.len = (void*) stack_len,
	.print = (void*) stack_print,
	.del = (void*) stack_del,

	// binary operators

	.eq = (void*) stack_eq,
	.add = (void*) stack_add,
	.mul = (void*) stack_mul,

	// indexing operators

	.iget = (void*) stack_iget,
	.iset = (void*) stack_iset,

	// list-like type operators

	.push = (void*) stack_push,
	.pop = (void*) stack_pop,
};

#endif