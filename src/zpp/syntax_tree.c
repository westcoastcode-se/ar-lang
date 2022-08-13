#include "syntax_tree.h"
#include "compiler.h"
#include "../vm_debug.h"

zpp_syntax_tree zpp_syntax_tree_error_ = { ZPP_SYNTAX_TREE_ERROR, NULL, NULL, NULL };

zpp_syntax_tree_node zpp_syntax_tree_error()
{
	return &zpp_syntax_tree_error_;
}

BOOL zpp_syntax_tree_is_error(zpp_syntax_tree_node node)
{
	return node == &zpp_syntax_tree_error_;
}

BOOL zpp_syntax_tree_has_parent(zpp_syntax_tree_node st)
{
	return st->parent != NULL;
}

zpp_syntax_tree_node zpp_syntax_tree_get_parent(zpp_syntax_tree_node st)
{
	return st->parent;
}

BOOL zpp_syntax_tree_has_children(zpp_syntax_tree_node st)
{
	switch (st->type)
	{
	case ZPP_SYNTAX_TREE_ROOT:
	case ZPP_SYNTAX_TREE_PACKAGE:
	case ZPP_SYNTAX_TREE_FUNC:
	case ZPP_SYNTAX_TREE_TYPE:
	case ZPP_SYNTAX_TREE_RETURN:
		return TRUE;
	default:
		return FALSE;
	}
}

void zpp_syntax_tree_add(zpp_syntax_tree* st, zpp_syntax_tree_node node)
{
	if (st->node == NULL)
		st->node = st->node_end = node;
	else {
		st->node_end->tail = node;
		node->head = st->node_end;
		st->node_end = node;
	}
	node->parent = st;
}

zpp_syntax_tree_node zpp_syntax_tree_find_parent_of_type(zpp_syntax_tree_node s, zpp_syntax_tree_object_type type)
{
	s = s->parent;
	while (s) {
		if (s->type == type) {
			return s;
		}
		s = s->parent;
	}
	return NULL;
}

void zpp_syntax_tree_init(zpp_syntax_tree* st, zpp_syntax_tree_object_type type)
{
	st->type = type;
	st->parent = NULL;
	st->stack_type = NULL;
	st->tail = st->head = NULL;
	st->node = st->node_end = NULL;
}

void zpp_syntax_tree_free(zpp_syntax_tree* st)
{
	ASSERT_NOT_NULL(st);
	while (st != NULL)
	{
		zpp_syntax_tree* const tail = st->tail;
		switch (st->type)
		{
		case ZPP_SYNTAX_TREE_PACKAGE:
			break;
		case ZPP_SYNTAX_TREE_FUNC:
			break;
		case ZPP_SYNTAX_TREE_BINOP:
			break;
		case ZPP_SYNTAX_TREE_UNARYOP:
			break;
		case ZPP_SYNTAX_TREE_RETURN:
			break;
		default:
			break;
		}

		zpp_syntax_tree* node = st->node;
		while (node) {
			zpp_syntax_tree* const node_tail = node->tail;
			zpp_syntax_tree_free(node);
			node = node_tail;
		}

		vm_free(st);
		st = tail;
	}
}

BOOL zpp_syntax_tree_compile_root(zpp_compiler* c, zpp_syntax_tree_root* st)
{
	zpp_syntax_tree* node = ZPP_SYNTAX_TREE(st)->node;
	while (node != NULL) {
		if (!zpp_synax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	return TRUE;
}

BOOL zpp_syntax_tree_compile_package(zpp_compiler* c, zpp_syntax_tree_package* st)
{
	zpp_syntax_tree* node = ZPP_SYNTAX_TREE(st)->node;
	while (node != NULL) {
		if (!zpp_synax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	return TRUE;
}

BOOL zpp_syntax_tree_compile_func(zpp_compiler* c, zpp_syntax_tree_func* st)
{
	vmp_func_begin_body(st->function->func);
	zpp_syntax_tree* node = ZPP_SYNTAX_TREE(st)->node;
	while (node != NULL) {
		if (!zpp_synax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	vmp_func_begin_end(st->function->func);
	return TRUE;
}

BOOL zpp_syntax_tree_compile_return(zpp_compiler* c, zpp_syntax_tree_return* st)
{
	zpp_syntax_tree* node = ZPP_SYNTAX_TREE(st)->node;
	while (node != NULL) {
		if (!zpp_synax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	vmp_func_add_instr(st->closest_function_node->function->func, vmp_instr_ret());
	return TRUE;
}

BOOL zpp_compiler_compile_binop(zpp_compiler* c, zpp_syntax_tree_binop* binop)
{
	// push left
	// push right
	// op

	zpp_syntax_tree* node = ZPP_SYNTAX_TREE(binop)->node;
	if (!zpp_synax_tree_compile(c, node))
		return FALSE;
	node = node->tail;
	if (!zpp_synax_tree_compile(c, node))
		return FALSE;
	switch (binop->op)
	{
	case ZPP_TOKEN_OP_MINUS:
		vmp_func_add_instr(binop->closest_function_node->function->func, 
			vmp_instr_sub(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(binop), c)));
		return TRUE;
	case ZPP_TOKEN_OP_PLUS:
		vmp_func_add_instr(binop->closest_function_node->function->func,
			vmp_instr_add(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(binop), c)));
		return TRUE;
	default:
		// TODO: Add support for alternate operators
		return FALSE;
	}
}

BOOL zpp_compiler_compile_unaryop(zpp_compiler* c, zpp_syntax_tree_unaryop* unaryop)
{
	// push expression
	// neg

	zpp_syntax_tree* node = ZPP_SYNTAX_TREE(unaryop)->node;
	if (!zpp_synax_tree_compile(c, node))
		return FALSE;
	switch (unaryop->op)
	{
	case ZPP_TOKEN_OP_MINUS:
		vmp_func_add_instr(unaryop->closest_function_node->function->func, 
			vmp_instr_neg(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(unaryop), c)));
		return TRUE;
	default:
		// TODO: Add support for alternate operators
		return FALSE;
	}
}

BOOL zpp_compiler_compile_const_value(zpp_compiler* c, zpp_syntax_tree_const_value* val)
{
	// ldc
	vmp_func_add_instr(val->closest_function_node->function->func, 
		vmp_instr_ldc(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(val), c), val->value));
	return TRUE;
}

zpp_syntax_tree_root* zpp_syntax_tree_root_new()
{
	zpp_syntax_tree_root* p = vm_safe_malloc(zpp_syntax_tree_root);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_ROOT);
	return p;
}

void zpp_syntax_tree_root_add(zpp_syntax_tree_root* root, zpp_syntax_tree_node node)
{
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(root), node);
}

zpp_syntax_tree_package* zpp_syntax_tree_package_new(zpp_package* pkg)
{
	zpp_syntax_tree_package* p = vm_safe_malloc(zpp_syntax_tree_package);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_PACKAGE);
	p->package = pkg;
	return p;
}

void zpp_syntax_tree_package_add_func(zpp_syntax_tree_package* package, zpp_syntax_tree_func* func)
{
	func->closest_package_node = package;
	zpp_syntax_tree_package_add(package, ZPP_SYNTAX_TREE(func));
}

void zpp_syntax_tree_package_add(zpp_syntax_tree_package* package, zpp_syntax_tree_node node)
{
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(package), node);
}

zpp_syntax_tree_func* zpp_syntax_tree_func_new(struct zpp_func* func)
{
	zpp_syntax_tree_func* p = vm_safe_malloc(zpp_syntax_tree_func);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_FUNC);
	p->function = func;
	p->closest_package_node = NULL;
	return p;
}

BOOL zpp_synax_tree_compile(zpp_compiler* c, zpp_syntax_tree* st)
{
	switch (st->type) {
	case ZPP_SYNTAX_TREE_ROOT:
		return zpp_syntax_tree_compile_root(c, (zpp_syntax_tree_root*)st);
	case ZPP_SYNTAX_TREE_PACKAGE:
		return zpp_syntax_tree_compile_package(c, (zpp_syntax_tree_package*)st);
	case ZPP_SYNTAX_TREE_FUNC:
		return zpp_syntax_tree_compile_func(c, (zpp_syntax_tree_func*)st);
	case ZPP_SYNTAX_TREE_RETURN:
		return zpp_syntax_tree_compile_return(c, (zpp_syntax_tree_return*)st);
	case ZPP_SYNTAX_TREE_BINOP:
		return zpp_compiler_compile_binop(c, (zpp_syntax_tree_binop*)st);
	case ZPP_SYNTAX_TREE_UNARYOP:
		return zpp_compiler_compile_unaryop(c, (zpp_syntax_tree_unaryop*)st);
	case ZPP_SYNTAX_TREE_CONST_VALUE:
		return zpp_compiler_compile_const_value(c, (zpp_syntax_tree_const_value*)st);
	case ZPP_SYNTAX_TREE_TYPE:
		return TRUE;
	case ZPP_SYNTAX_TREE_ERROR:
		return FALSE;
	default:
		return FALSE;
	}
	return TRUE;
}

zpp_symbol* zpp_symbol_find_symbol0(zpp_syntax_tree* st, const vm_string* name, BOOL recursive, BOOL include_imports)
{
	// TODO: Add support for constants and global variables
	// 

	// 
	switch (st->type)
	{
	case ZPP_SYNTAX_TREE_TYPE:
	{
		zpp_syntax_tree_type* const type = (zpp_syntax_tree_type*)st;
		if (zpp_symbol_has_name(ZPP_SYMBOL(type->type), name)) {
			return ZPP_SYMBOL(type->type);
		}
	}
	break;
	case ZPP_SYNTAX_TREE_FUNC:
	{
		zpp_syntax_tree_func* const func = (zpp_syntax_tree_func*)st;
		if (zpp_symbol_has_name(ZPP_SYMBOL(func->function), name)) {
			return ZPP_SYMBOL(func->function);
		}
	}
	break;
	case ZPP_SYNTAX_TREE_IMPORT:
	{
		if (!include_imports)
			break;

		zpp_syntax_tree_import* const import = (zpp_syntax_tree_import*)st;
		if (zpp_symbol_has_name(ZPP_SYMBOL(import->package), name)) {
			return ZPP_SYMBOL(import->package);
		}
		// Search the imported package after something with the supplied name
		zpp_symbol* const symbol = zpp_package_find_symbol(import->package, name);
		if (symbol) {
			return symbol;
		}
	}
	break;
	case ZPP_SYNTAX_TREE_PACKAGE:
	{
		zpp_syntax_tree_package* const pkg = (zpp_syntax_tree_package*)st;
		if (zpp_symbol_has_name(ZPP_SYMBOL(pkg->package), name)) {
			return ZPP_SYMBOL(pkg->package);
		}
		// Search all children if requested
		if (recursive) {
			zpp_syntax_tree* node = st->node;
			while (node) {
				zpp_symbol* const symbol = zpp_symbol_find_symbol0(node, name, FALSE, include_imports);
				if (symbol)
					return symbol;
				node = node->tail;
			}
		}
	}
	break;
	case ZPP_SYNTAX_TREE_ROOT:
	{
		zpp_syntax_tree_root* const root = (zpp_syntax_tree_root*)st;
		// Search all children in root 
		if (recursive) {
			zpp_syntax_tree* node = st->node;
			while (node) {
				zpp_symbol* const symbol = zpp_symbol_find_symbol0(node, name, FALSE, FALSE);
				if (symbol)
					return symbol;
				node = node->tail;
			}
		}
		// We've reached the root node, which is as far we are allowed to search
		return NULL;
	}
	break;
	}

	if (st->parent != NULL && recursive) {
		return zpp_symbol_find_symbol0(st->parent, name, recursive, include_imports);
	}
	return NULL;
}

zpp_symbol* zpp_syntax_tree_find_symbol(zpp_syntax_tree* st, const vm_string* name)
{
	return zpp_symbol_find_symbol0(st, name, FALSE, TRUE);
}

zpp_symbol* zpp_syntax_tree_find_symbol_include_imports(zpp_syntax_tree* st, const vm_string* name)
{

	return zpp_symbol_find_symbol0(st, name, TRUE, TRUE);
}

zpp_syntax_tree_type* zpp_syntax_tree_type_new(zpp_type* type)
{
	zpp_syntax_tree_type* p = vm_safe_malloc(zpp_syntax_tree_type);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_TYPE);
	p->type = type;
	return p;
}

void zpp_syntax_tree_root_destroy(zpp_syntax_tree_root* st)
{
}

void zpp_syntax_tree_package_destroy(zpp_syntax_tree_package* st)
{
}

void zpp_syntax_tree_func_destroy(zpp_syntax_tree_func* st)
{
}

void zpp_syntax_tree_type_destroy(zpp_syntax_tree_type* st)
{
}

void zpp_syntax_tree_destroy(zpp_syntax_tree* st)
{
	switch (st->type) {
	case ZPP_SYNTAX_TREE_ROOT:
		zpp_syntax_tree_root_destroy((zpp_syntax_tree_root*)st);
		break;
	case ZPP_SYNTAX_TREE_PACKAGE:
		zpp_syntax_tree_package_destroy((zpp_syntax_tree_package*)st);
		break;
	case ZPP_SYNTAX_TREE_FUNC:
		zpp_syntax_tree_func_destroy((zpp_syntax_tree_func*)st);
		break;
	case ZPP_SYNTAX_TREE_TYPE:
		zpp_syntax_tree_type_destroy((zpp_syntax_tree_type*)st);
		break;
	case ZPP_SYNTAX_TREE_RETURN:
		break;
	case ZPP_SYNTAX_TREE_CONST_VALUE:
		break;
	case ZPP_SYNTAX_TREE_BINOP:
		break;
	case ZPP_SYNTAX_TREE_UNARYOP:
		break;
	default:
		assert(FALSE); // Add 
	}

	zpp_syntax_tree* node = st->node;
	while (node) {
		zpp_syntax_tree_node tail = node->tail;
		zpp_syntax_tree_destroy(node);
		node = tail;
	}

	vm_free(st);
}

zpp_syntax_tree_unaryop* zpp_syntax_tree_unaryop_new(zpp_syntax_tree_node left, zpp_token_type op)
{
	zpp_syntax_tree_unaryop* const p = vm_safe_malloc(zpp_syntax_tree_unaryop);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_UNARYOP);
	p->header.stack_type = left->stack_type;
	p->op = op;
	p->closest_function_node = NULL;
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(p), left);
	return p;
}

zpp_syntax_tree_binop* zpp_syntax_tree_binop_new(zpp_syntax_tree_node left, zpp_syntax_tree_node right, zpp_token_type op)
{
	zpp_syntax_tree_binop* const p = vm_safe_malloc(zpp_syntax_tree_binop);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_BINOP);
	p->header.stack_type = right->stack_type;
	p->op = op;
	p->closest_function_node = NULL;
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(p), left);
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(p), right);
	return p;
}

zpp_syntax_tree_const_value* zpp_syntax_tree_const_value_new()
{
	zpp_syntax_tree_const_value* const p = vm_safe_malloc(zpp_syntax_tree_const_value);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_CONST_VALUE);
	p->closest_function_node = NULL;
	return p;
}

zpp_syntax_tree_node zpp_synax_tree_parse_factor(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* state)
{
	if (t->type == ZPP_TOKEN_OP_MINUS || t->type == ZPP_TOKEN_OP_PLUS) {
		const zpp_token_type token_type = t->type;
		zpp_token_next(t);
		zpp_syntax_tree_node left = zpp_synax_tree_parse_factor(c, t, state);
		if (left == NULL)
			return NULL;
		zpp_syntax_tree_unaryop* const unaryop = zpp_syntax_tree_unaryop_new(left, token_type);
		if (unaryop == NULL)
			return NULL;
		unaryop->closest_function_node = state->func_node;
		return ZPP_SYNTAX_TREE(unaryop);
	}

	if (t->type == ZPP_TOKEN_VALUE_INT) {
		zpp_syntax_tree_const_value* const val = zpp_syntax_tree_const_value_new();
		if (val == NULL)
			return NULL;
		val->closest_function_node = state->func_node;
		vm_string type_name;
		type_name.start = "int32";
		type_name.end = type_name.start + 5;
		ZPP_SYNTAX_TREE_STACK_TYPE(val) = zpp_syntax_tree_find_symbol_include_imports(state->parent_node, &type_name);
		val->value.i4 = zpp_token_i4(t);
		zpp_token_next(t);
		return ZPP_SYNTAX_TREE(val);
	}

	if (t->type == ZPP_TOKEN_VALUE_DECIMAL) {
		zpp_syntax_tree_const_value* const val = zpp_syntax_tree_const_value_new();
		if (val == NULL)
			return NULL;
		val->closest_function_node = state->func_node;
		vm_string type_name;
		type_name.start = "float32";
		type_name.end = type_name.start + 7;
		ZPP_SYNTAX_TREE_STACK_TYPE(val) = zpp_syntax_tree_find_symbol_include_imports(state->parent_node, &type_name);
		val->value.f4 = zpp_token_f4(t);
		zpp_token_next(t);
		return ZPP_SYNTAX_TREE(val);
	}

	if (t->type == ZPP_TOKEN_PARAN_L) {
		zpp_token_next(t);
		zpp_syntax_tree_node node = zpp_synax_tree_parse_term(c, t, state);
		if (node == NULL)
			return NULL;
		// Expected right paranthesis
		if (t->type != ZPP_TOKEN_PARAN_R) {
			zpp_message_syntax_error(state, ')');
			return NULL;
		}
		zpp_token_next(t);
		return node;
	}
	return NULL;
}

zpp_syntax_tree_node zpp_synax_tree_parse_term(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	zpp_syntax_tree_node left = zpp_synax_tree_parse_factor(c, t, state);
	if (left == NULL)
		return NULL;

	while (1) {
		switch (t->type) {
		case ZPP_TOKEN_OP_MINUS:
		case ZPP_TOKEN_OP_PLUS:
		{
			const zpp_token_type token_type = t->type;
			zpp_token_next(t);
			zpp_syntax_tree_node right = zpp_synax_tree_parse_factor(c, t, state);
			if (left == NULL)
				return NULL;
			zpp_syntax_tree_binop* binop = zpp_syntax_tree_binop_new(left, right, token_type);
			if (binop == NULL)
				return NULL;
			left = ZPP_SYNTAX_TREE(binop);
			binop->closest_function_node = state->func_node;
			zpp_token_next(t);
			break;
		}
		default:
			goto end;
		}
	}
end:
	return left;
}

zpp_syntax_tree_return* zpp_syntax_tree_return_new(zpp_syntax_tree_node expr)
{
	zpp_syntax_tree_return* const p = vm_safe_malloc(zpp_syntax_tree_return);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_RETURN);
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(p), expr);
	return p;
}

zpp_syntax_tree_node zpp_synax_tree_parse_expression(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	switch (t->type) {
	case ZPP_TOKEN_KEYWORD_RETURN:
	{
		zpp_token_next(t);
		zpp_syntax_tree_node return_expr = zpp_synax_tree_parse_term(c, t, state);
		if (return_expr == NULL)
			return NULL;

		zpp_syntax_tree_return* ret = zpp_syntax_tree_return_new(return_expr);
		if (ret == NULL) {
			zpp_message_out_of_memory(state);
			return NULL;
		}
		ret->closest_function_node = state->func_node;
		return ZPP_SYNTAX_TREE(ret);
	}
	break;
	}

	return NULL;
}
