#include "symbol.h"
#include "../arCompiler.h"
#include "../arMemory.h"

DEFINE_LIST_BASE_RELEASE_ONLY(arC_inherits_from, arC_type, 2, 2);
DEFINE_LIST_FIND(arC_inherits_from, arC_type, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(arC_inherited_by, arC_type, 2, 2);
DEFINE_LIST_FIND(arC_inherited_by, arC_type, header.name);

void arCompilerSymbol_init(arCompilerSymbolHeader* s, arC_symbol_type type)
{
	s->type = type;
	arString_zero(&s->name);
	s->initialized = FALSE;
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

	if (t1->type == arC_SYMBOL_UNRESOLVED || t2->type == arC_SYMBOL_UNRESOLVED) {
		assert(false && "Not implemented");
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

void arC_type_type(arC_type* p)
{
	arC_inherited_by_release(&p->inherited_by);
	arC_inherits_from_release(&p->inherits_from);
	arFree(p);
}

arB_type* arC_type_resolve_type(arC_type* t, struct arCompiler* c)
{
	if (t->type != NULL)
		return t->type;

	// Make sure to set the type as soon as possible. This will ensure that
	// we exit this function quickly if recursion happen 
	arB_type* const type = arB_type_new(&t->header.name);
	if (type == NULL) {
		// arC_message_out_of_memory(s);
		return NULL;
	}
	t->type = type;
	type->size = t->size;
	type->data_type = t->data_type;
	if (t->of_type != NULL) {
		type->of_type = arC_type_resolve_type(t->of_type, c);
	}
	if (BIT_ISSET(t->flags, arC_TYPE_FLAG_PTR))
		type->flags |= arB_TYPE_FLAGS_PTR;
	if (BIT_ISSET(t->flags, arC_TYPE_FLAG_ARRAY))
		type->flags |= arB_TYPE_FLAGS_ARRAY;
	return type;
}

BOOL arC_type_initialize_type(arC_type* t, struct arCompiler* c)
{
	if (t->header.initialized)
		return TRUE;

	arB_type* type = t->type;
	if (type == NULL) {
		type = arB_type_new(&t->header.name);
		if (type == NULL) {
			// arC_message_out_of_memory(s);
			return FALSE;
		}
		t->type = type;
		type->size = t->size;
		type->data_type = t->data_type;
		if (t->of_type != NULL) {
			// TODO: Handle circular dependencies?
			if (arC_type_initialize_type(t->of_type, c))
				return FALSE;
		}
		if (BIT_ISSET(t->flags, arC_TYPE_FLAG_PTR))
			type->flags |= arB_TYPE_FLAGS_PTR;
		if (BIT_ISSET(t->flags, arC_TYPE_FLAG_ARRAY))
			type->flags |= arB_TYPE_FLAGS_ARRAY;
	}
	
	t->header.initialized = TRUE;
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

	arC_type* type = ptr->types;
	while (type) {
		arC_type* const tail = type->tail;
		arC_type_type(type);
		type = tail;
	}

	arC_func* func = ptr->funcs;
	while (func) {
		arC_func* const tail = func->tail;
		arC_func_destroy(func);
		func = tail;
	}
	arFree(ptr);
}

arB_package* arC_package_resolve_package(arC_package* p, struct arCompiler* c)
{
	if (p->package != NULL)
		return p->package;

	arB_package* package = arB_package_new(&p->header.name);
	if (package == NULL) {
		// arC_message_out_of_memory(s);
		return NULL;
	}
	p->package = package;

	// Resolve types
	arC_type* type = p->types;
	while (type) {
		arB_type* const t = arC_type_resolve_type(type, c);
		if (t == NULL) {
			// arC_message_out_of_memory(s);
			return NULL;
		}
		if (arB_package_add_type(p->package, t) != VMP_LIST_ADDED) {
			// arC_message_out_of_memory(s);
			return NULL;
		}
		type = type->tail;
	}

	// Resolve functions
	arC_func* func = p->funcs;
	while (func) {
		arB_func* f = arC_func_resolve_func(func, c);
		if (f == NULL) {
			// arC_message_out_of_memory(s);
			return NULL;
		}
		if (arB_package_add_func(package, f) != VMP_LIST_ADDED) {
			// arC_message_out_of_memory(s);
			return NULL;
		}
		func = func->tail;
	}

	p->header.initialized = TRUE;
	return package;
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

arC_package_add_result arC_package_add_type(arC_package* p, arC_type* t)
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
	return arC_PACKAGE_ADD_ADDED;
}

arC_package_add_result arC_package_add_func(arC_package* p, arC_func* f)
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
	return arC_PACKAGE_ADD_ADDED;
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
	p->syntax_tree = p->syntax_tree_end = NULL;
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

void arC_func_add_syntax_tree(arC_func* f, arC_syntax_tree_node node)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(node);

	if (f->syntax_tree_end == NULL) {
		f->syntax_tree = f->syntax_tree_end = node;
	}
	else {
		f->syntax_tree_end->tail = node;
		node->head = f->syntax_tree_end;
		f->syntax_tree_end = node;
	}
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

arB_func* arC_func_resolve_func(arC_func* f, struct arCompiler* c)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(c);

	if (f->func)
		return f->func;

	arB_func* const func = arB_func_new(&f->header.name);
	if (func == NULL) {
		// arC_message_out_of_memory(s);
		return NULL;
	}
	f->func = func;

	// add arguments
	arC_arg* arg = f->arguments;
	while (arg != NULL) {
		arB_arg* a = arB_func_new_arg(f->func, arC_symbol_resolve_type(arg->type, c));
		if (a == NULL) {
			//vmcd_message_out_of_memory(s);
			return NULL;
		}
		arB_arg_set_name(a, &f->header.name);
		arg->arg = a;
		arg = arg->tail;
	}

	// add return values
	arC_return* ret = f->returns;
	while (ret != NULL) {
		arB_return* r = arB_func_new_return(f->func, arC_symbol_resolve_type(ret->type, c));
		if (r == NULL) {
			//vmcd_message_out_of_memory(s);
			return NULL;
		}
		ret->ret = r;
		ret = ret->tail;
	}

	// add local values
	arC_local* local = f->locals;
	while (local != NULL) {
		arB_local* l = arB_func_new_local(f->func, arC_symbol_resolve_type(local->type, c));
		if (l == NULL) {
			//vmcd_message_out_of_memory(s);
			return NULL;
		}
		local->local = l;
		local = local->tail;
	}

	return f->func;
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

arB_type* arC_symbol_resolve_type(arC_symbol* s, struct arCompiler* c)
{
	ASSERT_NOT_NULL(s);
	ASSERT_NOT_NULL(c);
	
	if (s->type == arC_SYMBOL_TYPE) {
		arC_type* const t = (arC_type*)s;
		return arC_type_resolve_type(t, c);
	}
	
	if (s->type == arC_SYMBOL_UNRESOLVED) {
		// This is a proxy symbol
		// TODO: Add support for trying to resolve an unresolved symbol
	}
	return NULL;
}

arInt32 arC_symbol_get_data_type(arC_symbol* s)
{
	ASSERT_NOT_NULL(s);

	if (s->type == arC_SYMBOL_TYPE) {
		arC_type* const t = (arC_type*)s;
		return t->data_type;
	}

	return ARLANG_PRIMITIVE_UNKNOWN;
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
