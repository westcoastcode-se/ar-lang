#include "symbol.h"
#include "messages.h"
#include "tokens.h"
#include "../arCompiler.h"
#include "../arMemory.h"
#include "../arUtils.h"

DEFINE_LIST_BASE_RELEASE_ONLY(arC_inherits_from, arC_type, 2, 2);
DEFINE_LIST_FIND(arC_inherits_from, arC_type, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(arC_inherited_by, arC_type, 2, 2);
DEFINE_LIST_FIND(arC_inherited_by, arC_type, header.name);

void arCompilerSymbol_init(arC_symbol* s, arC_symbol_type type)
{
	s->type = type;
	arString_zero(&s->name);
	arString_zero(&s->signature);
}

BOOL arC_symbol_has_name(arC_symbol* s, const arString* name)
{
	return arString_cmp(&s->name, name);
}

BOOL arC_symbol_has_signature(arC_symbol* s, const arString* signature)
{
	return arString_cmp(&s->signature, signature);
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

void arC_type_sign_init(arC_type_sign* sign)
{
	arString_zero(&sign->signature);
	arString_zero(&sign->short_signature);
	arString_zero(&sign->name);
	sign->package = NULL;
}

BOOL arC_type_sign_build(arC_type_sign* sign, const arC_state* s)
{
	ASSERT_NOT_NULL(sign);

	int bytes_left = 1024;
	arByte signature_data[1024];
	arByte* sig = signature_data;

	sig = arStrcpy_s(sig, &bytes_left, asC_symbol_signature(sign->package)->start, arString_length(asC_symbol_signature(sign->package)));
	sig = arStrcpy_s(sig, &bytes_left, "#", 1);
	sig = arStrcpy_s(sig, &bytes_left, sign->name.start, arString_length(&sign->name));
	if (bytes_left == 0) {
		// Size of the signature is too large
		arC_message_not_implemented(s);
		return FALSE;
	}

	const arString* const result = arStringPool_stringsz(&s->compiler->pipeline->string_pool, signature_data, 1024 - bytes_left);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	sign->signature = *result;
	// The short signature for the function startd after the "<package signature>#"
	sign->short_signature.start = sign->signature.start + arString_length(asC_symbol_signature(sign->package)) + 1;
	sign->short_signature.end = sign->signature.end;
	return TRUE;
}

BOOL arC_type_sign_parse(arC_type_sign* sign, const arC_state* s)
{
	return TRUE;
}

arC_type* arC_type_new(const arC_type_sign* sign)
{
	arC_type* const p = arSafeMalloc(arC_type);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_TYPE);
	p->header.name = sign->short_signature;
	p->header.signature = sign->signature;
	p->signature = *sign;
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
	if (type == NULL)
		return arC_message_out_of_memory(s);
	arB_type_set_signature(type, asC_symbol_signature(t));

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
	ASSERT_NOT_NULL(t->signature.package->package);
	if (arB_package_add_type(t->signature.package->package, type) != VMP_LIST_ADDED)
		return arC_message_out_of_memory(s);
	return TRUE;
}

BOOL arC_type_resolve(arC_type* t, const arC_state* s)
{
	if (t->type != NULL)
		return TRUE;

	arB_type* const type = arB_type_new(&t->header.name);
	if (type == NULL)
		return arC_message_out_of_memory(s);
	arB_type_set_signature(type, asC_symbol_signature(t));

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
	ASSERT_NOT_NULL(t->signature.package->package);
	if (arB_package_add_type(t->signature.package->package, type) != VMP_LIST_ADDED)
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
	p->children_head = p->children_tail = NULL;
	p->types = p->types_end = NULL;
	p->funcs = p->funcs_end = NULL;
	p->package = NULL;
	return p;
}

void arC_package_destroy(arC_package* ptr)
{
	ptr->children = ptr->children_end = NULL;
	ptr->children_head = ptr->children_tail = NULL;

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

BOOL arC_package_build_signature0(arC_package* p, const struct arC_state* s)
{
	int bytes_left = 1024;
	arByte signature_data[1024];
	arByte* sig = signature_data;

	sig = arStrcpy_s(sig, &bytes_left, asC_symbol_signature(p->parent)->start, arString_length(asC_symbol_signature(p->parent)));
	sig = arStrcpy_s(sig, &bytes_left, ".", 1);
	sig = arStrcpy_s(sig, &bytes_left, p->header.name.start, arString_length(&p->header.name));
	if (bytes_left == 0) {
		// Size of the signature is too large
		arC_message_not_implemented(s);
		return FALSE;
	}

	const arString* const result = arStringPool_stringsz(&s->compiler->pipeline->string_pool, signature_data, 1024 - bytes_left);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	p->header.signature = *result;
	return TRUE;
}

BOOL arC_package_build_signature(arC_package* p, const struct arC_state* s)
{
	ASSERT_NOT_NULL(p);
	if (p->parent == NULL) {
		p->header.signature = p->header.name;
		return TRUE;
	}
	return arC_package_build_signature0(p, s);
}

BOOL arC_package_resolve(arC_package* p, const arC_state* s)
{
	if (p->package != NULL)
		return TRUE;

	arB_package* const package = arB_package_new(&p->header.name);
	if (package == NULL)
		return arC_message_out_of_memory(s);
	arB_package_set_signature(package, asC_symbol_signature(p));
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
	assert((t->signature.package == NULL || t->signature.package == p) && "Expected the type to not be added to another package");

	t->signature.package = p;
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
	assert((f->signature.package == NULL || f->signature.package == p) && "Expected the function to not be added to another package");

	f->signature.package = p;
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
		p->children_end->children_tail = sub_package;
		sub_package->children_head = p->children_end;
		p->children_end = sub_package;
	}
}
void arC_func_sign_init(arC_func_sign* p)
{
	arString_zero(&p->signature);
	arString_zero(&p->short_signature);
	arString_zero(&p->name);
	p->package = NULL;
	p->package = NULL;
	p->arguments = p->arguments_end = NULL;
	p->arguments_count = 0;
	p->returns = p->returns_end = NULL;
	p->returns_count = 0;
}

BOOL arC_func_sign_build(arC_func_sign* sign, const arC_state* s)
{
	ASSERT_NOT_NULL(sign);

	int i;
	int bytes_left = 1024;
	int short_signature_length = 0;
	arByte signature_data[1024];
	arByte* sig = signature_data;

	sig = arString_cpy_s(sig, &bytes_left, asC_symbol_signature(sign->package));
	sig = arStrcpy_s(sig, &bytes_left, "#", 1);
	sig = arString_cpy_s(sig, &bytes_left, &sign->name);
	sig = arStrcpy_s(sig, &bytes_left, "(", 1);
	arC_arg* arg = sign->arguments;
	for (i = 0; i < sign->arguments_count; ++i, arg = arg->tail) {
		if (i > 0)
			sig = arStrcpy_s(sig, &bytes_left, ",", 1);
		const arString* type_sig = asC_symbol_signature(arg->type);
		sig = arStrcpy_s(sig, &bytes_left, type_sig->start, arString_length(type_sig));
	}
	sig = arStrcpy_s(sig, &bytes_left, ")", 1);
	sig = arStrcpy_s(sig, &bytes_left, "(", 1);
	arC_return* ret = sign->returns;
	for (i = 0; i < sign->returns_count; ++i, ret = ret->tail) {
		if (i > 0)
			sig = arStrcpy_s(sig, &bytes_left, ",", 1);
		const arString* type_sig = asC_symbol_signature(ret->type);
		sig = arStrcpy_s(sig, &bytes_left, type_sig->start, arString_length(type_sig));
	}
	sig = arStrcpy_s(sig, &bytes_left, ")", 1);
	if (bytes_left == 0) {
		// Size of the signature is too large
		return arC_message_not_implemented(s);
	}

	const arString* const result = arStringPool_stringsz(&s->compiler->pipeline->string_pool, signature_data, 1024 - bytes_left);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	sign->signature = *result;
	// The short signature for the function startd after the "<package signature>#"
	sign->short_signature.start = sign->signature.start + arString_length(asC_symbol_signature(sign->package)) + 1;
	sign->short_signature.end = sign->signature.end;
	return TRUE;
}

arC_func* arC_func_new(const arC_func_sign* signature)
{
	arC_func* const p = arSafeMalloc(arC_func);
	arCompilerSymbol_init(asC_symbol(p), arC_SYMBOL_FUNC);
	p->header.name = signature->short_signature;
	p->header.signature = signature->signature;
	p->signature = *signature;
	p->locals = p->locals_end = NULL;
	p->tail = p->head = NULL;
	p->func = NULL;
	return p;
}

void arC_func_destroy(arC_func* f)
{
	const arC_func_sign* s = &f->signature;

	arC_arg* arg = s->arguments;
	while (arg) {
		arC_arg* const tail = arg->tail;
		arC_arg_destroy(arg);
		arg = tail;
	}

	arC_return* ret = s->returns;
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
	arC_func_sign* s = &f->signature;

	if (s->arguments == NULL) {
		s->arguments = s->arguments_end = a;
	}
	else {
		s->arguments_end->tail = a;
		a->head = s->arguments_end;
		s->arguments_end = a;
	}
	s->arguments_count++;
}

void arC_func_add_return(arC_func* f, arC_return* r)
{
	ASSERT_NOT_NULL(f);
	ASSERT_NOT_NULL(r);
	arC_func_sign* s = &f->signature;

	if (s->returns == NULL) {
		s->returns = s->returns_end = r;
	}
	else {
		s->returns_end->tail = r;
		r->head = s->returns_end;
		s->returns_end = r;
	}
	s->returns_count++;
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
	arB_func_set_signature(func, &f->signature.signature);
	f->func = func;

	// add arguments
	arC_arg* arg = f->signature.arguments;
	while (arg != NULL) {
		if (!arC_type_resolve(arg->type, s))
			return FALSE;
		arg->arg = arB_func_new_arg(f->func, arg->type->type);
		arB_arg_set_name(arg->arg, asC_symbol_name(arg));
		arg = arg->tail;
	}

	// add return values
	arC_return* ret = f->signature.returns;
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
	ASSERT_NOT_NULL(f->signature.package->package);
	if (arB_package_add_func(f->signature.package->package, func) != VMP_LIST_ADDED)
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
	arC_arg* arg = f->signature.arguments;
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
	return f->signature.arguments_count;
}

arInt32 arC_func_count_returns(arC_func* f)
{
	ASSERT_NOT_NULL(f);
	return f->signature.returns_count;
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
