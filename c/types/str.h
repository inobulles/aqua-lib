#if !defined(__AQUA_LIB__TYPES_STR)
#define __AQUA_LIB__TYPES_STR

#include <types.h>
#include <string.h> // for cstr manipulation functions

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

static void str_del(str_t* str) {
	if (!str->cstr) {
		return;
	}
	
	free(str->cstr);
}

// binary operators

static unsigned str_eq(str_t* x, str_t* y) {
	if (y->obj.type != x->obj.type) {
		return 0;
	}

	return strcmp(x->cstr, y->cstr) == 0;
}

// type object itself

static type_t str_type = {
	.name = "str",
	
	// unary operators

	.len = str_len,
	.del = str_del,

	// binary operators

	.eq = str_eq,
};

#endif