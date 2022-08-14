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
	case ZPP_TOKEN_OP_MULT:
		vmp_func_add_instr(binop->closest_function_node->function->func,
			vmp_instr_mul(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(binop), c)));
		return TRUE;
	case ZPP_TOKEN_OP_DIV:
		vmp_func_add_instr(binop->closest_function_node->function->func,
			vmp_instr_div(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(binop), c)));
		return TRUE;
	case ZPP_TOKEN_TEST_LT:
		vmp_func_add_instr(binop->closest_function_node->function->func,
			vmp_instr_clt(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(binop), c)));
		return TRUE;
	case ZPP_TOKEN_TEST_GT:
		vmp_func_add_instr(binop->closest_function_node->function->func,
			vmp_instr_cgt(zpp_symbol_resolve_type(ZPP_SYNTAX_TREE_STACK_TYPE(binop), c)));
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

BOOL zpp_compiler_compile_assign(zpp_compiler* c, zpp_syntax_tree_assign* assign)
{
	// TODO: It should be possible to do something like "value, value2 := 10, 20"
	zpp_syntax_tree* node = ZPP_SYNTAX_TREE(assign)->node;
	if (!zpp_synax_tree_compile(c, node))
		return FALSE;

	if (assign->target->type == ZPP_SYMBOL_LOCAL) {
		// stl <index>
		zpp_local* const local = (zpp_local*)assign->target;
		vmp_func_add_instr(assign->closest_function_node->function->func, vmp_instr_stl(local->local));
		return TRUE;
	}
	// Unknown assign expression
	return FALSE;
}

BOOL zpp_compiler_compile_load_local(zpp_compiler* c, zpp_syntax_tree_load_local* local)
{
	vmp_func_add_instr(local->closest_function_node->function->func, vmp_instr_ldl(local->target->local));
	return TRUE;
}

BOOL zpp_compiler_compile_load_argument(zpp_compiler* c, zpp_syntax_tree_load_argument* arg)
{
	vmp_func_add_instr(arg->closest_function_node->function->func, vmp_instr_lda(arg->target->arg));
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
	case ZPP_SYNTAX_TREE_ASSIGN:
		return zpp_compiler_compile_assign(c, (zpp_syntax_tree_assign*)st);
	case ZPP_SYNTAX_TREE_LOAD_LOCAL:
		return zpp_compiler_compile_load_local(c, (zpp_syntax_tree_load_local*)st);
	case ZPP_SYNTAX_TREE_LOAD_ARGUMENT:
		return zpp_compiler_compile_load_argument(c, (zpp_syntax_tree_load_argument*)st);
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

		// Check local function types
		zpp_symbol* result = zpp_func_find_symbol(func->function, name);
		if (result)
			return result;

		// Search all function children in root 
		if (recursive) {
			zpp_syntax_tree* node = st->node;
			while (node) {
				zpp_symbol* const symbol = zpp_symbol_find_symbol0(node, name, FALSE, FALSE);
				if (symbol)
					return symbol;
				node = node->tail;
			}
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
	case ZPP_SYNTAX_TREE_ASSIGN:
		break;
	case ZPP_SYNTAX_TREE_BINOP:
		break;
	case ZPP_SYNTAX_TREE_UNARYOP:
		break;
	case ZPP_SYNTAX_TREE_LOAD_LOCAL:
		break;
	case ZPP_SYNTAX_TREE_LOAD_ARGUMENT:
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
	p->value.type = 0;
	return p;
}

zpp_syntax_tree_load_local* zpp_syntax_tree_load_local_new()
{
	zpp_syntax_tree_load_local* const p = vm_safe_malloc(zpp_syntax_tree_load_local);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_LOAD_LOCAL);
	p->target = NULL;
	p->closest_function_node = NULL;
	return p;
}

zpp_syntax_tree_load_argument* zpp_syntax_tree_load_argument_new()
{
	zpp_syntax_tree_load_argument* const p = vm_safe_malloc(zpp_syntax_tree_load_argument);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_LOAD_ARGUMENT);
	p->target = NULL;
	p->closest_function_node = NULL;
	return p;
}

zpp_syntax_tree_return* zpp_syntax_tree_return_new(zpp_syntax_tree_node expr)
{
	zpp_syntax_tree_return* const p = vm_safe_malloc(zpp_syntax_tree_return);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_RETURN);
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(p), expr);
	return p;
}

zpp_syntax_tree_assign* zpp_syntax_tree_assign_new(zpp_syntax_tree_node expr, zpp_symbol* target)
{
	zpp_syntax_tree_assign* const p = vm_safe_malloc(zpp_syntax_tree_assign);
	zpp_syntax_tree_init(ZPP_SYNTAX_TREE(p), ZPP_SYNTAX_TREE_ASSIGN);
	p->target = target;
	zpp_syntax_tree_add(ZPP_SYNTAX_TREE(p), expr);
	return p;
}

zpp_syntax_tree_node zpp_synax_tree_parse_arith_expr(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* s);
zpp_syntax_tree_node zpp_synax_tree_parse_comp_expr(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* s);
zpp_syntax_tree_node zpp_synax_tree_parse_term(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state);
zpp_syntax_tree_node zpp_synax_tree_parse_factor(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* state);
zpp_syntax_tree_node zpp_synax_tree_parse_bitwise(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* state);
zpp_syntax_tree_node zpp_synax_tree_parse_atom(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* state);

// Create a binary operator plus minus
zpp_syntax_tree_node zpp_synax_tree_parse_binop_plus_minus(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state, zpp_synax_tree_parse_fn fn)
{
	zpp_syntax_tree_node left = fn(c, t, state);
	if (zpp_syntax_tree_is_error(left))
		return left;

	while (1) {
		switch (t->type) {
		case ZPP_TOKEN_OP_MINUS:
		case ZPP_TOKEN_OP_PLUS:
		{
			const zpp_token_type token_type = t->type;
			zpp_token_next(t);
			zpp_syntax_tree_node right = fn(c, t, state);
			if (zpp_syntax_tree_is_error(right))
				return right;
			zpp_syntax_tree_binop* binop = zpp_syntax_tree_binop_new(left, right, token_type);
			if (binop == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			left = ZPP_SYNTAX_TREE(binop);
			binop->closest_function_node = state->func_node;
			break;
		}
		default:
			goto end;
		}
	}
end:
	return left;
}

// Create a binary operator mult div
zpp_syntax_tree_node zpp_synax_tree_parse_binop_mult_div(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state, zpp_synax_tree_parse_fn fn)
{
	zpp_syntax_tree_node left = fn(c, t, state);
	if (zpp_syntax_tree_is_error(left))
		return left;

	while (1) {
		switch (t->type) {
		case ZPP_TOKEN_OP_MULT:
		case ZPP_TOKEN_OP_DIV:
		{
			const zpp_token_type token_type = t->type;
			zpp_token_next(t);
			zpp_syntax_tree_node right = fn(c, t, state);
			if (zpp_syntax_tree_is_error(right))
				return right;
			zpp_syntax_tree_binop* binop = zpp_syntax_tree_binop_new(left, right, token_type);
			if (binop == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			left = ZPP_SYNTAX_TREE(binop);
			binop->closest_function_node = state->func_node;
			break;
		}
		default:
			goto end;
		}
	}
end:
	return left;
}

// Create a binary operator bitwise
zpp_syntax_tree_node zpp_synax_tree_parse_binop_bitwise(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state, zpp_synax_tree_parse_fn left_fn,
	zpp_synax_tree_parse_fn right_fn)
{
	zpp_syntax_tree_node left = left_fn(c, t, state);
	if (zpp_syntax_tree_is_error(left))
		return left;

	while (1) {
		switch (t->type) {
		case ZPP_TOKEN_BIT_AND:
		{
			const zpp_token_type token_type = t->type;
			zpp_token_next(t);
			zpp_syntax_tree_node right = right_fn(c, t, state);
			if (zpp_syntax_tree_is_error(right))
				return right;
			zpp_syntax_tree_binop* binop = zpp_syntax_tree_binop_new(left, right, token_type);
			if (binop == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			left = ZPP_SYNTAX_TREE(binop);
			binop->closest_function_node = state->func_node;
			break;
		}
		default:
			goto end;
		}
	}
end:
	return left;
}

zpp_syntax_tree_node zpp_synax_tree_parse_binop_comp(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state, zpp_synax_tree_parse_fn fn)
{
	zpp_syntax_tree_node left = fn(c, t, state);
	if (zpp_syntax_tree_is_error(left))
		return left;

	while (1) {
		switch (t->type) {
		case ZPP_TOKEN_TEST_EQUALS:
		case ZPP_TOKEN_TEST_NOT_EQUALS:
		case ZPP_TOKEN_TEST_LT:
		case ZPP_TOKEN_TEST_LTE:
		case ZPP_TOKEN_TEST_GT:
		case ZPP_TOKEN_TEST_GTE:
		{
			const zpp_token_type token_type = t->type;
			zpp_token_next(t);
			zpp_syntax_tree_node right = fn(c, t, state);
			if (zpp_syntax_tree_is_error(right))
				return right;
			zpp_syntax_tree_binop* binop = zpp_syntax_tree_binop_new(left, right, token_type);
			if (binop == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			left = ZPP_SYNTAX_TREE(binop);
			binop->closest_function_node = state->func_node;
			break;
		}
		default:
			goto end;
		}
	}
end:
	return left;
}


zpp_syntax_tree_node zpp_synax_tree_parse_atom(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* state)
{
	if (t->type == ZPP_TOKEN_VALUE_INT) {
		zpp_syntax_tree_const_value* const val = zpp_syntax_tree_const_value_new();
		if (val == NULL) {
			zpp_message_out_of_memory(state);
			return zpp_syntax_tree_error();
		}
		val->closest_function_node = state->func_node;
		vm_string type_name;
		type_name.start = "int32";
		type_name.end = type_name.start + 5;
		ZPP_SYNTAX_TREE_STACK_TYPE(val) = zpp_syntax_tree_find_symbol_include_imports(state->parent_node, &type_name);
		val->value.i4 = zpp_token_i4(t);
		zpp_token_next(t);
		return ZPP_SYNTAX_TREE(val);
	}
	else if (t->type == ZPP_TOKEN_VALUE_DECIMAL) {
		zpp_syntax_tree_const_value* const val = zpp_syntax_tree_const_value_new();
		if (val == NULL) {
			zpp_message_out_of_memory(state);
			return zpp_syntax_tree_error();
		}
		val->closest_function_node = state->func_node;
		vm_string type_name;
		type_name.start = "float32";
		type_name.end = type_name.start + 7;
		ZPP_SYNTAX_TREE_STACK_TYPE(val) = zpp_syntax_tree_find_symbol_include_imports(state->parent_node, &type_name);
		val->value.f4 = zpp_token_f4(t);
		zpp_token_next(t);
		return ZPP_SYNTAX_TREE(val);
	}
	else if (t->type == ZPP_TOKEN_IDENTITY) {
		const vm_string keyword = t->string;
		zpp_token_next(t);

		if (t->type == ZPP_TOKEN_DECL_ASSIGN) {
			zpp_token_next(t);

			// Check to see if the local variable exists, and if not then add it
			zpp_symbol* symbol = zpp_func_find_symbol(state->func_node->function, &keyword);
			if (symbol != NULL) {
				// Symbol is already defined
				zpp_message_not_implemented(state);
				return zpp_syntax_tree_error();
			}

			// Expression to be set in the local variable
			zpp_syntax_tree_node decl_expression = zpp_synax_tree_parse_comp_expr(c, t, state);
			if (zpp_syntax_tree_is_error(decl_expression))
				return decl_expression;

			// Define the local variable
			zpp_local* const local = zpp_local_new(&keyword);
			if (local == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			local->type = decl_expression->stack_type;
			local->header.name = keyword;
			zpp_func_add_local(state->func_node->function, local);

			zpp_syntax_tree_assign* assign = zpp_syntax_tree_assign_new(decl_expression, ZPP_SYMBOL(local));
			if (assign == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			assign->closest_function_node = state->func_node;
			return ZPP_SYNTAX_TREE(assign);
		}
		else if (t->type == ZPP_TOKEN_ASSIGN) {
			// Existing variable, might be a local, argument or a global variable
			zpp_token_next(t);

			// Check to see if the local variable exists, and if not then add it
			zpp_symbol* symbol = zpp_func_find_symbol(state->func_node->function, &keyword);
			if (symbol == NULL) {
				// Not defined yet
				// TODO: Add support for an "unresolved" assignable type
				zpp_message_not_implemented(state);
				return zpp_syntax_tree_error();
			}

			// Expression to be set in the local variable
			zpp_syntax_tree_node decl_expression = zpp_synax_tree_parse_comp_expr(c, t, state);
			if (zpp_syntax_tree_is_error(decl_expression))
				return decl_expression;

			zpp_syntax_tree_assign* assign = zpp_syntax_tree_assign_new(decl_expression, symbol);
			if (assign == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			assign->closest_function_node = state->func_node;
			return ZPP_SYNTAX_TREE(assign);
		}
		else {
			zpp_symbol* const symbol = zpp_syntax_tree_find_symbol_include_imports(state->parent_node, &keyword);
			if (symbol == NULL) {
				// TODO: Add support for "unresolved" types (which is later on resolved as the "closest" type)
				zpp_message_not_implemented(state);
				return zpp_syntax_tree_error();
			}

			// Load a local variable
			if (symbol->type == ZPP_SYMBOL_LOCAL) {
				zpp_syntax_tree_load_local* load = zpp_syntax_tree_load_local_new();
				if (load == NULL) {
					zpp_message_out_of_memory(state);
					return zpp_syntax_tree_error();
				}
				load->closest_function_node = state->func_node;
				load->target = (zpp_local*)symbol;
				return ZPP_SYNTAX_TREE(load);
			}
			else if (symbol->type == ZPP_SYMBOL_ARGUMENT) {
				zpp_syntax_tree_load_argument* load = zpp_syntax_tree_load_argument_new();
				if (load == NULL) {
					zpp_message_out_of_memory(state);
					return zpp_syntax_tree_error();
				}
				load->closest_function_node = state->func_node;
				load->target = (zpp_argument*)symbol;
				return ZPP_SYNTAX_TREE(load);
			}
			else {
				zpp_message_not_implemented(state);
				return zpp_syntax_tree_error();
			}
		}		
	}
	else if (t->type == ZPP_TOKEN_PARAN_L) {
		zpp_token_next(t);
		zpp_syntax_tree_node node = zpp_synax_tree_parse_expression(c, t, state);
		if (zpp_syntax_tree_is_error(node))
			return node;
		// Expected right paranthesis
		if (t->type != ZPP_TOKEN_PARAN_R) {
			zpp_message_syntax_error(state, "expected }");
			return zpp_syntax_tree_error();
		}
		zpp_token_next(t);
		return node;
	}

	zpp_message_syntax_error(state, "expected number, decimal, +, -, (");
	return zpp_syntax_tree_error();
}

zpp_syntax_tree_node zpp_synax_tree_parse_bitwise(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* state)
{
	return zpp_synax_tree_parse_binop_bitwise(c, t, state, zpp_synax_tree_parse_atom, zpp_synax_tree_parse_factor);
}

zpp_syntax_tree_node zpp_synax_tree_parse_factor(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* state)
{
	if (t->type == ZPP_TOKEN_OP_MINUS || t->type == ZPP_TOKEN_OP_PLUS) {
		const zpp_token_type token_type = t->type;
		zpp_token_next(t);
		zpp_syntax_tree_node left = zpp_synax_tree_parse_factor(c, t, state);
		if (zpp_syntax_tree_is_error(left)) {
			return left;
		}
		zpp_syntax_tree_unaryop* const unaryop = zpp_syntax_tree_unaryop_new(left, token_type);
		if (unaryop == NULL) {
			zpp_message_out_of_memory(state);
			return zpp_syntax_tree_error();
		}
		unaryop->closest_function_node = state->func_node;
		return ZPP_SYNTAX_TREE(unaryop);
	}
	
	return zpp_synax_tree_parse_bitwise(c, t, state);
}

zpp_syntax_tree_node zpp_synax_tree_parse_term(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* s)
{
	return zpp_synax_tree_parse_binop_mult_div(c, t, s, zpp_synax_tree_parse_factor);
}

zpp_syntax_tree_node zpp_synax_tree_parse_arith_expr(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* s)
{
	return zpp_synax_tree_parse_binop_plus_minus(c, t, s, zpp_synax_tree_parse_term);
}

zpp_syntax_tree_node zpp_synax_tree_parse_comp_expr(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* s)
{
	if (t->type == ZPP_TOKEN_NOT) {
		const zpp_token_type token_type = t->type;
		zpp_token_next(t);
		zpp_syntax_tree_node left = zpp_synax_tree_parse_comp_expr(c, t, s);
		if (zpp_syntax_tree_is_error(left)) {
			return left;
		}
		zpp_syntax_tree_unaryop* const unaryop = zpp_syntax_tree_unaryop_new(left, token_type);
		if (unaryop == NULL) {
			zpp_message_out_of_memory(s);
			return zpp_syntax_tree_error();
		}
		unaryop->closest_function_node = s->func_node;
		return ZPP_SYNTAX_TREE(unaryop);
	} 

	return zpp_synax_tree_parse_binop_comp(c, t, s, zpp_synax_tree_parse_arith_expr);
}

zpp_syntax_tree_node zpp_synax_tree_parse_expression(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	if (zpp_token_is_keyword(t) && t->type != ZPP_TOKEN_KEYWORD) {
		switch (t->type) {
		case ZPP_TOKEN_KEYWORD_RETURN:
		{
			zpp_token_next(t);
			zpp_syntax_tree_node return_expr = zpp_synax_tree_parse_comp_expr(c, t, state);
			if (zpp_syntax_tree_is_error(return_expr))
				return return_expr;

			zpp_syntax_tree_return* ret = zpp_syntax_tree_return_new(return_expr);
			if (ret == NULL) {
				zpp_message_out_of_memory(state);
				return zpp_syntax_tree_error();
			}
			ret->closest_function_node = state->func_node;
			return ZPP_SYNTAX_TREE(ret);
		}
		case ZPP_TOKEN_KEYWORD_IF:
		{
			zpp_message_not_implemented(state);
			return zpp_syntax_tree_error();
		}
		case ZPP_TOKEN_KEYWORD_VAR:
		{
			zpp_message_not_implemented(state);
			return zpp_syntax_tree_error();
		}
		case ZPP_TOKEN_TEST_EQUALS:
		{
			zpp_message_not_implemented(state);
			return zpp_syntax_tree_error();
		}
		case ZPP_TOKEN_TEST_NOT_EQUALS:
		{
			zpp_message_not_implemented(state);
			return zpp_syntax_tree_error();
		}
		break;
		}

		zpp_message_not_implemented(state);
		return zpp_syntax_tree_error();
	}

	return zpp_synax_tree_parse_comp_expr(c, t, state);
}
