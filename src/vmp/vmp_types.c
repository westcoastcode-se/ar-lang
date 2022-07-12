#include "vmp_types.h"
#include "vmp_instr.h"
#include "../vm_debug.h"

vmp_package* vmp_package_new(const vm_string* name)
{
	return vmp_package_newsz(name->start, vm_string_length(name));
}

vmp_package* vmp_package_newsz(const char* name, vm_int32 len)
{
	vmp_package* p = (vmp_package*)vm_malloc(sizeof(vmp_package));
	if (p == NULL)
		return NULL;
	p->header.keyword_type = VMP_KEYWORD_PACKAGE;
	p->pipeline = NULL;
	vm_string_setsz(&p->name, name, len);
	vmp_list_types_init(&p->types);
	vmp_list_funcs_init(&p->funcs);
	vmp_list_imports_init(&p->imports);
	return p;
}

void vmp_package_destroy(vmp_package* p)
{
	vmp_list_imports_release(&p->imports);
	vmp_list_funcs_release(&p->funcs);
	vmp_list_types_release(&p->types);
	vm_free(p);
}

int vmp_package_add_func(vmp_package* p, vmp_func* func)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(func);

	if (func->package != NULL)
		return VMP_LIST_ALREADY_ADDED;
	if (vmp_list_funcs_add(&p->funcs, func) < 0)
		return VMP_LIST_OUT_OF_MEMORY;
	func->package = p;
	return VMP_LIST_ADDED;
}

int vmp_package_add_type(vmp_package* p, vmp_type* type)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(type);

	if (type->package != NULL)
		return VMP_LIST_ALREADY_ADDED;
	if (vmp_list_types_find(&p->types, &type->name) != NULL)
		return VMP_LIST_ALREADY_EXISTS;
	if (vmp_list_types_add(&p->types, type) < 0)
		return VMP_LIST_OUT_OF_MEMORY;
	type->package = p;
	return VMP_LIST_ADDED;
}

int vmp_package_add_import(vmp_package* p, vmp_package* imported)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(imported);

	// TODO: Verify that circular dependencies doesnt exist
	if (vmp_list_imports_add(&p->imports, imported) < 0)
		return VMP_LIST_OUT_OF_MEMORY;
	return VMP_LIST_ADDED;
}

vmp_type* vmp_package_new_type(vmp_package* p, const vm_string* name, vm_uint32 size)
{
	vmp_type* type = vmp_type_new(name);
	type->size = size;
	if (vmp_package_add_type(p, type) == VMP_LIST_ADDED)
		return type;
	return NULL;
}

vmp_type* vmp_package_new_typesz(vmp_package* p, const char* name, int len, vm_uint32 size)
{
	vm_string name_str = { name, name + len };
	return vmp_package_new_type(p, &name_str, size);
}

vmp_type* vmp_package_find_type(vmp_package* p, const vm_string* name)
{
	return vmp_list_types_find(&p->types, name);
}

vmp_func* vmp_package_find_func(vmp_package* p, const vm_string* name)
{
	return vmp_list_funcs_find(&p->funcs, name);
}

vmp_package* vmp_package_find_import(vmp_package* p, const vm_string* name)
{
	return vmp_list_imports_find(&p->imports, name);
}

vmp_type* vmp_package_find_type_include_imports(vmp_package* p, const vm_string* name)
{
	vmp_type* type = vmp_package_find_type(p, name);
	if (type != NULL)
		return type;
	const vm_int32 count = p->imports.count;
	for (vm_int32 i = 0; i < count; ++i) {
		p = vmp_list_imports_get(&p->imports, i);
		type = vmp_package_find_type_include_imports(p, name);
		if (type != NULL) {
			return type;
		}
	}
	return NULL;
}

vmp_keyword* vmp_package_find_keyword(vmp_package* p, const vm_string* name)
{
	vmp_type* const type = vmp_package_find_type(p, name);
	if (type != NULL)
		return &type->header;
	vmp_func* func = vmp_package_find_func(p, name);
	if (func != NULL)
		return &func->header;
	vmp_package* const imports = vmp_package_find_import(p, name);
	if (imports != NULL)
		return &imports->header;
	return NULL;
}

vmp_type* vmp_type_new(const vm_string* name)
{
	vmp_type* p = (vmp_type*)vm_malloc(sizeof(vmp_type));
	if (p == NULL)
		return NULL;
	p->header.keyword_type = VMP_KEYWORD_TYPE;
	p->package = NULL;
	p->name = *name;
	p->size = 0;
	p->flags = 0;
	p->data_type = 0;
	p->of_type = NULL;
	vmp_list_inherits_from_init(&p->inherits_from);
	vmp_list_inherited_by_init(&p->inherited_by);
	return p;
}

vmp_type* vmp_type_new_from_props(const vmp_type_props* props)
{
	vmp_type* p = (vmp_type*)vm_malloc(sizeof(vmp_type));
	if (p == NULL)
		return NULL;
	p->header.keyword_type = VMP_KEYWORD_TYPE;
	p->package = NULL;
	p->name = props->name;
	p->size = props->size;
	p->flags = props->flags;
	p->data_type = props->data_type;
	p->of_type = props->of_type;
	vmp_list_inherits_from_init(&p->inherits_from);
	vmp_list_inherited_by_init(&p->inherited_by);
	return p;
}

const vmp_type_props* vmp_type_props_get(const vm_string* name, vm_uint32 size, vm_uint32 flags, vm_uint8 data_type, const vmp_type* of_type)
{
	static vmp_type_props p;
	p.name = *name;
	p.size = size;
	p.flags = flags;
	p.data_type = data_type;
	p.of_type = of_type;
	return &p;
}

// Mark the supplied type as an of type
BOOL vmp_type_of_type(vmp_type* t, const vmp_type* of_type)
{
	if (t->of_type != NULL) {
		return FALSE;
	}
	t->of_type = of_type;
	return TRUE;
}

void vmp_type_destroy(vmp_type* p)
{
	vmp_list_inherited_by_release(&p->inherited_by);
	vmp_list_inherits_from_release(&p->inherits_from);
	vm_free(p);
}

BOOL vmp_type_test_inherits_from(const vmp_type* type, const vmp_type* inherits_from)
{
	if (vmp_list_inherits_from_contains(&type->inherits_from, inherits_from)) {
		return TRUE;
	}
	const vm_int32 count = type->inherits_from.count;
	for (vm_int32 i = 0; i < count; ++i) {
		if (vmp_type_test_inherits_from(vmp_list_inherits_from_get(&type->inherits_from, i), inherits_from)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL vmp_type_inherit_from(vmp_type* type, vmp_type* inherits_from)
{
	// Don't allow for cirtcular dependencies
	if (vmp_type_test_inherits_from(inherits_from, type)) {
		return FALSE;
	}

	vmp_list_inherits_from_add(&type->inherits_from, inherits_from);
	vmp_list_inherited_by_add(&inherits_from->inherited_by, type);
	return TRUE;
}

BOOL vmp_type_can_convert(const vmp_type* from, const vmp_type* to)
{
	// TODO: Flags should indicate if a type is complex (a struct) or a data type
	return from->flags == 0 && to->flags == 0;
}

vmp_arg* vmp_arg_new()
{
	vmp_arg* p = (vmp_arg*)vm_malloc(sizeof(vmp_arg));
	if (p == NULL)
		return NULL;
	p->header.keyword_type = VMP_KEYWORD_ARG;
	p->func = NULL;
	p->type = NULL;
	vm_string_zero(&p->name);
	p->offset = 0;
	p->index = 0;
	return p;
}

void vmp_arg_free(vmp_arg* a)
{
	vm_free(a);
}

void vmp_arg_set_name(vmp_arg* a, const vm_string* name)
{
	a->name = *name;
}

void vmp_arg_set_namesz(vmp_arg* a, const char* name, vm_int32 len)
{
	vm_string_setsz(&a->name, name, len);
}

vmp_return* vmp_return_new()
{
	vmp_return* p = (vmp_return*)vm_malloc(sizeof(vmp_return));
	if (p == NULL)
		return NULL;
	p->header.keyword_type = VMP_KEYWORD_RETURN;
	vm_string_zero(&p->name);
	p->func = NULL;
	p->type = NULL;
	return p;
}

void vmp_return_free(vmp_return* a)
{
	vm_free(a);
}

vmp_local* vmp_local_new()
{
	vmp_local* p = (vmp_local*)vm_malloc(sizeof(vmp_local));
	if (p == NULL)
		return NULL;
	p->header.keyword_type = VMP_KEYWORD_LOCAL;
	p->func = NULL;
	p->type = NULL;
	vm_string_zero(&p->name);
	p->offset = 0;
	p->index = 0;
	return p;
}

void vmp_local_free(vmp_local* l)
{
	vm_free(l);
}

void vmp_local_set_name(vmp_local* l, const vm_string* name)
{
	l->name = *name;
}

void vmp_local_set_namesz(vmp_local* l, const char* name, vm_int32 len)
{
	vm_string_setsz(&l->name, name, len);
}

vmp_func* vmp_func_new(const vm_string* name)
{
	return vmp_func_newsz(name->start, vm_string_length(name));
}

vmp_func* vmp_func_newsz(const char* name, vm_int32 name_len)
{
	vmp_func* p = (vmp_func*)vm_malloc(sizeof(vmp_func));
	if (p == NULL)
		return NULL;
	p->header.keyword_type = VMP_KEYWORD_FUNC;
	p->package = NULL;
	vm_string_setsz(&p->name, name, name_len);
	p->offset = 0;
	p->body_size = 0;
	vmp_list_args_init(&p->args);
	vmp_list_returns_init(&p->returns);
	vmp_list_locals_init(&p->locals);
	vmp_list_markers_init(&p->markers);
	p->native_func = NULL;
	p->flags = 0;
	p->args_stack_size = 0;
	p->returns_stack_size = 0;
	p->locals_stack_size = 0;
	p->first_instr = NULL;
	p->last_instr = NULL;
	return p;
}

void vmp_func_destroy(vmp_func* f)
{
	vmp_instr* instr = f->first_instr;
	while (instr != NULL) {
		vmp_instr* const next = instr->next;
		vm_free(instr);
		instr = next;
	}
	f->first_instr = f->last_instr = NULL;
	vmp_list_markers_release(&f->markers);
	vmp_list_locals_release(&f->locals);
	vmp_list_returns_release(&f->returns);
	vmp_list_args_release(&f->args);
	vm_free(f);
}


void vmp_func_add_flag(vmp_func* p, vm_int32 flag)
{
	p->flags |= flag;
}

void vmp_func_set_nativefunc(vmp_func* p, vm_nativefunc func)
{
	p->native_func = func;
	p->flags |= VMP_FUNC_FLAGS_EXTERN;
}

BOOL vmp_func_add_arg(vmp_func* f, vmp_arg* arg)
{
	if (arg->func != NULL)
		return FALSE;
	arg->func = f;
	const vm_int32 ret = vmp_list_args_add(&f->args, arg);
	if (ret >= 0) {
		arg->index = (vm_uint32)ret;
		return TRUE;
	}
	return FALSE;
}

vmp_arg* vmp_func_new_arg(vmp_func* f, vmp_type* type)
{
	vmp_arg* arg = vmp_arg_new();
	arg->type = type;
	if (vmp_func_add_arg(f, arg))
		return arg;
	return NULL;
}

vmp_arg* vmp_func_find_arg(vmp_func* f, const vm_string* name)
{
	return vmp_list_args_find(&f->args, name);
}

BOOL vmp_func_add_return(vmp_func* f, vmp_return* ret)
{
	if (ret->func != NULL)
		return FALSE;
	ret->func = f;
	return vmp_list_returns_add(&f->returns, ret) >= 0;
}

vmp_return* vmp_func_new_return(vmp_func* f, vmp_type* type)
{
	vmp_return* ret = vmp_return_new();
	ret->type = type;
	if (vmp_func_add_return(f, ret))
		return ret;
	return NULL;
}

BOOL vmp_func_add_local(vmp_func* f, vmp_local* l)
{
	if (l->func != NULL)
		return FALSE;
	l->func = f;
	const vm_int32 ret = vmp_list_locals_add(&f->locals, l);
	if (ret >= 0) {
		l->index = ret;
		return TRUE;
	}
	return FALSE;
}

vmp_local* vmp_func_find_local(vmp_func* f, const vm_string* name)
{
	return vmp_list_locals_find(&f->locals, name);
}

vmp_local* vmp_func_new_local(vmp_func* f, vmp_type* type)
{
	vmp_local* l = vmp_local_new();
	l->type = type;
	if (vmp_func_add_local(f, l))
		return l;
	return NULL;
}

vmp_marker* vmp_func_new_marker(vmp_func* f)
{
	vmp_marker* p = (vmp_marker*)vm_malloc(sizeof(vmp_marker));
	if (p == NULL)
		return NULL;
	p->func = f;
	vmp_list_markers_add(&f->markers, p);
	return p;
}

void vmp_marker_set_instr(vmp_marker* m, vmp_instr* instr)
{
	m->instr_offset = instr->instr_offset;
}

void vmp_marker_free(vmp_marker* m)
{
	vm_free(m);
}

vmp_instr* vmp_func_add_instr(vmp_func* f, vmp_instr* instr)
{
	instr->instr_offset = f->body_size;
	instr->func = f;
	f->body_size += instr->instr_size;

	if (f->first_instr == NULL) {
		f->first_instr = instr;
	}
	else {
		// Add the instruction to the linked list
		instr->prev = f->last_instr;
		f->last_instr->next = instr;
	}

	// Attach the rest of the instructions part of the execution chain until
	// we get the last instruction
	while (instr->next != NULL) {
		instr = instr->next;

		instr->instr_offset = f->body_size;
		instr->func = f;
		f->body_size += instr->instr_size;
	}

	// Figure out the last index
	f->last_instr = instr;

	return instr;
}

vmp_instr* vmp_func_inject_at_start(vmp_func* f, vmp_instr* instr)
{
	vmp_instr* const add_to_end = f->first_instr;
	instr->func = f;
	instr->instr_offset = 0;
	f->first_instr = instr;
	f->body_size = instr->instr_size;

	// Add the rest of the instructions
	while (instr->next != NULL) {
		instr = instr->next;
		instr->func = f;
		instr->instr_offset = f->body_size;
		f->body_size += instr->instr_size;
	}

	if (add_to_end != NULL) {
		add_to_end->prev = instr;
		instr->next = add_to_end;

		// Recalculate the offset and body size using the already added instructions
		instr = instr->next;
		while (instr != NULL) {
			instr->instr_offset = f->body_size;
			f->body_size += instr->instr_size;
			instr = instr->next;
		}
		return add_to_end->prev;
	}
	else {
		f->last_instr = instr;
	}
	return instr;
}

vmp_instr* vmp_func_inject_at_end(vmp_func* f, vmp_instr* instr)
{
	// Add the instruction to the end
	f->last_instr->next = instr;
	instr->prev = f->last_instr;

	f->last_instr = instr;
	instr->func = f;
	instr->instr_offset = f->body_size;
	f->body_size += instr->instr_size;

	// Add the rest of the instructions and calculate the instruction offset
	while (instr->next != NULL) {
		instr = instr->next;
		instr->func = f;
		instr->instr_offset = f->body_size;
		f->body_size += instr->instr_size;
	}
	f->last_instr = instr;
	return instr;
}

vmp_instr* vmp_func_inject_after(vmp_func* f, vmp_instr* after, vmp_instr* instr)
{
	if (after == NULL)
		return vmp_func_inject_at_start(f, instr);
	else if (after == f->last_instr) {
		return vmp_func_inject_at_end(f, instr);
	}
	else {
		// We are adding the instruction in the middle, where instructions exist both before and
		// after the supplied instruction.
		vm_int32 offset = after->instr_offset + after->instr_size;
		vmp_instr* const add_to_end = after->next;
		f->body_size = offset;
		
		instr->func = f;
		instr->instr_offset = f->body_size;
		f->body_size += instr->instr_size;
		instr->prev = after;
		after->next = instr;

		// Figure out the offset and the size of the function body
		while (instr->next != NULL) {
			instr = instr->next;
			instr->func = f;
			instr->instr_offset = f->body_size;
			f->body_size += instr->instr_size;
		}
		instr->next = add_to_end;
		add_to_end->prev = instr;

		// Recalculate the offset after the injection is done
		instr = add_to_end;
		while (instr != NULL) {
			instr->instr_offset = f->body_size;
			f->body_size += instr->instr_size;
			instr = instr->next;
		}
		return add_to_end->prev;
	}
}

vmp_instr* vmp_func_remove_instr(vmp_func* f, vmp_instr* instr)
{
	if (f->first_instr == instr) {
		f->first_instr = instr->next;
	}
	if (f->last_instr == instr) {
		f->last_instr = instr->prev;
	}
	if (instr->prev != NULL) {
		instr->prev->next = instr->next;
	}
	if (instr->next != NULL) {
		instr->next->prev = instr->prev;
	}

	// recalculate the instructions offset because of this
	instr = f->first_instr;
	f->body_size = 0;
	while (instr != NULL) {
		instr->instr_offset = f->body_size;
		f->body_size += instr->instr_size;
		instr = instr->next;
	}
	return instr;
}

vmp_keyword* vmp_func_find_keyword(vmp_func* f, const vm_string* name)
{
	vmp_arg* const arg = vmp_func_find_arg(f, name);
	if (arg != NULL)
		return &arg->header;
	vmp_local* const local = vmp_func_find_local(f, name);
	if (local != NULL)
		return &local->header;
	return vmp_package_find_keyword(f->package, name);
}

void vmp_func_begin_body(vmp_func* f)
{
}

void vmp_func_begin_end(vmp_func* f)
{
}
