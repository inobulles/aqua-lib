#if !defined(__AQUA_LIB__TYPES_STACK_t)
#define __AQUA_LIB__TYPES_STACK_t

typedef struct {
	object_t obj;

	uint64_t len;
	object_t** elems;
} stack_t;

#endif