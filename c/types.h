#if !defined(__AQUA_LIB__TYPES)
#define __AQUA_LIB__TYPES

#include <root.h>

// TODO exception handling system (perhaps a global exception queue?)

typedef struct type_t type_t; // forward declaration

typedef struct {
	type_t* type;
} object_t; // abstract "class"

// manatory types

#include <types/stack_t.h>

struct type_t {
	const char* name;

	// unary operators

	uint64_t (*len) (void* _x);
	void (*print) (void* _x);
	void (*del) (void* _x);

	// binary operators
	
	unsigned (*eq) (void* _x, void* _y);
	void* (*add) (void* _x, void* _y);
	void* (*mul) (void* _x, int64_t fac); // XXX this is kinda neither unary nor binary... create a separate section for this?

	// indexing operators

	void* (*iget) (void* _x, int64_t index);
	int (*iset) (void* _x, int64_t index, void* _y);

	// list-like type operators

	int (*push) (void* _x, void* _y);
	void* (*pop) (void* _x);

	// string-like type operators

	stack_t* (*split) (void* _x, void* _delim);
};

#define TYPE_OP_ERROR fprintf(stderr, "[TYPES] ERROR '%s' does not have a '%s' operator\n", x->type->name, __func__);

// unary operator

static uint64_t len(void* _x) {
	object_t* x = _x;

	if (!x) {
		return -1;
	}

	if (!x->type->len) {
		TYPE_OP_ERROR
		return -1;
	}

	return x->type->len(x);
}

static void _print(void* _x) {
	object_t* x = _x;

	if (!x) {
		printf("NULL");
		return;
	}

	if (x->type->print) {
		x->type->print(x);
		return;
	}

	printf("<type %s>", x->type->name);
}

static inline void print(void* _x) {
	_print(_x);
	printf("\n");
}

static void del(void* _x) {
	object_t* x = _x;

	if (!x) {
		return;
	}

	if (x->type->del) {
		x->type->del(x);
	}

	free(x);
}

// binary operators

static unsigned eq(void* _x, void* _y) {
	object_t* x = _x;
	object_t* y = _y;

	if (!x && !y) {
		// two objects are equal if they're both NULL
		return 1;
	}

	if (!x || !y) {
		return 0;
	}

	if (!x->type->eq) {
		TYPE_OP_ERROR
		return 0;
	}

	return x->type->eq(x, y);
}

static void* add(void* _x, void* _y) {
	object_t* x = _x;
	object_t* y = _y;

	if (!x || !y) {
		return NULL;
	}

	if (!x->type->add) {
		TYPE_OP_ERROR
		return NULL;
	}

	return x->type->add(x, y);
}

static void* mul(void* _x, int64_t fac) {
	object_t* x = _x;

	if (!x) {
		return NULL;
	}

	if (!x->type->mul) {
		TYPE_OP_ERROR
		return NULL;
	}
	
	return x->type->mul(x, fac);
}

// indexing operators

static void* iget(void* _x, int64_t index) {
	object_t* x = _x;

	if (!x) {
		return NULL;
	}

	if (!x->type->iget) {
		TYPE_OP_ERROR
		return NULL;
	}

	return x->type->iget(x, index);
}

static int iset(void* _x, int64_t index, void* _y) {
	object_t* x = _x;
	object_t* y = _y;

	if (!x) {
		return -1;
	}

	if (!x->type->iset) {
		TYPE_OP_ERROR
		return -1;
	}

	return x->type->iset(x, index, y);
}

// list-like type operators

static int push(void* _x, void* _y) {
	object_t* x = _x;
	object_t* y = _y;

	if (!x) {
		return -1;
	}

	if (!x->type->push) {
		TYPE_OP_ERROR
		return -1;
	}

	return x->type->push(x, y);
}

static void* pop(void* _x) {
	object_t* x = _x;

	if (!x) {
		return NULL;
	}

	if (!x->type->pop) {
		TYPE_OP_ERROR
		return NULL;
	}

	return x->type->pop(x);
}

// string-like type operators

static stack_t* split(void* _x, void* _delim) {
	object_t* x = _x;
	object_t* delim = _delim;

	if (!x) {
		return NULL;
	}

	if (!x->type->split) {
		TYPE_OP_ERROR
		return NULL;
	}

	return x->type->split(x, delim);
}

// mandatory types

#include <types/stack.h>

#endif