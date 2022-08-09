#include "vmcd_types.h"
#include "vmcd_messages.h"
#include "vmcode.h"

vmcd_type* vmcd_type_new(const vm_string* name)
{
	vmcd_type* const p = vm_safe_malloc(vmcd_type);
	p->header.name = *name;
	p->header.type = VMCD_KEYWORD_TYPE;
	p->package = NULL;
	p->of_type = NULL;
	p->data_type = 0;
	vmcd_list_inherits_from_init(&p->inherits_from);
	vmcd_list_inherited_by_init(&p->inherited_by);
	p->type = NULL;
	return p;
}

void vmcd_type_destroy(vmcd_type* ptr)
{
	vmcd_list_inherited_by_release(&ptr->inherited_by);
	vmcd_list_inherits_from_release(&ptr->inherits_from);
	vm_free(ptr);
}

BOOL vmcd_type_of_type(vmcd_type* t, vmcd_type* of_type)
{
	if (t->of_type != NULL)
		return FALSE;
	t->of_type = of_type;
	return TRUE;
}

vmcd_type* vmcd_type_new_from_props(const vmcd_type_props* props)
{
	vmcd_type* p = vm_safe_malloc(vmcd_type);
	p->header.type = VMCD_KEYWORD_TYPE;
	p->header.name = props->name;
	p->package = NULL;
	p->size = props->size;
	p->flags = props->flags;
	p->data_type = props->data_type;
	p->of_type = props->of_type;
	vmcd_list_inherits_from_init(&p->inherits_from);
	vmcd_list_inherited_by_init(&p->inherited_by);
	p->type = NULL;
	return p;
}

const vmcd_type_props* vmcd_type_props_get(const vm_string* name, vm_uint32 size, vm_uint32 flags, vm_uint8 data_type, vmcd_type* of_type)
{
	static vmcd_type_props p;
	p.name = *name;
	p.size = size;
	p.flags = flags;
	p.data_type = data_type;
	p.of_type = of_type;
	return &p;
}

vmp_type* vmcd_type_resolve_type(vmcd_type* t, vmcode* s)
{
	if (t->type != NULL)
		return t->type;

	// Make sure to set the type as soon as possible. This will ensure that
	// we exit this function quickly if recursion happen 
	vmp_type* type = vmp_type_new(&t->header.name);
	if (type == NULL) {
		vmcd_message_out_of_memory(s);
		return NULL;
	}
	t->type = type;
	type->size = t->size;
	if (t->of_type != NULL)
		type->of_type = vmcd_type_resolve_type(t->of_type, s);
	type->data_type = t->data_type;
	if ((t->flags & VMCD_TYPE_FLAG_PTR) == VMCD_TYPE_FLAG_PTR)
		type->flags |= VMP_TYPE_FLAGS_PTR;
	if ((t->flags & VMCD_TYPE_FLAG_ARRAY) == VMCD_TYPE_FLAG_ARRAY)
		type->flags |= VMP_TYPE_FLAGS_ARRAY;
	return t->type;
}

vmcd_package* vmcd_package_new(const vm_string* name)
{
	vmcd_package* const p = vm_safe_malloc(vmcd_package);
	p->header.name = *name;
	p->header.type = VMCD_KEYWORD_PACKAGE;
	vmcd_list_types_init(&p->types);
	vmcd_list_funcs_init(&p->funcs);
	vmcd_list_imports_init(&p->imports);
	p->package = NULL;
	return p;
}

void vmcd_package_destroy(vmcd_package* ptr)
{
	// This is deleted by the vmp_builder type
	//if (ptr->package) {
	//	vmp_package_destroy(ptr->package);
	ptr->package = NULL;
	//}
	vmcd_list_imports_release(&ptr->imports);
	vmcd_list_funcs_release(&ptr->funcs);
	vmcd_list_types_release(&ptr->types);
	vm_free(ptr);
}

vmcd_func* vmcd_package_find_func(vmcd_package* p, const vm_string* name)
{
	return vmcd_list_funcs_find(&p->funcs, name);
}

vmcd_type* vmcd_package_find_type(vmcd_package* p, const vm_string* name)
{
	return vmcd_list_types_find(&p->types, name);
}

vmcd_type* vmcd_package_find_type_include_imports(vmcd_package* p, const vm_string* name)
{
	vmcd_type* type = vmcd_package_find_type(p, name);
	if (type != NULL)
		return type;
	const vm_int32 count = p->imports.count;
	for (vm_int32 i = 0; i < count; ++i) {
		p = vmcd_list_imports_get(&p->imports, i);
		type = vmcd_package_find_type(p, name);
		if (type != NULL) {
			return type;
		}
	}
	return NULL;
}

vmcd_package* vmcd_package_find_import(vmcd_package* p, const vm_string* name)
{
	return vmcd_list_imports_find(&p->imports, name);
}

vmcd_package_add_result vmcd_package_add_type(vmcd_package* p, vmcd_type* type)
{
	if (vmcd_list_types_add(&p->types, type) == -1)
		return VMCD_PACKAGE_ADD_OUT_OF_MEMORY;
	return VMCD_PACKAGE_ADD_ADDED;
}

vmcd_package_add_result vmcd_package_add_func(vmcd_package* p, vmcd_func* f)
{
	if (vmcd_list_funcs_add(&p->funcs, f) == -1)
		return VMCD_PACKAGE_ADD_OUT_OF_MEMORY;
	return VMCD_PACKAGE_ADD_ADDED;
}

vmcd_package_add_result vmcd_package_add_import(vmcd_package* p, vmcd_package* i)
{
	if (vmcd_list_imports_add(&p->imports, i) == -1)
		return VMCD_PACKAGE_ADD_OUT_OF_MEMORY;
	return VMCD_PACKAGE_ADD_ADDED;
}

BOOL vmcd_package_compile(vmcd_package* p, vmcode* s)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(s);

	// Create a package
	p->package = vmcd_package_resolve_package(p, s);
	if (p->package == NULL)
		return vmcd_message_out_of_memory(s);

	// Compile functions
	const vm_int32 count = p->funcs.count;
	for (vm_int32 i = 0; i < count; ++i) {
		vmcd_func* f = vmcd_list_funcs_get(&p->funcs, i);
		if (!vmcd_func_compile(f, s))
			return FALSE;
	}
	return TRUE;
}

vmp_package* vmcd_package_resolve_package(vmcd_package* p, vmcode* s)
{
	if (p->package != NULL)
		return p->package;
	
	vmp_package* package = vmp_package_new(&p->header.name);
	if (package == NULL) {
		vmcd_message_out_of_memory(s);
		return NULL;
	}
	p->package = package;

	// Resolve types part of this package
	vm_int32 i;
	for (i = 0; i < p->types.count; ++i) {
		vmcd_type* const type = vmcd_list_types_get(&p->types, i);
		vmp_type* const t = vmcd_type_resolve_type(type, s);
		if (t == NULL) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}
		if (vmp_package_add_type(p->package, t) != VMP_LIST_ADDED) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}
	}

	// Resolve functions
	for (i = 0; i < p->funcs.count; ++i) {
		vmcd_func* const func = vmcd_list_funcs_get(&p->funcs, i);
		vmp_func* const f = vmcd_func_resolve_func(func, s);
		if (f == NULL) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}
		if (vmp_package_add_func(p->package, f) != VMP_LIST_ADDED) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}
	}

	return p->package;
}

vmcd_func* vmcd_func_new(const vm_string* name)
{
	vmcd_func* const p = vm_safe_malloc(vmcd_func);
	p->header.name = *name;
	p->header.type = VMCD_KEYWORD_FUNC;
	p->flags = VMCD_FUNC_FLAG_PUBLIC;
	p->args = p->args_end = NULL;
	p->returns = p->returns_end = NULL;
	p->syntax_tree = p->syntax_tree_end = NULL;
	p->func = NULL;
	return p;
}

void vmcd_func_destroy(vmcd_func* ptr)
{
	vmcd_arg* arg = ptr->args;
	while (arg != NULL) {
		vmcd_arg* const arg_tail = arg->tail;
		vmcd_arg_destroy(arg);
		arg = arg_tail;
	}

	vmcd_return* ret = ptr->returns;
	while (ret != NULL) {
		vmcd_return* const ret_tail = ret->tail;
		vmcd_return_destroy(ret);
		ret = ret_tail;
	}

	vmcd_syntax_tree* st = ptr->syntax_tree;
	while (st != NULL) {
		vmcd_syntax_tree* tail = st->tail;
		vmcd_syntax_tree_free(st);
		st = tail;
	}
	vm_free(ptr);
}

void vmcd_func_add_syntax_tree(vmcd_func* f, vmcd_syntax_tree_node node)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(node);

	if (f->syntax_tree_end == NULL) {
		f->syntax_tree = f->syntax_tree_end = node;
		return;
	}

	// Add the node to the end of the syntax tree list
	node->head = f->syntax_tree_end;
	f->syntax_tree_end->tail = node;
	f->syntax_tree_end = node;
}

void vmcd_func_add_arg(vmcd_func* f, vmcd_arg* a)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(a);

	if (f->args_end == NULL) {
		f->args = f->args_end = a;
	}

	a->head = f->args_end;
	f->args_end->tail = a;
	f->args_end = a;
}

void vmcd_func_add_return(vmcd_func* f, vmcd_return* r)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(r);

	if (f->returns_end == NULL) {
		f->returns = f->returns_end = r;
		return;
	}

	r->head = f->returns_end;
	f->returns_end->tail = r;
	f->returns_end = r;
}

BOOL vmcd_func_compile(vmcd_func* f, vmcode* s)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(s);
	ASSERT_NOT_NULL(f->func);

	// The function has no body
	if (f->syntax_tree == NULL) {
		return vmcd_message_function_no_body(s, &f->header.name);
	}

	// Compile each node in the syntax tree
	vmcd_syntax_tree_node node = f->syntax_tree;
	while (node != NULL) {
		if (!vmcd_syntax_tree_compile(node, f)) {
			return FALSE;
		}
		node = node->tail;
	}

	return TRUE;
}

vmp_func* vmcd_func_resolve_func(vmcd_func* f, vmcode* s)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(s);

	if (f->func != NULL)
		return f->func;

	f->func = vmp_func_new(&f->header.name);
	if (f->func == NULL) {
		vmcd_message_out_of_memory(s);
		return NULL;
	}

	// add arguments
	vmcd_arg* arg = f->args;
	while (arg != NULL) {
		vmp_arg* a = vmp_func_new_arg(f->func, vmcd_keyword_resolve_type(arg->type));
		if (a == NULL) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}
		vmp_arg_set_name(a, &f->header.name);
		arg = arg->tail;
	}

	// add return values
	vmcd_return* ret = f->returns;
	while (ret != NULL) {
		vmp_return* r = vmp_func_new_return(f->func, vmcd_keyword_resolve_type(ret->type));
		if (r == NULL) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}
		ret = ret->tail;
	}
	return f->func;
}

vmcd_arg* vmcd_arg_new(const vm_string* name)
{
	vmcd_arg* const p = vm_safe_malloc(vmcd_arg);
	p->header.type = VMCD_KEYWORD_ARG;
	p->header.name = *name;
	p->type = NULL;
	p->head = p->tail = NULL;
	p->arg = NULL;
	return p;
}

void vmcd_arg_destroy(vmcd_arg* ptr)
{
	vm_free(ptr);
}

vmcd_return* vmcd_return_new()
{
	vmcd_return* const p = vm_safe_malloc(vmcd_return);
	p->header.type = VMCD_KEYWORD_RETURN;
	vm_string_zero(&p->header.name);
	p->type = NULL;
	p->head = p->tail = NULL;
	p->ret = NULL;
	return p;
}

void vmcd_return_destroy(vmcd_return* ptr)
{
	vm_free(ptr);
}

vmp_type* vmcd_keyword_resolve_type(vmcd_keyword* keyword)
{
	ASSERT_NOT_NULL(keyword);
	if (keyword->type == VMCD_KEYWORD_UNRESOLVED) {
		// Try to resolve this as a type
	}
	else if (keyword->type == VMCD_KEYWORD_TYPE) {
		// This is already considered a type. 
		vmcd_type* const type = (vmcd_type*)keyword;
		if (type->type == NULL) {
			vmp_type* t = vmp_type_new(&type->header.name);
			if (t == NULL) {
				// TODO:
				assert(FALSE && "Implement this");
			}
			type->type = t;
			t->data_type = type->data_type;
		}
		return type->type;
	}
	assert(FALSE && "Implement this");
	return NULL;
}