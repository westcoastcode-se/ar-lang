#ifndef _ARLANG_LIST_H_
#define _ARLANG_LIST_H_

#include "arString.h"
#include "arMemory.h"

#define DECLARE_LIST_TYPE(NAME, TYPE) \
	typedef struct NAME { \
		arInt32 count; \
		arInt32 capacity; \
		struct TYPE** memory; \
	} NAME;\
	ARLANG_API BOOL NAME##_init(NAME* l); \
	ARLANG_API void NAME##_release(NAME* l); \
	ARLANG_API arInt32 NAME##_add(NAME* l, struct TYPE* ptr);\
	ARLANG_API struct TYPE* NAME##_get(const NAME* l, arInt32 index);\
	ARLANG_API struct TYPE* NAME##_first(const NAME* l);\
	ARLANG_API void NAME##_set(const NAME* l, arInt32 index, struct TYPE* ptr);\
	ARLANG_API BOOL NAME##_contains(const NAME* l, const struct TYPE* ptr)

#define DECLARE_LIST_TYPE_FIND(NAME, TYPE) \
	ARLANG_API struct TYPE* NAME##_find(const NAME* l, const arString* s)

#define DEFINE_LIST_INIT(NAME, TYPE, CAPACITY) \
	BOOL NAME##_init(NAME* l) \
	{ \
		l->count = 0; \
		l->capacity = CAPACITY; \
		l->memory = (TYPE**)arMalloc(sizeof(TYPE*) * l->capacity); \
		return l->memory != NULL; \
	}

#define DEFINE_LIST_RELEASE(NAME, TYPE) \
	void NAME##_release(NAME* l) \
	{ \
		arFree(l->memory); \
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
		arFree(l->memory); \
		l->memory = NULL; \
		l->capacity = 0; \
		l->count = 0; \
	}

#define DEFINE_LIST_ADD(NAME, TYPE, RESIZE) \
	arInt32 NAME##_add(NAME* l, struct TYPE* ptr) \
	{ \
		if (l->count >= l->capacity) { \
			l->capacity += RESIZE; \
			l->memory = (struct TYPE**)arRealloc(l->memory, sizeof(struct TYPE*) * l->capacity); \
			if (l->memory == NULL) \
				return -1; \
		} \
		l->memory[l->count] = ptr; \
		return l->count++; \
	}

#define DEFINE_LIST_GET(NAME, TYPE) \
	struct TYPE* NAME##_get(const NAME* l, arInt32 index) \
	{ \
		assert(l->count > index); \
		return l->memory[index]; \
	}

#define DEFINE_LIST_SET(NAME, TYPE) \
	void NAME##_set(const NAME* l, arInt32 index, struct TYPE* ptr) \
	{ \
		assert(l->count > index);\
		l->memory[index] = ptr; \
	}

#define DEFINE_LIST_FIND(NAME, TYPE, PROPERTY_NAME) \
	struct TYPE* NAME##_find(const NAME* l, const arString* s) \
	{ \
		struct TYPE** item = &l->memory[0]; \
		struct TYPE** const end = &l->memory[l->count]; \
		for (; item != end; ++item) { \
			struct TYPE* p = *item; \
			if (arString_cmp((arString*)&p->PROPERTY_NAME, s)) { \
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

#define DEFINE_LIST_FIRST(NAME, TYPE) \
	struct TYPE* NAME##_first(const NAME* l) \
	{ \
		assert(l->count > 0); \
		return l->memory[0]; \
	}

#define DEFINE_LIST_BASE_DEFAULT(NAME, TYPE, CAPACITY, RESIZE) \
	DEFINE_LIST_INIT(NAME, TYPE, CAPACITY) \
	DEFINE_LIST_RELEASE_DESTROY(NAME, TYPE) \
	DEFINE_LIST_ADD(NAME, TYPE, RESIZE) \
	DEFINE_LIST_GET(NAME, TYPE) \
	DEFINE_LIST_FIRST(NAME, TYPE) \
	DEFINE_LIST_SET(NAME, TYPE) \
	DEFINE_LIST_CONTAINS(NAME, TYPE)

#define DEFINE_LIST_BASE_RELEASE_ONLY(NAME, TYPE, CAPACITY, RESIZE) \
	DEFINE_LIST_INIT(NAME, TYPE, CAPACITY) \
	DEFINE_LIST_RELEASE(NAME, TYPE) \
	DEFINE_LIST_ADD(NAME, TYPE, RESIZE) \
	DEFINE_LIST_GET(NAME, TYPE) \
	DEFINE_LIST_FIRST(NAME, TYPE) \
	DEFINE_LIST_SET(NAME, TYPE) \
	DEFINE_LIST_CONTAINS(NAME, TYPE)

#endif
