#include "symbol.h"
#include "messages.h"
#include "../arCompiler.h"
#include "../arMemory.h"

DEFINE_LIST_BASE_RELEASE_ONLY(arC_inherits_from, arC_type, 2, 2);
DEFINE_LIST_FIND(arC_inherits_from, arC_type, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(arC_inherited_by, arC_type, 2, 2);
DEFINE_LIST_FIND(arC_inherited_by, arC_type, header.name);

void arCompilerSymbol_init(arC_symbol* s, arC_symbol_type type)
{
	s->type = type;
	arString_zero(&s->name);
}

BOOL arC_symbol_has_name(arC_symbol* s, const arString* name)
{
	return arString_cmp(&s->name, name);
}

BOOL arC_symbol_equals(arC_symbol* t1, arC_symbol* t2)
{
	ASSERT_NOT_NULL(t1);
	ASSERT_NOT_NULL(t2);

	if (t1 == t2) {
		return TRUE;
	}
	return FALSE;
}

const arC_type_props* arC_type_props_get(const arString* name, arUint32 size, arUint32 flags, arUint8 data_type, arC_type* of_type)
{
	static arC_type_props p;
	p.name = *name;
	p.size = size;
	p.flags = flags;
	p.data_type = data_type;
	p.of_type = of_type;
	return &p;
}

arC_type* arC_type_new(const arString* name)
{
	arC_type* const p = arSafeMalloc(arC_type);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_TYPE);
	p->header.name = *name;
	p->package = NULL;
	p->size = -1;
	p->flags = 0;
	p->data_type = 0;
	p->of_type = NULL; 
	arC_inherits_from_init(&p->inherits_from);
	arC_inherited_by_init(&p->inherited_by);
	p->head = p->tail = NULL;
	p->type = NULL;
	return p;
}

arC_type* arC_type_from_props(const arC_type_props* props)
{
	arC_type* const p = arSafeMalloc(arC_type);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_TYPE);
	p->header.name = props->name;
	p->package = NULL;
	p->size = props->size;
	p->flags = props->flags;
	p->data_type = props->data_type;
	p->of_type = props->of_type;
	arC_inherits_from_init(&p->inherits_from);
	arC_inherited_by_init(&p->inherited_by);
	p->head = p->tail = NULL;
	p->type = NULL;
	return p;
}

void arC_type_destroy(arC_type* p)
{
	arC_inherited_by_release(&p->inherited_by);
	arC_inherits_from_release(&p->inherits_from);
	arFree(p);
}

BOOL arC_type_resolve_type0(arC_type* t, arC_type* root_type, const struct arC_state* s)
{
	if (t == root_type) {
		// TODO: Circular dependencies message error
		return FALSE;
	}

	if (t->type != NULL)
		return TRUE;

	arB_type* const type = arB_type_new(&t->header.name);
	if (type == NULL) {
		// TODO: arC_message_out_of_memory(s);
		return FALSE;
	}
	t->type = type;
	type->size = t->size;
	type->data_type = t->data_type;
	if (BIT_ISSET(t->flags, arC_TYPE_FLAG_PTR))
		type->flags |= arB_TYPE_FLAGS_PTR;
	if (BIT_ISSET(t->flags, arC_TYPE_FLAG_ARRAY))
		type->flags |= arB_TYPE_FLAGS_ARRAY;

	if (t->of_type != NULL) {
		if (!arC_type_resolve_type0(t->of_type, root_type, s))
			return FALSE;
	}
	ASSERT_NOT_NULL(t->package->package);
	if (arB_package_add_type(t->package->package, type) != VMP_LIST_ADDED)
		return arC_message_out_of_memory(s);
	return TRUE;
}

BOOL arC_type_resolve(arC_type* t, const arC_state* s)
{
	if (t->type != NULL)
		return TRUE;

	arB_type* const type = arB_type_new(&t->header.name);
	if (type == NULL) {
		// TODO: arC_message_out_of_memory(s);
		return FALSE;
	}
	t->type = type;
	type->size = t->size;
	type->data_type = t->data_type;
	if (BIT_ISSET(t->flags, arC_TYPE_FLAG_PTR))
		type->flags |= arB_TYPE_FLAGS_PTR;
	if (BIT_ISSET(t->flags, arC_TYPE_FLAG_ARRAY))
		type->flags |= arB_TYPE_FLAGS_ARRAY;

	if (t->of_type != NULL) {
		if (!arC_type_resolve_type0(t->of_type, t, s))
			return FALSE;
	}
	ASSERT_NOT_NULL(t->package->package);
	if (arB_package_add_type(t->package->package, type) != VMP_LIST_ADDED)
		return arC_message_out_of_memory(s);
	return TRUE;
}

arC_package* arC_package_new(const arString* name)
{
	arC_package* const p = arSafeMalloc(arC_package);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_PACKAGE);
	p->header.name = *name;
	p->parent = NULL;
	p->children = p->children_end = NULL;
	p->types = p->types_end = NULL;
	p->funcs = p->funcs_end = NULL;
	p->head = p->tail = NULL;
	p->package = NULL;
	return p;
}

void arC_package_destroy(arC_package* ptr)
{
	arC_package* package = ptr->children;
	while (package) {
		arC_package* const tail = package->tail;
		arC_package_destroy(package);
		package = tail;
	}
	ptr->children = ptr->children_end = NULL;

	arC_type* type = ptr->types;
	while (type) {
		arC_type* const tail = type->tail;
		arC_type_destroy(type);
		type = tail;
	}
	ptr->types = ptr->types_end = NULL;

	arC_func* func = ptr->funcs;
	while (func) {
		arC_func* const tail = func->tail;
		arC_func_destroy(func);
		func = tail;
	}
	ptr->funcs = ptr->funcs_end = NULL;

	arFree(ptr);
}

BOOL arC_package_resolve(arC_package* p, const arC_state* s)
{
	if (p->package != NULL)
		return TRUE;

	arB_package* const package = arB_package_new(&p->header.name);
	if (package == NULL)
		return arC_message_out_of_memory(s);
	p->package = package;

	// Resolve types	
	arC_type* type = p->types;
	while (type) {
		if (!arC_type_resolve(type, s))
			return FALSE;
		type = type->tail;
	}

	// Resolve functions
	arC_func* func = p->funcs;
	while (func) {
		if (!arC_func_resolve(func, s))
			return FALSE;
		func = func->tail;
	}

	return TRUE;
}

arC_func* arC_package_find_func(arC_package* p, const arString* name)
{
	arC_func* func = p->funcs;
	while (func) {
		if (arString_cmp(&func->header.name, name)) {
			return func;
		}
		func = func->tail;
	}
	return NULL;
}

arC_type* arC_package_find_type(arC_package* p, const arString* name)
{
	arC_type* type = p->types;
	while (type) {
		if (arC_symbol_has_name(asC_symbol(type), name)) {
			return type;
		}
		type = type->tail;
	}
	return NULL;
}

arC_symbol* arC_package_find_symbol(arC_package* p, const arString* name)
{
	ASSERT_NOT_NULL(p);
	
	arC_type* const type = arC_package_find_type(p, name);
	if (type)
		return asC_symbol(type);

	arC_func* const func = arC_package_find_func(p, name);
	if (func)
		return asC_symbol(func);

	return NULL;
}

void arC_package_add_type(arC_package* p, arC_type* t)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(t);
	assert(t->package == NULL && "Expected the type to not be added to another package");

	t->package = p;
	if (p->types_end == NULL) {
		p->types = p->types_end = t;
	}
	else {
		p->types_end->tail = t;
		t->head = p->types_end;
		p->types_end = t;
	}
}

void arC_package_add_func(arC_package* p, arC_func* f)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(f);
	assert(f->package == NULL && "Expected the function to not be added to another package");

	f->package = p;
	if (p->funcs_end == NULL) {
		p->funcs = p->funcs_end = f;
	}
	else {
		p->funcs_end->tail = f;
		f->head = p->funcs_end;
		p->funcs_end = f;
	}
}

void arC_package_add_package(arC_package* p, arC_package* sub_package)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(sub_package);
	assert(sub_package->parent == NULL && "Expected the function to not be added to another package");

	sub_package->parent = p;
	if (sub_package->children_end == NULL) {
		p->children = p->children_end = sub_package;
	}
	else {
		p->children_end->tail = sub_package;
		sub_package->head = p->children_end;
		p->children_end = sub_package;
	}
}

arC_func* arC_func_new(const arString* name)
{
	arC_func* const p = arSafeMalloc(arC_func);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_FUNC);
	p->header.name = *name;
	p->package = NULL;
	p->arguments = p->arguments_end = NULL;
	p->arguments_count = 0;
	p->returns = p->returns_end = NULL;
	p->returns_count = 0;
	p->locals = p->locals_end = NULL;
	p->tail = p->head = NULL;
	p->func = NULL;
	return p;
}

void arC_func_destroy(arC_func* f)
{
	arC_arg* arg = f->arguments;
	while (arg) {
		arC_arg* const tail = arg->tail;
		arC_arg_destroy(arg);
		arg = tail;
	}

	arC_return* ret = f->returns;
	while (ret) {
		arC_return* const tail = ret->tail;
		arC_return_destroy(ret);
		ret = tail;
	}

	arC_local* local = f->locals;
	while (local) {
		arC_local* const tail = local->tail;
		arC_local_destroy(local);
		local = tail;
	}

	arFree(f);
}

void arC_func_add_arg(arC_func* f, arC_arg* a)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(a);

	if (f->arguments == NULL) {
		f->arguments = f->arguments_end = a;
	}
	else {
		f->arguments_end->tail = a;
		a->head = f->arguments_end;
		f->arguments_end = a;
	}
	f->arguments_count++;
}

void arC_func_add_return(arC_func* f, arC_return* r)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(r);

	if (f->returns == NULL) {
		f->returns = f->returns_end = r;
	}
	else {
		f->returns_end->tail = r;
		r->head = f->returns_end;
		f->returns_end = r;
	}
	f->returns_count++;
}

void arC_func_add_local(arC_func* f, arC_local* l)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(l);

	if (f->locals == NULL) {
		f->locals = f->locals_end = l;
	}
	else {
		f->locals_end->tail = l;
		l->head = f->locals_end;
		f->locals_end = l;
	}
}

BOOL arC_func_resolve(arC_func* f, const arC_state* s)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(s);

	if (f->func)
		return TRUE;

	arB_func* const func = arB_func_new(asC_symbol_name(f));
	if (func == NULL)
		return arC_message_out_of_memory(s);
	f->func = func;

	// add arguments
	arC_arg* arg = f->arguments;
	while (arg != NULL) {
		if (!arC_type_resolve(arg->type, s))
			return FALSE;
		arg->arg = arB_func_new_arg(f->func, arg->type->type);
		arB_arg_set_name(arg->arg, asC_symbol_name(arg));
		arg = arg->tail;
	}

	// add return values
	arC_return* ret = f->returns;
	while (ret != NULL) {
		if (!arC_type_resolve(ret->type, s))
			return FALSE;
		ret->ret = arB_func_new_return(f->func, ret->type->type);
		ret = ret->tail;
	}

	// add local values
	arC_local* local = f->locals;
	while (local != NULL) {
		if (!arC_type_resolve(local->type, s))
			return FALSE;
		local->local = arB_func_new_local(f->func, local->type->type);
		arB_local_set_name(local->local, asC_symbol_name(local));
		local = local->tail;
	}
	ASSERT_NOT_NULL(f->package->package);
	if (arB_package_add_func(f->package->package, func) != VMP_LIST_ADDED)
		return arC_message_out_of_memory(s);
	return TRUE;
}

arC_local* arC_func_find_local(arC_func* f, const arString* name)
{
	arC_local* local = f->locals;
	while (local != NULL) {
		if (arString_cmp(&local->header.name, name))
			return local;
		local = local->tail;
	}
	return NULL;
}

arC_arg* arC_func_find_arg(arC_func* f, const arString* name)
{
	arC_arg* arg = f->arguments;
	while (arg != NULL) {
		if (arString_cmp(&arg->header.name, name))
			return arg;
		arg = arg->tail;
	}
	return NULL;
}

arC_symbol* arC_func_find_symbol(arC_func* f, const arString* name)
{
	arC_local* local = arC_func_find_local(f, name);
	if (local)
		return asC_symbol(local);

	arC_arg* arg = arC_func_find_arg(f, name);
	if (arg)
		return asC_symbol(arg);

	return NULL;
}

arInt32 arC_func_count_args(arC_func* f)
{
	ASSERT_NOT_NULL(f);
	return f->arguments_count;
}

arInt32 arC_func_count_returns(arC_func* f)
{
	ASSERT_NOT_NULL(f);
	return f->returns_count;
}

arC_arg* arC_arg_new(const arString* name)
{
	arC_arg* p = arSafeMalloc(arC_arg);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_ARGUMENT);
	p->header.name = *name;
	p->type = NULL;
	p->head = p->tail = NULL;
	p->arg = NULL;
	return p;
}

void arC_arg_destroy(arC_arg* ptr)
{
	arFree(ptr);
}

arC_return* arC_return_new()
{
	arC_return* p = arSafeMalloc(arC_return);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_RETURN);
	p->type = NULL;
	p->head = p->tail = NULL;
	p->ret = NULL;
	return p;
}

void arC_return_destroy(arC_return* ptr)
{
	arFree(ptr);
}

arC_local* arC_local_new(const arString* name)
{
	arC_local* p = arSafeMalloc(arC_local);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_LOCAL);
	p->header.name = *name;
	p->type = NULL;
	p->head = p->tail = NULL;
	p->local = NULL;
	return p;
}

void arC_local_destroy(arC_local* ptr)
{
	arFree(ptr);
}
