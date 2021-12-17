#include <types/stack.h>

#if !defined(__AQUA_LIB__TYPES_STR)
#define __AQUA_LIB__TYPES_STR

typedef struct {
	object_t obj;
	
	uint64_t len;
	char* cstr;
} str_t;

static type_t str_type; // forward declaration

static str_t* str_new(const char* cstr) {
	if (!cstr) {
		return NULL;
	}
	
	str_t* str = calloc(1, sizeof *str);
	str->obj.type = &str_type;

	str->len = strlen(cstr);
	str->cstr = strdup(cstr);

	return str;
}

// unary operators

static uint64_t str_len(str_t* str) {
	return str->len;
}

static void str_print(str_t* str) {
	printf("%s", str->cstr);
}

static void str_del(str_t* str) {
	if (!str->cstr) {
		return;
	}
	
	free(str->cstr);
}

// binary operators

static unsigned str_eq(str_t* x, str_t* y) {
	if (y->obj.type != &str_type) {
		return 0; // comparing to invalid type
	}

	return strcmp(x->cstr, y->cstr) == 0;
}

static str_t* str_add(str_t* x, str_t* y) {
	if (y->obj.type != &str_type) {
		return NULL; // adding invalid type
	}

	str_t* str = calloc(1, sizeof *str);
	str->obj.type = &str_type;
	
	str->len = x->len + y->len;
	str->cstr = malloc(str->len + 1);

	memcpy(str->cstr, x->cstr, x->len);
	memcpy(str->cstr + x->len, y->cstr, y->len);

	str->cstr[str->len] = 0;

	return str;
}

static str_t* str_mul(str_t* x, int64_t fac) {
	if (fac < 0) {
		return NULL; // cannot multiply a string a negative amount of times
	}

	str_t* str = calloc(1, sizeof *str);
	str->obj.type = &str_type;

	str->len = x->len * fac;
	str->cstr = malloc(str->len + 1);

	for (int i = 0; i < fac; i++) {
		memcpy(str->cstr + x->len * i, x->cstr, x->len);
	}

	str->cstr[str->len] = 0;

	return str;
}

// string-like type operators

static stack_t* str_split(str_t* x, str_t* delim) {
	if (delim->obj.type != &str_type) {
		return NULL; // splitting with invalid delimiter
	}

	stack_t* stack = calloc(1, sizeof *stack);
	stack->obj.type = &stack_type;

	unsigned p = 0;

	for (int i = 0; i < x->len + delim->len + 1; i++) {
		if (i > x->len - delim->len) {
			i += delim->len;
			goto last;
		}

		if (strncmp(x->cstr + i, delim->cstr, delim->len)) {
			continue;
		}

		str_t* str;

	last:

		str = calloc(1, sizeof *str);
		str->obj.type = &str_type;

		str->len = i - p;
		str->cstr = malloc(str->len + 1);

		memcpy(str->cstr, x->cstr + p, str->len);

		str->cstr[str->len] = 0;
		push(stack, str);

		i += delim->len - 1;
		p = i + 1;
	}

	return stack;
}

// type object itself

static type_t str_type = {
	.name = "str",
	
	// unary operators

	.len = (void*) str_len,
	.print = (void*) str_print,
	.del = (void*) str_del,

	// binary operators

	.eq = (void*) str_eq,
	.add = (void*) str_add,
	.mul = (void*) str_mul,

	// string-like type operators

	.split = (void*) str_split,
};

#endif