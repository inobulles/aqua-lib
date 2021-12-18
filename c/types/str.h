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
	str_t* str = batch_alloc(&str_type);

	if (!cstr) {
		str->len = 0;
		str->cstr = calloc(1, 1);

		return str;
	}

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

	str_t* str = batch_alloc(&str_type);
	
	str->len = x->len + y->len;
	str->cstr = malloc(str->len + 1);

	memcpy(str->cstr, x->cstr, x->len);
	memcpy(str->cstr + x->len, y->cstr, y->len);

	str->cstr[str->len] = '\0';

	return str;
}

static str_t* str_mul(str_t* x, int64_t fac) {
	if (fac < 0) {
		return NULL; // cannot multiply a string a negative amount of times
	}

	str_t* str = batch_alloc(&str_type);

	str->len = x->len * fac;
	str->cstr = malloc(str->len + 1);

	for (int i = 0; i < fac; i++) {
		memcpy(str->cstr + x->len * i, x->cstr, x->len);
	}

	str->cstr[str->len] = '\0';

	return str;
}

// indexing operators
// a few awkward casts here because this isn't really the intended uses for these operators
// TODO perhaps create special operators just for this?
// TODO also see how indexing UTF-8 encoded strings would work
// btw have you ever noticed how the word "awkward" is well... awkward? 🤔
// must be the work of Big Word and the MSM 🧐
// anyway

static inline int64_t str_process_index(str_t* x, int64_t index) {
	index += x->len * (index < 0); // wrap negative values
	
	if (index < 0 || index >= x->len) {
		return -1; // index out of bounds
	}

	return index;
}

static object_t* str_iget(str_t* x, int64_t index) {
	index = str_process_index(x, index);

	if (index < 0) {
		return (object_t*) '\0'; // invalid index
	}

	return (object_t*) (intptr_t) x->cstr[index];
}

static int str_iset(str_t* x, int64_t index, object_t* _y) {
	char y = (char) (intptr_t) _y;
	
	if (!y) {
		return -1; // '\0' can't be a character
	}

	index = str_process_index(x, index);

	if (index < 0) {
		return -1; // invalid index
	}

	x->cstr[index] = y;
	return 0;
}

// list-like type operators
// cf. indexing operators comment

static int str_push(str_t* x, object_t* _y) {
	char y = (char) (intptr_t) _y;

	if (!y) {
		return -1; // NULL can't be an element
	}

	x->cstr = realloc(x->cstr, ++x->len + 1);
	
	x->cstr[x->len - 1] = y;
	x->cstr[x->len] = '\0';

	return 0;
}

static object_t* str_pop(str_t* x) {
	if (!x->len) {
		return NULL; // no more characters left to pop
	}

	x->cstr = realloc(x->cstr, x->len);

	char res = x->cstr[--x->len];
	x->cstr[x->len] = '\0';

	return (object_t*) (intptr_t) res;
}

// string-like type operators

static stack_t* str_split(str_t* x, str_t* delim) {
	if (delim->obj.type != &str_type) {
		return NULL; // splitting with invalid delimiter
	}

	stack_t* stack = batch_alloc(&stack_type);

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

		str = batch_alloc(&str_type);

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
	.size = sizeof(str_t),
	
	// unary operators

	.len = (void*) str_len,
	.print = (void*) str_print,
	.del = (void*) str_del,

	// binary operators

	.eq = (void*) str_eq,
	.add = (void*) str_add,
	.mul = (void*) str_mul,

	// indexing operators

	.iget = (void*) str_iget,
	.iset = (void*) str_iset,

	// list-like type operators

	.push = (void*) str_push,
	.pop = (void*) str_pop,

	// string-like type operators

	.split = (void*) str_split,
};

#endif