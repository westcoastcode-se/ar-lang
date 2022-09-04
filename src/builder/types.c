#include "types.h"
#include "definition.h"
#include "messages.h"
#include "../arMemory.h"

DEFINE_LIST_BASE_DEFAULT(arB_types, arB_type, 4, 4);
DEFINE_LIST_FIND(arB_types, arB_type, name);

DEFINE_LIST_BASE_DEFAULT(arB_funcs, arB_func, 8, 8);
DEFINE_LIST_FIND(arB_funcs, arB_func, name);

DEFINE_LIST_BASE_DEFAULT(vmp_list_globals, arB_global, 4, 4);
DEFINE_LIST_FIND(vmp_list_globals, arB_global, name);

DEFINE_LIST_BASE_RELEASE_ONLY(arB_inherited_by, arB_type, 2, 2);
DEFINE_LIST_FIND(arB_inherited_by, arB_type, name);

DEFINE_LIST_BASE_RELEASE_ONLY(arB_inherits_from, arB_type, 2, 2);
DEFINE_LIST_FIND(arB_inherits_from, arB_type, name);

DEFINE_LIST_BASE_DEFAULT(arB_locals, arB_local, 8, 8);
DEFINE_LIST_FIND(arB_locals, arB_local, name);

DEFINE_LIST_BASE_DEFAULT(arB_returns, arB_return, 2, 2);

DEFINE_LIST_BASE_DEFAULT(arB_args, arB_arg, 4, 4);
DEFINE_LIST_FIND(arB_args, arB_arg, name);

DEFINE_LIST_BASE_DEFAULT(arB_markers, arB_marker, 4, 4);

arB_package* arB_package_new(const arString* name)
{
	return arB_package_newsz(name->start, arString_length(name));
}

arB_package* arB_package_newsz(const char* name, arInt32 len)
{
	arB_package* p = (arB_package*)arMalloc(sizeof(arB_package));
	if (p == NULL)
		return NULL;
	arString_setsz(&p->name, name, len);
	p->pipeline = NULL;
	arB_types_init(&p->types);
	arB_funcs_init(&p->funcs);
	vmp_list_globals_init(&p->globals);
	return p;
}

void arB_package_destroy(arB_package* p)
{
	vmp_list_globals_release(&p->globals);
	arB_funcs_release(&p->funcs);
	arB_types_release(&p->types);
	arFree(p);
}

int arB_package_add_func(arB_package* p, arB_func* func)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(func);

	if (func->package != NULL)
		return VMP_LIST_ALREADY_ADDED;
	if (arB_funcs_add(&p->funcs, func) < 0)
		return VMP_LIST_OUT_OF_MEMORY;
	func->package = p;
	return VMP_LIST_ADDED;
}

int arB_package_add_type(arB_package* p, arB_type* type)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(type);

	if (type->package != NULL)
		return VMP_LIST_ALREADY_ADDED;
	if (arB_types_find(&p->types, &type->name) != NULL)
		return VMP_LIST_ALREADY_EXISTS;
	if (arB_types_add(&p->types, type) < 0)
		return VMP_LIST_OUT_OF_MEMORY;
	type->package = p;
	return VMP_LIST_ADDED;
}

int arB_package_add_global(arB_package* p, arB_global* g)
{
	ASSERT_NOT_NULL(p);
	ASSERT_NOT_NULL(g);

	if (g->package != NULL)
		return VMP_LIST_ALREADY_ADDED;
	if (vmp_list_globals_find(&p->globals, &g->name) != NULL)
		return VMP_LIST_ALREADY_EXISTS;
	if (vmp_list_globals_add(&p->globals, g) < 0)
		return VMP_LIST_OUT_OF_MEMORY;
	g->package = p;
	return VMP_LIST_ADDED;
}

arB_type* arB_type_new(const arString* name)
{
	arB_type* p = (arB_type*)arMalloc(sizeof(arB_type));
	if (p == NULL)
		return NULL;
	p->name = *name;
	p->package = NULL;
	p->size = 0;
	p->flags = 0;
	p->data_type = 0;
	p->of_type = NULL;
	arB_inherits_from_init(&p->inherits_from);
	arB_inherited_by_init(&p->inherited_by);
	return p;
}

arB_type* arB_type_from_props(const arB_type_props* props)
{
	arB_type* p = (arB_type*)arMalloc(sizeof(arB_type));
	if (p == NULL)
		return NULL;
	p->name = props->name;
	p->package = NULL;
	p->size = props->size;
	p->flags = props->flags;
	p->data_type = props->data_type;
	p->of_type = props->of_type;
	arB_inherits_from_init(&p->inherits_from);
	arB_inherited_by_init(&p->inherited_by);
	return p;
}

const arB_type_props* arB_type_props_get(const arString* name, arUint32 size, arUint32 flags, arUint8 data_type, const arB_type* of_type)
{
	static arB_type_props p;
	p.name = *name;
	p.size = size;
	p.flags = flags;
	p.data_type = data_type;
	p.of_type = of_type;
	return &p;
}

// Mark the supplied type as an of type
BOOL arB_type_set_of_type(arB_type* t, const arB_type* of_type)
{
	if (t->of_type != NULL) {
		return FALSE;
	}
	t->of_type = of_type;
	return TRUE;
}

void arB_type_destroy(arB_type* p)
{
	arB_inherited_by_release(&p->inherited_by);
	arB_inherits_from_release(&p->inherits_from);
	arFree(p);
}

BOOL arB_type_test_inherits_from(const arB_type* type, const arB_type* inherits_from)
{
	if (arB_inherits_from_contains(&type->inherits_from, inherits_from)) {
		return TRUE;
	}
	const arInt32 count = type->inherits_from.count;
	for (arInt32 i = 0; i < count; ++i) {
		if (arB_type_test_inherits_from(arB_inherits_from_get(&type->inherits_from, i), inherits_from)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL arB_type_inherit_from(arB_type* type, arB_type* inherits_from)
{
	// Don't allow for cirtcular dependencies
	if (arB_type_test_inherits_from(inherits_from, type)) {
		return FALSE;
	}

	arB_inherits_from_add(&type->inherits_from, inherits_from);
	arB_inherited_by_add(&inherits_from->inherited_by, type);
	return TRUE;
}

BOOL arB_type_can_convert(const arB_type* from, const arB_type* to)
{
	// TODO: Flags should indicate if a type is complex (a struct) or a data type
	return from->flags == 0 && to->flags == 0;
}

arB_arg* arB_arg_new()
{
	arB_arg* p = (arB_arg*)arMalloc(sizeof(arB_arg));
	if (p == NULL)
		return NULL;
	arString_zero(&p->name);
	p->func = NULL;
	p->type = NULL;
	p->offset = 0;
	p->index = 0;
	return p;
}

void arB_arg_destroy(arB_arg* a)
{
	arFree(a);
}

void arB_arg_set_name(arB_arg* a, const arString* name)
{
	a->name = *name;
}

void arB_arg_set_namesz(arB_arg* a, const char* name, arInt32 len)
{
	arString_setsz(&a->name, name, len);
}

arB_return* arB_return_new()
{
	arB_return* p = (arB_return*)arMalloc(sizeof(arB_return));
	if (p == NULL)
		return NULL;
	arString_zero(&p->name);
	p->func = NULL;
	p->type = NULL;
	return p;
}

void arB_return_destroy(arB_return* a)
{
	arFree(a);
}

arB_local* arB_local_new()
{
	arB_local* p = (arB_local*)arMalloc(sizeof(arB_local));
	if (p == NULL)
		return NULL;
	arString_zero(&p->name);
	p->func = NULL;
	p->type = NULL;
	p->offset = 0;
	p->index = 0;
	return p;
}

void arB_local_destroy(arB_local* l)
{
	arFree(l);
}

void arB_local_set_name(arB_local* l, const arString* name)
{
	l->name = *name;
}

void arB_local_set_namesz(arB_local* l, const char* name, arInt32 len)
{
	arString_setsz(&l->name, name, len);
}

arB_global* arB_global_new()
{
	arB_global* p = arSafeMalloc(arB_global);
	arString_zero(&p->name);
	p->package = NULL;
	p->type = NULL;
	p->offset = 0;
	return p;
}

void arB_global_set_name(arB_global* l, const arString* name)
{
	l->name = *name;
}

void arB_global_set_namesz(arB_global* l, const char* name, arInt32 len)
{
	arString_setsz(&l->name, name, len);
}

void arB_global_set_type(arB_global* g, arB_type* type)
{
	g->type = type;
}

void arB_global_destroy(arB_global* g)
{
	arFree(g);
}

arB_func* arB_func_new(const arString* name)
{
	return arB_func_newsz(name->start, arString_length(name));
}

arB_func* arB_func_newsz(const char* name, arInt32 name_len)
{
	arB_func* p = arSafeMalloc(arB_func);
	arString_setsz(&p->name, name, name_len);
	p->package = NULL;
	p->offset = 0;
	p->body_size = 0;
	arB_args_init(&p->args);
	arB_returns_init(&p->returns);
	arB_locals_init(&p->locals);
	arB_markers_init(&p->markers);
	p->native_func = NULL;
	p->flags = 0;
	p->args_stack_size = 0;
	p->returns_stack_size = 0;
	p->locals_stack_size = 0;
	p->first_instr = NULL;
	p->last_instr = NULL;
	return p;
}

void arB_func_destroy(arB_func* f)
{
	arB_definition* instr = f->first_instr;
	while (instr != NULL) {
		arB_definition* const next = instr->next;
		arFree(instr);
		instr = next;
	}
	f->first_instr = f->last_instr = NULL;
	arB_markers_release(&f->markers);
	arB_locals_release(&f->locals);
	arB_returns_release(&f->returns);
	arB_args_release(&f->args);
	arFree(f);
}


void arB_func_add_flag(arB_func* p, arInt32 flag)
{
	p->flags |= flag;
}

void arB_func_set_native(arB_func* p, arB_native_func func)
{
	p->native_func = func;
	p->flags |= arB_FUNC_FLAGS_EXTERN;
}

BOOL arB_func_add_arg(arB_func* f, arB_arg* arg)
{
	if (arg->func != NULL)
		return FALSE;
	arg->func = f;
	const arInt32 ret = arB_args_add(&f->args, arg);
	if (ret >= 0) {
		arg->index = (arUint32)ret;
		return TRUE;
	}
	return FALSE;
}

arB_arg* arB_func_new_arg(arB_func* f, arB_type* type)
{
	arB_arg* arg = arB_arg_new();
	arg->type = type;
	if (arB_func_add_arg(f, arg))
		return arg;
	return NULL;
}

BOOL arB_func_add_return(arB_func* f, arB_return* ret)
{
	if (ret->func != NULL)
		return FALSE;
	ret->func = f;
	return arB_returns_add(&f->returns, ret) >= 0;
}

arB_return* arB_func_new_return(arB_func* f, arB_type* type)
{
	arB_return* ret = arB_return_new();
	ret->type = type;
	if (arB_func_add_return(f, ret))
		return ret;
	return NULL;
}

BOOL arB_func_add_local(arB_func* f, arB_local* l)
{
	if (l->func != NULL)
		return FALSE;
	l->func = f;
	const arInt32 ret = arB_locals_add(&f->locals, l);
	if (ret >= 0) {
		l->index = ret;
		return TRUE;
	}
	return FALSE;
}

arB_local* arB_func_new_local(arB_func* f, arB_type* type)
{
	arB_local* l = arB_local_new();
	l->type = type;
	if (arB_func_add_local(f, l))
		return l;
	return NULL;
}

arB_marker* arB_func_new_marker(arB_func* f)
{
	arB_marker* p = arSafeMalloc(arB_marker);
	p->func = f;
	arB_markers_add(&f->markers, p);
	return p;
}

void arB_marker_set_instr(arB_marker* m, arB_definition* instr)
{
	m->instr_offset = instr->instr_offset;
}

void arB_marker_destroy(arB_marker* m)
{
	arFree(m);
}

arB_definition* arB_func_add_instr(arB_func* f, arB_definition* instr)
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

arB_definition* arB_func_inject_at_start(arB_func* f, arB_definition* instr)
{
	arB_definition* const add_to_end = f->first_instr;
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

arB_definition* arB_func_inject_at_end(arB_func* f, arB_definition* instr)
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

arB_definition* arB_func_inject_after(arB_func* f, arB_definition* after, arB_definition* instr)
{
	if (after == NULL)
		return arB_func_inject_at_start(f, instr);
	else if (after == f->last_instr) {
		return arB_func_inject_at_end(f, instr);
	}
	else {
		// We are adding the instruction in the middle, where instructions exist both before and
		// after the supplied instruction.
		arInt32 offset = after->instr_offset + after->instr_size;
		arB_definition* const add_to_end = after->next;
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

arB_definition* arB_func_remove_instr(arB_func* f, arB_definition* instr)
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

void arB_func_body_begin(arB_func* f)
{
}

void arB_func_body_end(arB_func* f)
{
	const arInt32 num_args = f->args.count;
	const arInt32 num_rets = f->returns.count;
	arInt32 i;
	arInt32 stack_offset = 0;

	for (i = 0; i < num_args; ++i) {
		arB_arg* const arg = arB_args_get(&f->args, i);
		arg->offset = stack_offset;
		stack_offset += arg->type->size;
	}
	f->args_stack_size = stack_offset;

	stack_offset = 0;
	for (i = 0; i < num_rets; ++i) {
		arB_return* const ret = arB_returns_get(&f->returns, i);
		stack_offset += ret->type->size;
	}
	f->returns_stack_size = stack_offset;
}
