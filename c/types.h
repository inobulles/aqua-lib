#if !defined(__AQUA_LIB__TYPES)
#define __AQUA_LIB__TYPES

// TODO exception handling system (perhaps a global exception queue?)

typedef struct type_t type_t; // forward declaration

typedef struct {
	type_t* type;
} object_t; // abstract "class"

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

	// list-like type operators

	int (*push) (void* _x, void* _y);
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

static void print(void* _x) {
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

#endif