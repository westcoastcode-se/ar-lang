#include "symbols.h"
#include "compiler.h"
#include "../vm_debug.h"

void zpp_symbol_init(zpp_symbol_header* s, zpp_symbol_type type)
{
	s->type = type;
	vm_string_zero(&s->name);
	s->initialized = FALSE;
}

BOOL zpp_symbol_has_name(zpp_symbol* s, const vm_string* name)
{
	return vm_string_cmp(&s->name, name);
}

BOOL zpp_symbol_equals(zpp_symbol* t1, zpp_symbol* t2)
{
	ASSERT_NOT_NULL(t1);
	ASSERT_NOT_NULL(t2);

	if (t1 == t2) {
		return TRUE;
	}

	if (t1->type == ZPP_SYMBOL_UNRESOLVED || t2->type == ZPP_SYMBOL_UNRESOLVED) {
		assert(false && "Not implemented");
	}

	return FALSE;
}

const zpp_type_props* zpp_type_props_get(const vm_string* name, vm_uint32 size, vm_uint32 flags, vm_uint8 data_type, zpp_type* of_type)
{
	static zpp_type_props p;
	p.name = *name;
	p.size = size;
	p.flags = flags;
	p.data_type = data_type;
	p.of_type = of_type;
	return &p;
}

zpp_type* zpp_type_new(const vm_string* name)
{
	zpp_type* const p = vm_safe_malloc(zpp_type);
	zpp_symbol_init(ZPP_SYMBOL(p), ZPP_SYMBOL_TYPE);
	p->header.name = *name;
	p->package = NULL;
	p->size = -1;
	p->flags = 0;
	p->data_type = 0;
	p->of_type = NULL; 
	zpp_inherits_from_init(&p->inherits_from);
	zpp_inherited_by_init(&p->inherited_by);
	p->head = p->tail = NULL;
	p->type = NULL;
	return p;
}

zpp_type* zpp_type_new_from_props(const zpp_type_props* props)
{
	zpp_type* const p = vm_safe_malloc(zpp_type);
	zpp_symbol_init(ZPP_SYMBOL(p), ZPP_SYMBOL_TYPE);
	p->header.name = props->name;
	p->package = NULL;
	p->size = props->size;
	p->flags = props->flags;
	p->data_type = props->data_type;
	p->of_type = props->of_type;
	zpp_inherits_from_init(&p->inherits_from);
	zpp_inherited_by_init(&p->inherited_by);
	p->head = p->tail = NULL;
	p->type = NULL;
	return p;
}

void zpp_type_destroy(zpp_type* p)
{
	zpp_inherited_by_release(&p->inherited_by);
	zpp_inherits_from_release(&p->inherits_from);
	vm_free(p);
}

vmp_type* zpp_type_resolve_type(zpp_type* t, struct zpp_compiler* c)
{
	if (t->type != NULL)
		return t->type;

	// Make sure to set the type as soon as possible. This will ensure that
	// we exit this function quickly if recursion happen 
	vmp_type* const type = vmp_type_new(&t->header.name);
	if (type == NULL) {
		// zpp_message_out_of_memory(s);
		return NULL;
	}
	t->type = type;
	type->size = t->size;
	type->data_type = t->data_type;
	if (t->of_type != NULL) {
		type->of_type = zpp_type_resolve_type(t->of_type, c);
	}
	if (BIT_ISSET(t->flags, ZPP_TYPE_FLAG_PTR))
		type->flags |= VMP_TYPE_FLAGS_PTR;
	if (BIT_ISSET(t->flags, ZPP_TYPE_FLAG_ARRAY))
		type->flags |= VMP_TYPE_FLAGS_ARRAY;
	return type;
}

BOOL zpp_type_initialize_type(zpp_type* t, struct zpp_compiler* c)
{
	if (t->header.initialized)
		return TRUE;

	vmp_type* type = t->type;
	if (type == NULL) {
		type = vmp_type_new(&t->header.name);
		if (type == NULL) {
			// zpp_message_out_of_memory(s);
			return FALSE;
		}
		t->type = type;
		type->size = t->size;
		type->data_type = t->data_type;
		if (t->of_type != NULL) {
			// TODO: Handle circular dependencies?
			if (zpp_type_initialize_type(t->of_type, c))
				return FALSE;
		}
		if (BIT_ISSET(t->flags, ZPP_TYPE_FLAG_PTR))
			type->flags |= VMP_TYPE_FLAGS_PTR;
		if (BIT_ISSET(t->flags, ZPP_TYPE_FLAG_ARRAY))
			type->flags |= VMP_TYPE_FLAGS_ARRAY;
	}
	
	t->header.initialized = TRUE;
	return TRUE;
}

zpp_package* zpp_package_new(const vm_string* name)
{
	zpp_package* const p = vm_safe_malloc(zpp_package);
	zpp_symbol_init(ZPP_SYMBOL(p), ZPP_SYMBOL_PACKAGE);
	p->header.name = *name;
	//zpp_imports_init(&p->imports);
	p->parent = NULL;
	p->children = p->children_end = NULL;
	p->types = p->types_end = NULL;
	p->funcs = p->funcs_end = NULL;
	p->head = p->tail = NULL;
	p->package = NULL;
	return p;
}

void zpp_package_destroy(zpp_package* ptr)
{
	zpp_package* package = ptr->children;
	while (package) {
		zpp_package* const tail = package->tail;
		zpp_package_destroy(package);
		package = tail;
	}

	zpp_type* type = ptr->types;
	while (type) {
		zpp_type* const tail = type->tail;
		zpp_type_destroy(type);
		type = tail;
	}

	zpp_func* func = ptr->funcs;
	while (func) {
		zpp_func* const tail = func->tail;
		zpp_func_destroy(func);
		func = tail;
	}
	//zpp_imports_release(&ptr->imports);
	vm_free(ptr);
}

vmp_package* zpp_package_resolve_package(zpp_package* p, struct zpp_compiler* c)
{
	if (p->package != NULL)
		return p->package;

	vmp_package* package = vmp_package_new(&p->header.name);
	if (package == NULL) {
		// zpp_message_out_of_memory(s);
		return NULL;
	}
	p->package = package;

	// Resolve types
	zpp_type* type = p->types;
	while (type) {
		vmp_type* const t = zpp_type_resolve_type(type, c);
		if (t == NULL) {
			// zpp_message_out_of_memory(s);
			return NULL;
		}
		if (vmp_package_add_type(p->package, t) != VMP_LIST_ADDED) {
			// zpp_message_out_of_memory(s);
			return NULL;
		}
		type = type->tail;
	}

	// Resolve functions
	zpp_func* func = p->funcs;
	while (func) {
		vmp_func* f = zpp_func_resolve_func(func, c);
		if (f == NULL) {
			// zpp_message_out_of_memory(s);
			return NULL;
		}
		if (vmp_package_add_func(package, f) != VMP_LIST_ADDED) {
			// zpp_message_out_of_memory(s);
			return NULL;
		}
		func = func->tail;
	}

	p->header.initialized = TRUE;
	return package;
}

zpp_func* zpp_package_find_func(zpp_package* p, const vm_string* name)
{
	zpp_func* func = p->funcs;
	while (func) {
		if (vm_string_cmp(&func->header.name, name)) {
			return func;
		}
		func = func->tail;
	}
	return NULL;
}

zpp_type* zpp_package_find_type(zpp_package* p, const vm_string* name)
{
	zpp_type* type = p->types;
	while (type) {
		if (zpp_symbol_has_name(ZPP_SYMBOL(type), name)) {
			return type;
		}
		type = type->tail;
	}
	return NULL;
}

zpp_symbol* zpp_package_find_symbol(zpp_package* p, const vm_string* name)
{
	ASSERT_NOT_NULL(p);
	
	zpp_type* const type = zpp_package_find_type(p, name);
	if (type)
		return ZPP_SYMBOL(type);

	zpp_func* const func = zpp_package_find_func(p, name);
	if (func)
		return ZPP_SYMBOL(func);

	return NULL;
}

zpp_package_add_result zpp_package_add_type(zpp_package* p, zpp_type* t)
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
	return ZPP_PACKAGE_ADD_ADDED;
}

zpp_package_add_result zpp_package_add_func(zpp_package* p, zpp_func* f)
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
	return ZPP_PACKAGE_ADD_ADDED;
}

zpp_func* zpp_func_new(const vm_string* name)
{
	zpp_func* const p = vm_safe_malloc(zpp_func);
	zpp_symbol_init(ZPP_SYMBOL(p), ZPP_SYMBOL_FUNC);
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

void zpp_func_destroy(zpp_func* f)
{
	zpp_argument* arg = f->arguments;
	while (arg) {
		zpp_argument* const tail = arg->tail;
		zpp_argument_destroy(arg);
		arg = tail;
	}

	zpp_return* ret = f->returns;
	while (ret) {
		zpp_return* const tail = ret->tail;
		zpp_return_destroy(ret);
		ret = tail;
	}

	zpp_local* local = f->locals;
	while (local) {
		zpp_local* const tail = local->tail;
		zpp_local_destroy(local);
		local = tail;
	}

	vm_free(f);
}

void zpp_func_add_syntax_tree(zpp_func* f, zpp_syntax_tree_node node)
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

void zpp_func_add_argument(zpp_func* f, zpp_argument* a)
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

void zpp_func_add_return(zpp_func* f, zpp_return* r)
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

void zpp_func_add_local(zpp_func* f, zpp_local* l)
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

vmp_func* zpp_func_resolve_func(zpp_func* f, struct zpp_compiler* c)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(c);

	if (f->func)
		return f->func;

	vmp_func* const func = vmp_func_new(&f->header.name);
	if (func == NULL) {
		// zpp_message_out_of_memory(s);
		return NULL;
	}
	f->func = func;

	// add arguments
	zpp_argument* arg = f->arguments;
	while (arg != NULL) {
		vmp_arg* a = vmp_func_new_arg(f->func, zpp_symbol_resolve_type(arg->type, c));
		if (a == NULL) {
			//vmcd_message_out_of_memory(s);
			return NULL;
		}
		vmp_arg_set_name(a, &f->header.name);
		arg->arg = a;
		arg = arg->tail;
	}

	// add return values
	zpp_return* ret = f->returns;
	while (ret != NULL) {
		vmp_return* r = vmp_func_new_return(f->func, zpp_symbol_resolve_type(ret->type, c));
		if (r == NULL) {
			//vmcd_message_out_of_memory(s);
			return NULL;
		}
		ret->ret = r;
		ret = ret->tail;
	}

	// add local values
	zpp_local* local = f->locals;
	while (local != NULL) {
		vmp_local* l = vmp_func_new_local(f->func, zpp_symbol_resolve_type(local->type, c));
		if (l == NULL) {
			//vmcd_message_out_of_memory(s);
			return NULL;
		}
		local->local = l;
		local = local->tail;
	}

	return f->func;
}

zpp_local* zpp_func_find_local(zpp_func* f, const vm_string* name)
{
	zpp_local* local = f->locals;
	while (local != NULL) {
		if (vm_string_cmp(&local->header.name, name))
			return local;
		local = local->tail;
	}
	return NULL;
}

zpp_argument* zpp_func_find_argument(zpp_func* f, const vm_string* name)
{
	zpp_argument* arg = f->arguments;
	while (arg != NULL) {
		if (vm_string_cmp(&arg->header.name, name))
			return arg;
		arg = arg->tail;
	}
	return NULL;
}

zpp_symbol* zpp_func_find_symbol(zpp_func* f, const vm_string* name)
{
	zpp_local* local = zpp_func_find_local(f, name);
	if (local)
		return ZPP_SYMBOL(local);

	zpp_argument* arg = zpp_func_find_argument(f, name);
	if (arg)
		return ZPP_SYMBOL(arg);

	return NULL;
}

vm_int32 zpp_func_num_arguments(zpp_func* f)
{
	ASSERT_NOT_NULL(f);
	return f->arguments_count;
}

vm_int32 zpp_func_num_returns(zpp_func* f)
{
	ASSERT_NOT_NULL(f);
	return f->returns_count;
}

zpp_argument* zpp_argument_new(const vm_string* name)
{
	zpp_argument* p = vm_safe_malloc(zpp_argument);
	zpp_symbol_init(ZPP_SYMBOL(p), ZPP_SYMBOL_ARGUMENT);
	p->header.name = *name;
	p->type = NULL;
	p->head = p->tail = NULL;
	p->arg = NULL;
	return p;
}

void zpp_argument_destroy(zpp_argument* ptr)
{
	vm_free(ptr);
}

zpp_return* zpp_return_new()
{
	zpp_return* p = vm_safe_malloc(zpp_return);
	zpp_symbol_init(ZPP_SYMBOL(p), ZPP_SYMBOL_RETURN);
	p->type = NULL;
	p->head = p->tail = NULL;
	p->ret = NULL;
	return p;
}

void zpp_return_destroy(zpp_return* ptr)
{
	vm_free(ptr);
}

vmp_type* zpp_symbol_resolve_type(zpp_symbol* s, struct zpp_compiler* c)
{
	ASSERT_NOT_NULL(s);
	ASSERT_NOT_NULL(c);
	
	if (s->type == ZPP_SYMBOL_TYPE) {
		zpp_type* const t = (zpp_type*)s;
		return zpp_type_resolve_type(t, c);
	}
	
	if (s->type == ZPP_SYMBOL_UNRESOLVED) {
		// This is a proxy symbol
		// TODO: Add support for trying to resolve an unresolved symbol
	}
	return NULL;
}

vm_int32 zpp_symbol_get_data_type(zpp_symbol* s)
{
	ASSERT_NOT_NULL(s);

	if (s->type == ZPP_SYMBOL_TYPE) {
		zpp_type* const t = (zpp_type*)s;
		return t->data_type;
	}

	return -1;
}

zpp_local* zpp_local_new(const vm_string* name)
{
	zpp_local* p = vm_safe_malloc(zpp_local);
	zpp_symbol_init(ZPP_SYMBOL(p), ZPP_SYMBOL_LOCAL);
	p->header.name = *name;
	p->type = NULL;
	p->head = p->tail = NULL;
	p->local = NULL;
	return p;
}

void zpp_local_destroy(zpp_local* ptr)
{
	vm_free(ptr);
}
