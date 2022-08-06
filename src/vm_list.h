#ifndef _VM_LIST_H_
#define _VM_LIST_H_

#include "vm_string.h"
#include "vm_debug.h"

#define DECLARE_LIST_TYPE(NAME, TYPE) \
	typedef struct NAME { \
		vm_int32 count; \
		vm_int32 capacity; \
		struct TYPE** memory; \
	} NAME;\
	extern BOOL NAME##_init(NAME* l); \
	extern void NAME##_release(NAME* l); \
	extern vm_int32 NAME##_add(NAME* l, struct TYPE* ptr);\
	extern struct TYPE* NAME##_get(const NAME* l, vm_int32 index);\
	extern void NAME##_set(const NAME* l, vm_int32 index, struct TYPE* ptr);\
	extern BOOL NAME##_contains(const NAME* l, const struct TYPE* ptr)

#define DECLARE_LIST_TYPE_FIND(NAME, TYPE) \
	extern struct TYPE* NAME##_find(const NAME* l, const vm_string* s)

#define DEFINE_LIST_INIT(NAME, TYPE, CAPACITY) \
	BOOL NAME##_init(NAME* l) \
	{ \
		l->count = 0; \
		l->capacity = CAPACITY; \
		l->memory = (TYPE**)vm_malloc(sizeof(TYPE*) * l->capacity); \
		return l->memory != NULL; \
	}

#define DEFINE_LIST_RELEASE(NAME, TYPE) \
	void NAME##_release(NAME* l) \
	{ \
		vm_free(l->memory); \
		l->memory = NULL; \
		l->capacity = 0; \
		l->count = 0; \
	}

#define DEFINE_LIST_RELEASE_DESTROY(NAME, TYPE) \
	void NAME##_release(NAME* l) \
	{ \
		for (int i = 0; i < l->count; ++i) { \
			TYPE* const p = l->memory[i]; \
			TYPE##_destroy(p); \
		} \
		vm_free(l->memory); \
		l->memory = NULL; \
		l->capacity = 0; \
		l->count = 0; \
	}

#define DEFINE_LIST_ADD(NAME, TYPE, RESIZE) \
	vm_int32 NAME##_add(NAME* l, struct TYPE* ptr) \
	{ \
		if (l->count >= l->capacity) { \
			l->capacity += RESIZE; \
			l->memory = (struct TYPE**)vm_realloc(l->memory, sizeof(struct TYPE*) * l->capacity); \
			if (l->memory == NULL) \
				return -1; \
		} \
		l->memory[l->count] = ptr; \
		return l->count++; \
	}

#define DEFINE_LIST_GET(NAME, TYPE) \
	struct TYPE* NAME##_get(const NAME* l, vm_int32 index) \
	{ \
		assert(l->count > index); \
		return l->memory[index]; \
	}

#define DEFINE_LIST_SET(NAME, TYPE) \
	void NAME##_set(const NAME* l, vm_int32 index, struct TYPE* ptr) \
	{ \
		assert(l->count > index);\
		l->memory[index] = ptr; \
	}

#define DEFINE_LIST_FIND(NAME, TYPE, PROPERTY_NAME) \
	struct TYPE* NAME##_find(const NAME* l, const vm_string* s) \
	{ \
		struct TYPE** item = &l->memory[0]; \
		struct TYPE** const end = &l->memory[l->count]; \
		for (; item != end; ++item) { \
			struct TYPE* p = *item; \
			if (vm_string_cmp((vm_string*)&p->PROPERTY_NAME, s)) { \
				return p; \
			} \
		} \
		return NULL; \
	}

#define DEFINE_LIST_CONTAINS(NAME, TYPE) \
	BOOL NAME##_contains(const NAME* l, const struct TYPE* ptr) \
	{ \
		struct TYPE** item = &l->memory[0]; \
		struct TYPE** const end = &l->memory[l->count]; \
		for (; item != end; ++item) { \
			if (*item == ptr) return TRUE; \
		} \
		return FALSE; \
	}

#define DEFINE_LIST_BASE_DEFAULT(NAME, TYPE, CAPACITY, RESIZE) \
	DEFINE_LIST_INIT(NAME, TYPE, CAPACITY) \
	DEFINE_LIST_RELEASE_DESTROY(NAME, TYPE) \
	DEFINE_LIST_ADD(NAME, TYPE, RESIZE) \
	DEFINE_LIST_GET(NAME, TYPE) \
	DEFINE_LIST_SET(NAME, TYPE) \
	DEFINE_LIST_CONTAINS(NAME, TYPE)

#define DEFINE_LIST_BASE_RELEASE_ONLY(NAME, TYPE, CAPACITY, RESIZE) \
	DEFINE_LIST_INIT(NAME, TYPE, CAPACITY) \
	DEFINE_LIST_RELEASE(NAME, TYPE) \
	DEFINE_LIST_ADD(NAME, TYPE, RESIZE) \
	DEFINE_LIST_GET(NAME, TYPE) \
	DEFINE_LIST_SET(NAME, TYPE) \
	DEFINE_LIST_CONTAINS(NAME, TYPE)

#endif
