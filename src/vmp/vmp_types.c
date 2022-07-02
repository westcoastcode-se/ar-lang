#include "vmp_types.h"
#include "../vmc/vmc_debug.h"

vmp_package* vmp_package_new(const vm_string* name)
{
	return vmp_package_newsz(name->start, vm_string_length(name));
}

vmp_package* vmp_package_newsz(const char* name, vm_int32 len)
{
	vmp_package* p = (vmp_package*)vmc_malloc(sizeof(vmp_package));
	if (p == NULL)
		return NULL;
	p->pipeline = NULL;
	vm_string_setsz(&p->name, name, len);
	vmp_list_types_init(&p->types);
	vmp_list_funcs_init(&p->funcs);
	return p;
}

void vmp_package_destroy(vmp_package* p)
{
	vmp_list_funcs_release(&p->funcs);
	vmp_list_types_release(&p->types);
	vmc_free(p);
}

BOOL vmp_package_add_func(vmp_package* p, vmp_func* func)
{
	if (func->package != NULL)
		return FALSE;
	return vmp_list_funcs_add(&p->funcs, func) >= 0;
}

BOOL vmp_package_add_type(vmp_package* p, vmp_type* type)
{
	if (type->package != NULL)
		return FALSE;
	return vmp_list_types_add(&p->types, type) >= 0;
}

vmp_type* vmp_package_new_type(vmp_package* p, const vm_string* name, vm_uint32 size)
{
	vmp_type* type = vmp_type_new(name);
	type->size = size;
	if (vmp_package_add_type(p, type))
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

vmp_type* vmp_type_new(const vm_string* name)
{
	vmp_type* p = (vmp_type*)vmc_malloc(sizeof(vmp_type));
	if (p == NULL)
		return NULL;
	p->package = NULL;
	p->name = *name;
	p->size = 0;
	p->parent_type = NULL;
	return p;
}

void vmp_type_destroy(vmp_type* p)
{
	vmc_free(p);
}

vmp_arg* vmp_arg_new()
{
	vmp_arg* p = (vmp_arg*)vmc_malloc(sizeof(vmp_arg));
	if (p == NULL)
		return NULL;
	p->func = NULL;
	p->type = NULL;
	vm_string_zero(&p->name);
	p->offset = 0;
	return p;
}

void vmp_arg_free(vmp_arg* a)
{
	vmc_free(a);
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
	vmp_return* p = (vmp_return*)vmc_malloc(sizeof(vmp_return));
	if (p == NULL)
		return NULL;
	p->func = NULL;
	p->type = NULL;
	p->offset = 0;
	return p;
}

void vmp_return_free(vmp_return* a)
{
	vmc_free(a);
}

vmp_local* vmp_local_new()
{
	vmp_local* p = (vmp_local*)vmc_malloc(sizeof(vmp_local));
	if (p == NULL)
		return NULL;
	p->func = NULL;
	p->type = NULL;
	vm_string_zero(&p->name);
	p->offset = 0;
	return p;
}

void vmp_local_free(vmp_local* l)
{
	vmc_free(l);
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
	vmp_func* p = (vmp_func*)vmc_malloc(sizeof(vmp_func));
	if (p == NULL)
		return NULL;
	p->package = NULL;
	vm_string_setsz(&p->name, name, name_len);
	p->offset = 0;
	p->body_size = 0;
	vmp_list_args_init(&p->args);
	vmp_list_returns_init(&p->returns);
	vmp_list_locals_init(&p->locals);
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
		vmc_free(instr);
		instr = next;
	}
	f->first_instr = f->last_instr = NULL;
	vmp_list_locals_release(&f->locals);
	vmp_list_returns_release(&f->returns);
	vmp_list_args_release(&f->args);
	vmc_free(f);
}

BOOL vmp_func_add_arg(vmp_func* f, vmp_arg* arg)
{
	if (arg->func != NULL)
		return FALSE;
	f->args_stack_size += arg->type->size;
	return vmp_list_args_add(&f->args, arg) >= 0;
}

vmp_arg* vmp_func_new_arg(vmp_func* f, vmp_type* type)
{
	vmp_arg* arg = vmp_arg_new();
	arg->type = type;
	if (vmp_func_add_arg(f, arg))
		return arg;
	return NULL;
}

BOOL vmp_func_add_return(vmp_func* f, vmp_return* ret)
{
	if (ret->func != NULL)
		return FALSE;
	f->returns_stack_size += ret->type->size;
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
	f->locals_stack_size += l->type->size;
	return vmp_list_locals_add(&f->locals, l) >= 0;
}

vmp_local* vmp_func_new_local(vmp_func* f, vmp_type* type)
{
	vmp_local* l = vmp_local_new();
	l->type = type;
	if (vmp_func_add_local(f, l))
		return l;
	return NULL;
}

BOOL vmp_func_add_instr(vmp_func* f, vmp_instr* instr)
{
	if (f->first_instr == NULL)
		f->first_instr = f->last_instr = instr;
	else {
		f->last_instr->next = instr;
		f->last_instr = instr;
	}
	instr->instr_offset = f->body_size;
	instr->func = f;
	f->body_size += instr->instr_size;
	return TRUE;
}

void vmp_func_begin_body(vmp_func* f)
{
}

void vmp_func_begin_end(vmp_func* f)
{
}
