#include "syntax_tree.h"
#include "tokens.h"
#include "optimizations.h"
#include "messages.h"
#include "../arMemory.h"
#include "../arCompiler.h"

arC_syntax_tree_node arC_syntax_tree_error()
{
	static arC_syntax_tree instance = { AR_SYNTAX_TREE_ERROR, NULL, NULL, NULL };
	return &instance;
}

BOOL arC_syntax_tree_is_error(arC_syntax_tree_node node)
{
	return node == arC_syntax_tree_error();
}

BOOL arC_syntax_tree_has_parent(arC_syntax_tree_node st)
{
	return st->parent != NULL;
}

arC_syntax_tree_node arC_syntax_tree_get_parent(arC_syntax_tree_node st)
{
	return st->parent;
}

// Get the underlying arC_func function representation from the supplied state
arC_func* arCompiler_state_get_func(const arC_state* s)
{
	return s->func_node->function;
}

void arC_syntax_tree_add(arC_syntax_tree* st, arC_syntax_tree_node node)
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

void arC_syntax_tree_remove(arC_syntax_tree* st, arC_syntax_tree_node node)
{
	assert(st == node->parent);
	if (st->node == node) {
		st->node = st->node->tail;
		st->node->head = NULL;
	}
	if (st->node_end == node) {
		st->node_end = st->node_end->head;
		st->node_end->tail = NULL;
	}
	if (node->head) {
		node->head->tail = node->tail;
	}
	if (node->tail) {
		node->tail->head = node->head;
	}
}

void arC_syntax_tree_remove_replace(arC_syntax_tree* st, arC_syntax_tree_node old_node, arC_syntax_tree_node new_node)
{
	assert(st == old_node->parent);
	assert(old_node == new_node->parent);

	arC_syntax_tree_node head = old_node->head;
	arC_syntax_tree_node tail = old_node->tail;

	new_node->head = head;
	new_node->tail = tail;

	if (head == NULL)
		st->node = new_node;
	else
		head->tail = new_node;
	
	if (tail == NULL)
		st->node_end = new_node;
	else
		tail->head = new_node;
}

arC_syntax_tree_node arC_syntax_tree_find_parent_type(arC_syntax_tree_node s, arC_syntax_tree_type type)
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

void arC_syntax_tree_init(arC_syntax_tree* st, arC_syntax_tree_type type)
{
	st->type = type;
	st->parent = NULL;
	st->stack_type = NULL;
	st->tail = st->head = NULL;
	st->node = st->node_end = NULL;
}

BOOL arC_syntax_tree_compile_root(arCompiler* c, arC_syntax_tree_root* st)
{
	arC_syntax_tree* node = asC_syntax_tree(st)->node;
	while (node != NULL) {
		if (!arC_syntax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	return TRUE;
}

BOOL arC_syntax_tree_compile_package(arCompiler* c, arC_syntax_tree_node_package* st)
{
	arC_syntax_tree* node = asC_syntax_tree(st)->node;
	while (node != NULL) {
		if (!arC_syntax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	return TRUE;
}

BOOL arC_syntax_tree_compile_func(arCompiler* c, arC_syntax_tree_node_func* st)
{
	arB_func_body_begin(st->function->func);
	arC_syntax_tree* node = asC_syntax_tree(st)->node;
	while (node != NULL) {
		if (!arC_syntax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	arB_func_body_end(st->function->func);
	return TRUE;
}

BOOL arC_syntax_tree_compile_return(arCompiler* c, arC_syntax_tree_node_return* st)
{
	arC_syntax_tree* node = asC_syntax_tree(st)->node;
	while (node != NULL) {
		if (!arC_syntax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	arB_func_add_instr(st->closest_function_node->function->func, arB_instr_ret());
	return TRUE;
}

BOOL arC_syntax_tree_compile_binop(arCompiler* c, arC_syntax_tree_node_binop* binop)
{
	// push left
	// push right
	// op

	arC_syntax_tree* node = asC_syntax_tree(binop)->node;
	if (!arC_syntax_tree_compile(c, node))
		return FALSE;
	node = node->tail;
	if (!arC_syntax_tree_compile(c, node))
		return FALSE;
	switch (binop->op)
	{
	case ARTOK_OP_MINUS:
		arB_func_add_instr(binop->closest_function_node->function->func, 
			arB_instr_sub(arC_symbol_resolve_type(asC_syntax_tree_stack_type(binop), c)));
		return TRUE;
	case ARTOK_OP_PLUS:
		arB_func_add_instr(binop->closest_function_node->function->func,
			arB_instr_add(arC_symbol_resolve_type(asC_syntax_tree_stack_type(binop), c)));
		return TRUE;
	case ARTOK_OP_MULT:
		arB_func_add_instr(binop->closest_function_node->function->func,
			arB_instr_mul(arC_symbol_resolve_type(asC_syntax_tree_stack_type(binop), c)));
		return TRUE;
	case ARTOK_OP_DIV:
		arB_func_add_instr(binop->closest_function_node->function->func,
			arB_instr_div(arC_symbol_resolve_type(asC_syntax_tree_stack_type(binop), c)));
		return TRUE;
	case ARTOK_TEST_LT:
		arB_func_add_instr(binop->closest_function_node->function->func,
			arB_instr_clt(arC_symbol_resolve_type(asC_syntax_tree_stack_type(binop), c)));
		return TRUE;
	case ARTOK_TEST_GT:
		arB_func_add_instr(binop->closest_function_node->function->func,
			arB_instr_cgt(arC_symbol_resolve_type(asC_syntax_tree_stack_type(binop), c)));
		return TRUE;
	default:
		// TODO: Add support for alternate operators
		return FALSE;
	}
}

BOOL arC_syntax_tree_compile_unaryop(arCompiler* c, arC_syntax_tree_node_unaryop* unaryop)
{
	// push expression
	// neg

	arC_syntax_tree* node = asC_syntax_tree(unaryop)->node;
	if (!arC_syntax_tree_compile(c, node))
		return FALSE;
	switch (unaryop->op)
	{
	case ARTOK_OP_MINUS:
		arB_func_add_instr(unaryop->closest_function_node->function->func, 
			arB_instr_neg(arC_symbol_resolve_type(asC_syntax_tree_stack_type(unaryop), c)));
		return TRUE;
	case ARTOK_BIT_NOT:
		arB_func_add_instr(unaryop->closest_function_node->function->func,
			arB_instr_bit_not(arC_symbol_resolve_type(asC_syntax_tree_stack_type(unaryop), c)));
		return TRUE;
	default:
		// TODO: Add support for alternate operators
		return FALSE;
	}
}

BOOL arC_syntax_tree_compile_const_value(arCompiler* c, arC_syntax_tree_node_const_value* val)
{
	// ldc
	arB_func_add_instr(val->closest_function_node->function->func, 
		arB_instr_ldc(arC_symbol_resolve_type(asC_syntax_tree_stack_type(val), c), val->value));
	return TRUE;
}

BOOL arC_syntax_tree_compile_assign(arCompiler* c, arC_syntax_tree_node_assign* assign)
{
	// TODO: It should be possible to do something like "value, value2 := 10, 20"
	arC_syntax_tree* node = asC_syntax_tree(assign)->node;
	if (!arC_syntax_tree_compile(c, node))
		return FALSE;

	if (assign->target->type == arC_SYMBOL_LOCAL) {
		// stl <index>
		arC_local* const local = (arC_local*)assign->target;
		arB_func_add_instr(assign->closest_function_node->function->func, arB_instr_stl(local->local));
		return TRUE;
	}
	// Unknown assign expression
	return FALSE;
}

BOOL arC_syntax_tree_compile_load_local(arCompiler* c, arC_syntax_tree_node_load_local* local)
{
	arB_func_add_instr(local->closest_function_node->function->func, arB_instr_ldl(local->target->local));
	return TRUE;
}

BOOL arC_syntax_tree_compile_load_arg(arCompiler* c, arC_syntax_tree_node_load_arg* arg)
{
	arB_func_add_instr(arg->closest_function_node->function->func, arB_instr_lda(arg->target->arg));
	return TRUE;
}

arC_syntax_tree_root* arC_syntax_tree_root_new()
{
	arC_syntax_tree_root* p = arSafeMalloc(arC_syntax_tree_root);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_ROOT);
	return p;
}

void arC_syntax_tree_root_add(arC_syntax_tree_root* root, arC_syntax_tree_node node)
{
	arC_syntax_tree_add(asC_syntax_tree(root), node);
}

arC_syntax_tree_node_package* arC_syntax_tree_node_package_new(arC_package* pkg)
{
	arC_syntax_tree_node_package* p = arSafeMalloc(arC_syntax_tree_node_package);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_PACKAGE);
	p->package = pkg;
	return p;
}

void arC_syntax_tree_node_package_add_func(arC_syntax_tree_node_package* package, arC_syntax_tree_node_func* func)
{
	func->closest_package_node = package;
	arC_syntax_tree_node_package_add(package, asC_syntax_tree(func));
}

void arC_syntax_tree_node_package_add(arC_syntax_tree_node_package* package, arC_syntax_tree_node node)
{
	arC_syntax_tree_add(asC_syntax_tree(package), node);
}

arC_syntax_tree_node_func* arC_syntax_tree_node_func_new(struct arC_func* func)
{
	arC_syntax_tree_node_func* p = arSafeMalloc(arC_syntax_tree_node_func);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_FUNC);
	p->function = func;
	p->closest_package_node = NULL;
	return p;
}

BOOL arC_syntax_tree_compile(arCompiler* c, arC_syntax_tree* st)
{
	switch (st->type) {
	case AR_SYNTAX_TREE_ROOT:
		return arC_syntax_tree_compile_root(c, (arC_syntax_tree_root*)st);
	case AR_SYNTAX_TREE_PACKAGE:
		return arC_syntax_tree_compile_package(c, (arC_syntax_tree_node_package*)st);
	case AR_SYNTAX_TREE_FUNC:
		return arC_syntax_tree_compile_func(c, (arC_syntax_tree_node_func*)st);
	case AR_SYNTAX_TREE_RETURN:
		return arC_syntax_tree_compile_return(c, (arC_syntax_tree_node_return*)st);
	case AR_SYNTAX_TREE_BINOP:
		return arC_syntax_tree_compile_binop(c, (arC_syntax_tree_node_binop*)st);
	case AR_SYNTAX_TREE_UNARYOP:
		return arC_syntax_tree_compile_unaryop(c, (arC_syntax_tree_node_unaryop*)st);
	case AR_SYNTAX_TREE_CONST_VALUE:
		return arC_syntax_tree_compile_const_value(c, (arC_syntax_tree_node_const_value*)st);
	case AR_SYNTAX_TREE_ASSIGN:
		return arC_syntax_tree_compile_assign(c, (arC_syntax_tree_node_assign*)st);
	case AR_SYNTAX_TREE_LOAD_LOCAL:
		return arC_syntax_tree_compile_load_local(c, (arC_syntax_tree_node_load_local*)st);
	case AR_SYNTAX_TREE_LOAD_ARGUMENT:
		return arC_syntax_tree_compile_load_arg(c, (arC_syntax_tree_node_load_arg*)st);
	case AR_SYNTAX_TREE_TYPE:
		return TRUE;
	case AR_SYNTAX_TREE_ERROR:
		return FALSE;
	default:
		return FALSE;
	}
	return TRUE;
}

arC_symbol* arC_symbol_find_symbol0(arC_syntax_tree* st, const arString* name, BOOL recursive, BOOL include_imports)
{
	// TODO: Add support for constants and global variables
	// 

	// 
	switch (st->type)
	{
	case AR_SYNTAX_TREE_TYPE:
	{
		arC_syntax_tree_node_type* const type = (arC_syntax_tree_node_type*)st;
		if (arC_symbol_has_name(asC_symbol(type->type), name)) {
			return asC_symbol(type->type);
		}
	}
	break;
	case AR_SYNTAX_TREE_FUNC:
	{
		arC_syntax_tree_node_func* const func = (arC_syntax_tree_node_func*)st;
		if (arC_symbol_has_name(asC_symbol(func->function), name)) {
			return asC_symbol(func->function);
		}

		// Check local function types
		arC_symbol* result = arC_func_find_symbol(func->function, name);
		if (result)
			return result;

		// Search all function children in root 
		if (recursive) {
			arC_syntax_tree* node = st->node;
			while (node) {
				arC_symbol* const symbol = arC_symbol_find_symbol0(node, name, FALSE, FALSE);
				if (symbol)
					return symbol;
				node = node->tail;
			}
		}
	}
	break;
	case AR_SYNTAX_TREE_IMPORT:
	{
		if (!include_imports)
			break;

		arC_syntax_tree_node_import* const import = (arC_syntax_tree_node_import*)st;
		if (arC_symbol_has_name(asC_symbol(import->package), name)) {
			return asC_symbol(import->package);
		}
		// Search the imported package after something with the supplied name
		arC_symbol* const symbol = arC_package_find_symbol(import->package, name);
		if (symbol) {
			return symbol;
		}
	}
	break;
	case AR_SYNTAX_TREE_PACKAGE:
	{
		arC_syntax_tree_node_package* const pkg = (arC_syntax_tree_node_package*)st;
		if (arC_symbol_has_name(asC_symbol(pkg->package), name)) {
			return asC_symbol(pkg->package);
		}
		// Search all children if requested
		if (recursive) {
			arC_syntax_tree* node = st->node;
			while (node) {
				arC_symbol* const symbol = arC_symbol_find_symbol0(node, name, FALSE, include_imports);
				if (symbol)
					return symbol;
				node = node->tail;
			}
		}
	}
	break;
	case AR_SYNTAX_TREE_ROOT:
	{
		arC_syntax_tree_root* const root = (arC_syntax_tree_root*)st;
		// Search all children in root 
		if (recursive) {
			arC_syntax_tree* node = st->node;
			while (node) {
				arC_symbol* const symbol = arC_symbol_find_symbol0(node, name, FALSE, FALSE);
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
		return arC_symbol_find_symbol0(st->parent, name, recursive, include_imports);
	}
	return NULL;
}

arC_symbol* arC_syntax_tree_find_symbol(arC_syntax_tree* st, const arString* name)
{
	return arC_symbol_find_symbol0(st, name, FALSE, TRUE);
}

arC_symbol* arC_syntax_tree_find_symbol_incl_imports(arC_syntax_tree* st, const arString* name)
{

	return arC_symbol_find_symbol0(st, name, TRUE, TRUE);
}

arC_syntax_tree_node_type* arC_syntax_tree_node_type_new(arC_type* type)
{
	arC_syntax_tree_node_type* p = arSafeMalloc(arC_syntax_tree_node_type);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_TYPE);
	p->type = type;
	return p;
}

void arC_syntax_tree_node_type_destroy(arC_syntax_tree* st)
{
	switch (st->type) {
	case AR_SYNTAX_TREE_ROOT:
		break;
	case AR_SYNTAX_TREE_PACKAGE:
		break;
	case AR_SYNTAX_TREE_FUNC:
		break;
	case AR_SYNTAX_TREE_TYPE:
		break;
	case AR_SYNTAX_TREE_RETURN:
		break;
	case AR_SYNTAX_TREE_CONST_VALUE:
		break;
	case AR_SYNTAX_TREE_ASSIGN:
		break;
	case AR_SYNTAX_TREE_BINOP:
		break;
	case AR_SYNTAX_TREE_UNARYOP:
		break;
	case AR_SYNTAX_TREE_LOAD_LOCAL:
		break;
	case AR_SYNTAX_TREE_LOAD_ARGUMENT:
		break;
	default:
		assert(FALSE); // Add 
	}

	arC_syntax_tree* node = st->node;
	while (node) {
		arC_syntax_tree_node tail = node->tail;
		arC_syntax_tree_node_type_destroy(node);
		node = tail;
	}

	arFree(st);
}

arC_syntax_tree_node_unaryop* arC_syntax_tree_unaryop_new(arC_syntax_tree_node left, arC_tokens op)
{
	arC_syntax_tree_node_unaryop* const p = arSafeMalloc(arC_syntax_tree_node_unaryop);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_UNARYOP);
	p->header.stack_type = left->stack_type;
	p->op = op;
	p->closest_function_node = NULL;
	arC_syntax_tree_add(asC_syntax_tree(p), left);
	return p;
}

arC_syntax_tree_node_binop* arC_syntax_tree_binop_new(arC_syntax_tree_node left, arC_syntax_tree_node right, arC_tokens op)
{
	arC_syntax_tree_node_binop* const p = arSafeMalloc(arC_syntax_tree_node_binop);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_BINOP);
	p->header.stack_type = right->stack_type;
	p->op = op;
	p->closest_function_node = NULL;
	arC_syntax_tree_add(asC_syntax_tree(p), left);
	arC_syntax_tree_add(asC_syntax_tree(p), right);
	return p;
}

arC_syntax_tree_node_const_value* arC_syntax_tree_const_value_new()
{
	arC_syntax_tree_node_const_value* const p = arSafeMalloc(arC_syntax_tree_node_const_value);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_CONST_VALUE);
	p->closest_function_node = NULL;
	p->value.type = 0;
	return p;
}

arC_syntax_tree_node_load_local* arC_syntax_tree_load_local_new()
{
	arC_syntax_tree_node_load_local* const p = arSafeMalloc(arC_syntax_tree_node_load_local);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_LOAD_LOCAL);
	p->target = NULL;
	p->closest_function_node = NULL;
	return p;
}

arC_syntax_tree_node_load_arg* arC_syntax_tree_load_argument_new()
{
	arC_syntax_tree_node_load_arg* const p = arSafeMalloc(arC_syntax_tree_node_load_arg);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_LOAD_ARGUMENT);
	p->target = NULL;
	p->closest_function_node = NULL;
	return p;
}

arC_syntax_tree_node_return* arC_syntax_tree_return_new()
{
	arC_syntax_tree_node_return* const p = arSafeMalloc(arC_syntax_tree_node_return);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_RETURN);
	return p;
}

arC_syntax_tree_node_assign* arC_syntax_tree_assign_new(arC_syntax_tree_node expr, arC_symbol* target)
{
	arC_syntax_tree_node_assign* const p = arSafeMalloc(arC_syntax_tree_node_assign);
	arC_syntax_tree_init(asC_syntax_tree(p), AR_SYNTAX_TREE_ASSIGN);
	p->target = target;
	arC_syntax_tree_add(asC_syntax_tree(p), expr);
	return p;
}

arC_syntax_tree_node arC_synax_tree_parse_comp_expr(arCompiler* c, arC_token* t, const arC_state* s);
arC_syntax_tree_node arC_synax_tree_parse_term(arCompiler* c, arC_token* t, const arC_state* state);
arC_syntax_tree_node arC_synax_tree_parse_factor(arCompiler* c, arC_token* t, const arC_state* state);
arC_syntax_tree_node arC_synax_tree_parse_atom(arCompiler* c, arC_token* t, const arC_state* state);

arC_syntax_tree_node arC_synax_tree_out_of_memory(const arC_state* s)
{
	arC_message_out_of_memory(s);
	return arC_syntax_tree_error();
}

arC_syntax_tree_node arC_synax_tree_not_implemented(const arC_state* s)
{
	arC_message_not_implemented(s);
	return arC_syntax_tree_error();
}

arC_syntax_tree_node arC_synax_tree_syntax_error(const arC_state* s, const char* msg)
{
	arC_message_syntax_error(s, "expected ,");
	return arC_syntax_tree_error();
}

// A generic unary operator parser
arC_syntax_tree_node arC_synax_tree_parse_unaryop(arCompiler* const c, arC_token* const t, const arC_state* const s,
	arC_tokens token_type, arC_syntax_tree_parse_fn right_fn) {
	arC_token_next(t);
	arC_syntax_tree_node right = right_fn(c, t, s);
	if (arC_syntax_tree_is_error(right))
		return right;

	// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
	// node into a new constant node instead
	right = arC_synax_tree_merge_children(s, right);

	arC_syntax_tree_node_unaryop* const unaryop = arC_syntax_tree_unaryop_new(right, token_type);
	if (unaryop == NULL)
		return arC_synax_tree_out_of_memory(s);
	unaryop->closest_function_node = s->func_node;

	// Merge the unaryop
	return arC_synax_tree_merge_children(s, asC_syntax_tree(unaryop));
}

// A generic binary operator parser
arC_syntax_tree_node arC_synax_tree_parse_binop(arCompiler* const c, arC_token* const t, const arC_state* const s,
	const arC_tokens* const types, int types_count, arC_syntax_tree_parse_fn left_fn, arC_syntax_tree_parse_fn right_fn) {
	if (right_fn == NULL)
		right_fn = left_fn;
	arC_syntax_tree_node left = left_fn(c, t, s);
	if (arC_syntax_tree_is_error(left))
		return left;

	// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
	// node into a new constant node instead
	left = arC_synax_tree_merge_children(s, left);

	while (1) {
		arC_tokens const token_type = t->type;
		int i;
		for (i = 0; i < types_count; ++i) {
			if (token_type == types[i]) {
				arC_token_next(t);
				arC_syntax_tree_node right = right_fn(c, t, s);
				if (arC_syntax_tree_is_error(right))
					return right;
				
				// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
				// node into a new constant node instead
				right = arC_synax_tree_merge_children(s, right);

				arC_syntax_tree_node_binop* const binop = arC_syntax_tree_binop_new(left, right, token_type);
				if (binop == NULL)
					return arC_synax_tree_out_of_memory(s);
				binop->closest_function_node = s->func_node;

				// Merge the actual binop
				left = arC_synax_tree_merge_children(s, asC_syntax_tree(binop));
				break;
			}
		}
		if (i == types_count)
			break;
	}
	return left;
}

// Types that represents binary operators
const arC_tokens ops_compares[] = { ARTOK_TEST_EQ, ARTOK_TEST_NEQ, ARTOK_TEST_LT, ARTOK_TEST_LTE, ARTOK_TEST_GT, ARTOK_TEST_GTW };
const arC_tokens ops_plus_minus[] = { ARTOK_OP_MINUS, ARTOK_OP_PLUS };
const arC_tokens ops_mult_div[] = { ARTOK_OP_MULT, ARTOK_OP_DIV };

// Create a binary operator bitwise
arC_syntax_tree_node arC_synax_tree_parse_binop_bitwise(arCompiler* c, arC_token* t, const arC_state* state, arC_syntax_tree_parse_fn left_fn,
	arC_syntax_tree_parse_fn right_fn)
{
	arC_syntax_tree_node left = left_fn(c, t, state);
	if (arC_syntax_tree_is_error(left))
		return left;

	// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
	// node into a new constant node instead
	left = arC_synax_tree_merge_children(state, left);

	while (1) {
		switch (t->type) {
		case ARTOK_BIT_AND:
		{
			const arC_tokens token_type = t->type;
			arC_token_next(t);
			arC_syntax_tree_node right = right_fn(c, t, state);
			if (arC_syntax_tree_is_error(right))
				return right;

			// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
			// node into a new constant node instead
			right = arC_synax_tree_merge_children(state, right);

			arC_syntax_tree_node_binop* binop = arC_syntax_tree_binop_new(left, right, token_type);
			if (binop == NULL) {
				arC_message_out_of_memory(state);
				return arC_syntax_tree_error();
			}
			binop->closest_function_node = state->func_node;

			// Merge the actual binop
			left = arC_synax_tree_merge_children(state, asC_syntax_tree(binop));
			break;
		}
		default:
			goto end;
		}
	}
end:
	return left;
}

const arString* arC_int32_type_name() {
	static const char name[] = "int32";
	static const arString str = { name, name + 5 };
	return &str;
}

const arString* arC_float64_type_name() {
	static const char name[] = "float64";
	static const arString str = { name, name + 7 };
	return &str;
}

arC_syntax_tree_node arC_synax_tree_parse_atom(arCompiler* c, arC_token* t, const arC_state* state)
{
	if (t->type == ARTOK_VALUE_INT) {
		arC_syntax_tree_node_const_value* const val = arC_syntax_tree_const_value_new();
		if (val == NULL)
			return arC_synax_tree_out_of_memory(state);
		val->closest_function_node = state->func_node;
		asC_syntax_tree_stack_type(val) = arC_syntax_tree_find_symbol_incl_imports(state->parent_node, arC_int32_type_name());
		val->value.i32 = arC_token_i4(t);
		// TODO: Constant types are always known. They might have a parent CAST unaryop though
		val->value.type = arC_symbol_get_data_type(val->header.stack_type);
		arC_token_next(t);
		return asC_syntax_tree(val);
	}
	else if (t->type == ARTOK_VALUE_DECIMAL) {
		arC_syntax_tree_node_const_value* const val = arC_syntax_tree_const_value_new();
		if (val == NULL)
			return arC_synax_tree_out_of_memory(state);
		val->closest_function_node = state->func_node;
		asC_syntax_tree_stack_type(val) = arC_syntax_tree_find_symbol_incl_imports(state->parent_node, arC_float64_type_name());
		val->value.f64 = arC_token_f8(t);
		// TODO: Constant types are always known. They might have a parent CAST unaryop though
		val->value.type = arC_symbol_get_data_type(val->header.stack_type);
		arC_token_next(t);
		return asC_syntax_tree(val);
	}
	if (t->type == ARTOK_IDENTITY) {
		const arString identity = t->string;
		arC_symbol* symbol = arC_func_find_symbol(state->func_node->function, &identity);
		arC_token_next(t);

		if (t->type == ARTOK_DECL_ASSIGN) {
			arC_token_next(t);

			// Check to see if the local variable exists, and if not then add it
			if (symbol != NULL) {
				// Symbol is already defined
				arC_message_not_implemented(state);
				return arC_syntax_tree_error();
			}

			// Expression to be set in the local variable
			arC_syntax_tree_node decl_expression = arC_synax_tree_parse_comp_expr(c, t, state);
			if (arC_syntax_tree_is_error(decl_expression))
				return decl_expression;

			// Define the local variable
			arC_local* const local = arC_local_new(&identity);
			if (local == NULL) {
				arC_message_out_of_memory(state);
				return arC_syntax_tree_error();
			}
			local->type = decl_expression->stack_type;
			local->header.name = identity;
			arC_func_add_local(state->func_node->function, local);

			arC_syntax_tree_node_assign* assign = arC_syntax_tree_assign_new(decl_expression, asC_symbol(local));
			if (assign == NULL) {
				arC_message_out_of_memory(state);
				return arC_syntax_tree_error();
			}
			assign->closest_function_node = state->func_node;
			return asC_syntax_tree(assign);
		}
		else if (t->type == ARTOK_ASSIGN) {
			// Existing variable, might be a local, argument or a global variable
			arC_token_next(t);

			// Check to see if the local variable exists, and if not then add it
			if (symbol == NULL) {
				// Not defined yet
				// TODO: Add support for an "unresolved" assignable type
				arC_message_not_implemented(state);
				return arC_syntax_tree_error();
			}

			// Expression to be set in the local variable
			arC_syntax_tree_node decl_expression = arC_synax_tree_parse_comp_expr(c, t, state);
			if (arC_syntax_tree_is_error(decl_expression))
				return decl_expression;

			arC_syntax_tree_node_assign* assign = arC_syntax_tree_assign_new(decl_expression, symbol);
			if (assign == NULL) {
				arC_message_out_of_memory(state);
				return arC_syntax_tree_error();
			}
			assign->closest_function_node = state->func_node;
			return asC_syntax_tree(assign);
		}
		else {
			if (symbol == NULL) {
				// TODO: Add support for "unresolved" types (which is later on resolved as the "closest" type)
				arC_message_not_implemented(state);
				return arC_syntax_tree_error();
			}

			arC_syntax_tree_node result = NULL;
			// Load a local variable or an argument
			if (symbol->type == arC_SYMBOL_LOCAL) {
				arC_syntax_tree_node_load_local* const load = arC_syntax_tree_load_local_new();
				if (load == NULL) {
					arC_message_out_of_memory(state);
					return arC_syntax_tree_error();
				}
				load->closest_function_node = state->func_node;
				load->target = (arC_local*)symbol;
				result = asC_syntax_tree(load);
				result->stack_type = load->target->type;
			}
			else if (symbol->type == arC_SYMBOL_ARGUMENT) {
				arC_syntax_tree_node_load_arg* const load = arC_syntax_tree_load_argument_new();
				if (load == NULL) {
					arC_message_out_of_memory(state);
					return arC_syntax_tree_error();
				}
				load->closest_function_node = state->func_node;
				load->target = (arC_arg*)symbol;
				result = asC_syntax_tree(load);
				result->stack_type = load->target->type;
			}

			// Unknown variable type
			if (result == NULL) {
				arC_message_not_implemented(state);
				return arC_syntax_tree_error();
			}

			return result;
		}
	}
	else if (t->type == ARTOK_PARAN_L) {
		arC_token_next(t);
		arC_syntax_tree_node node = arC_syntax_tree_parse_keywords(c, t, state);
		if (arC_syntax_tree_is_error(node))
			return node;
		// Expected right paranthesis
		if (t->type != ARTOK_PARAN_R) {
			arC_message_syntax_error(state, "expected }");
			return arC_syntax_tree_error();
		}
		arC_token_next(t);
		return node;
	}

	arC_message_syntax_error(state, "expected number, decimal, +, -, (");
	return arC_syntax_tree_error();
}

arC_syntax_tree_node arC_synax_tree_parse_factor(arCompiler* c, arC_token* t, const arC_state* s)
{
	switch (t->type)
	{
	case ARTOK_OP_MINUS:
	case ARTOK_OP_PLUS:
	case ARTOK_BIT_NOT:
		return arC_synax_tree_parse_unaryop(c, t, s, t->type, arC_synax_tree_parse_factor);
	}

	return arC_synax_tree_parse_atom(c, t, s);
}

arC_syntax_tree_node arC_synax_tree_parse_term(arCompiler* c, arC_token* t, const arC_state* s)
{
	return arC_synax_tree_parse_binop(c, t, s, ops_mult_div, 2, 
		arC_synax_tree_parse_factor, arC_synax_tree_parse_factor);
}

arC_syntax_tree_node arC_synax_tree_parse_expression(arCompiler* c, arC_token* t, const arC_state* s)
{
	return arC_synax_tree_parse_binop(c, t, s, ops_plus_minus, 2, 
		arC_synax_tree_parse_term, arC_synax_tree_parse_term);
}

arC_syntax_tree_node arC_synax_tree_parse_comp_expr(arCompiler* c, arC_token* t, const arC_state* s)
{
	if (t->type == ARTOK_NOT) {
		return arC_synax_tree_parse_unaryop(c, t, s, t->type, arC_synax_tree_parse_comp_expr);
	}

	return arC_synax_tree_parse_binop(c, t, s, ops_compares, 6, 
		arC_synax_tree_parse_expression, arC_synax_tree_parse_expression);
}

arC_syntax_tree_node arC_syntax_tree_parse_keywords(arCompiler* c, arC_token* t, const arC_state* s)
{
	if (arC_token_is_keyword(t)) {
		switch (t->type) {
		case ARTOK_KEYWORD_RETURN:
		{
			arC_token_next(t);
			arC_syntax_tree_node_return* ret = arC_syntax_tree_return_new();
			if (ret == NULL)
				return arC_synax_tree_out_of_memory(s);
			ret->closest_function_node = s->func_node;

			// The number of expressions that are left
			arInt32 return_expressions_left = arC_func_count_returns(arCompiler_state_get_func(s));
			arC_return* return_symbol = s->func_node->function->returns;

			// Fetch all return statements 
			// TODO: Allow for skipping return values and let the compiler return the default value for you
			while (return_expressions_left > 0) {
				arC_syntax_tree_node return_expr = arC_synax_tree_parse_comp_expr(c, t, s);
				if (arC_syntax_tree_is_error(return_expr))
					return return_expr;
				// Spearate each statement with a comma
				if (return_expressions_left > 1) {
					if (t->type != ARTOK_COMMA)
						return arC_synax_tree_syntax_error(s, "expected ,");
					arC_token_next(t);
				}
				// Verify that the return_expression results in the same type as the return type
				arC_syntax_tree_add(asC_syntax_tree(ret), return_expr);

				if (!arC_symbol_equals(return_expr->stack_type, return_symbol->type)) {
					// Add a cast tree node
					// TODO: Traverse and search for constants that can be merged?
				}
				return_symbol = return_symbol->tail;
				--return_expressions_left;
			}
			return asC_syntax_tree(ret);
		}
		break;
		case ARTOK_KEYWORD_VAR:
			return arC_synax_tree_not_implemented(s);
		default:
			arC_message_not_implemented(s);
			return arC_syntax_tree_error();
		}
	}

	return arC_synax_tree_parse_comp_expr(c, t, s);
}
