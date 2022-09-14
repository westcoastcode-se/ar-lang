#include "syntax_tree.h"
#include "tokens.h"
#include "optimizations.h"
#include "messages.h"
#include "../arMemory.h"
#include "../arCompiler.h"
#include "../arBuilder.h"

arC_syntax_tree_node arC_syntax_tree_error()
{
	static arC_syntax_tree instance = { arC_SYNTAX_TREE_ERROR, NULL, NULL, NULL };
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

BOOL arC_syntax_tree_has_children(arC_syntax_tree_node st)
{
	return st->node != NULL;
}

arC_syntax_tree_node arC_syntax_tree_get_parent(arC_syntax_tree_node st)
{
	return st->parent;
}

// Get the underlying arC_func function representation from the supplied state
arC_func* arCompiler_state_get_func(const arC_state* s)
{
	return s->func_node->symbol;
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

arC_syntax_tree_node arC_syntax_tree_find_incl_parents(arC_syntax_tree_node s, arInt32 types)
{
	if ((BIT(s->type) & types) != 0)
		return s;
	s = s->parent;
	while (s) {
		if ((BIT(s->type) & types) != 0) {
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

BOOL arC_compiler_out_of_memory(arCompiler* c)
{
	const arC_state state = { c, NULL, NULL, NULL, NULL, NULL };
	return arC_message_out_of_memory(&state);
}

BOOL arC_compiler_symbol_unresolved(arCompiler* c, const arString* name)
{
	const arC_state state = { c, NULL, NULL, NULL, NULL, NULL };
	return arC_message_symbol_unresolved(&state, name);
}

BOOL arC_syntax_tree_compile_package(arCompiler* c, arC_syntax_tree_node_package* st)
{
	ASSERT_NOT_NULL(st->symbol->package);
	arBuilder_add_package(c->pipeline, st->symbol->package);

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
	if (!arC_syntax_tree_has_children(asC_syntax_tree(st))) {
		return arC_compiler_symbol_unresolved(c, asC_symbol_name(st->symbol));
	}

	arB_func_body_begin(st->symbol->func);
	arC_syntax_tree* node = asC_syntax_tree(st)->node;
	while (node != NULL) {
		if (!arC_syntax_tree_compile(c, node))
			return FALSE;
		node = node->tail;
	}
	arB_func_body_end(st->symbol->func);
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
	arB_func_add_instr(st->closest_function_node->symbol->func, arB_instr_ret());
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
	arC_type* const stack_type = (arC_type*)asC_syntax_tree_stack_type(binop);
	ASSERT_NOT_NULL(stack_type);
	switch (binop->op)
	{
	case ARTOK_OP_MINUS:
		arB_func_add_instr(binop->closest_function_node->symbol->func,
			arB_instr_sub(stack_type->type));
		return TRUE;
	case ARTOK_OP_PLUS:
		arB_func_add_instr(binop->closest_function_node->symbol->func,
			arB_instr_add(stack_type->type));
		return TRUE;
	case ARTOK_OP_MULT:
		arB_func_add_instr(binop->closest_function_node->symbol->func,
			arB_instr_mul(stack_type->type));
		return TRUE;
	case ARTOK_OP_DIV:
		arB_func_add_instr(binop->closest_function_node->symbol->func,
			arB_instr_div(stack_type->type));
		return TRUE;
	case ARTOK_TEST_LT:
		arB_func_add_instr(binop->closest_function_node->symbol->func,
			arB_instr_clt(stack_type->type));
		return TRUE;
	case ARTOK_TEST_GT:
		arB_func_add_instr(binop->closest_function_node->symbol->func,
			arB_instr_cgt(stack_type->type));
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
	arC_type* const stack_type = (arC_type*)asC_syntax_tree_stack_type(unaryop);
	ASSERT_NOT_NULL(stack_type);
	switch (unaryop->op)
	{
	case ARTOK_OP_MINUS:
		arB_func_add_instr(unaryop->closest_function_node->symbol->func,
			arB_instr_neg(stack_type->type));
		return TRUE;
	case ARTOK_BIT_NOT:
		arB_func_add_instr(unaryop->closest_function_node->symbol->func,
			arB_instr_bit_not(stack_type->type));
		return TRUE;
	default:
		// TODO: Add support for alternate operators
		return FALSE;
	}
}

BOOL arC_syntax_tree_compile_const_value(arCompiler* c, arC_syntax_tree_node_const_value* val)
{
	arC_type* const stack_type = (arC_type*)asC_syntax_tree_stack_type(val);
	ASSERT_NOT_NULL(stack_type);

	// ldc
	arB_func_add_instr(val->closest_function_node->symbol->func,
		arB_instr_ldc(stack_type->type, val->value));
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
		arB_func_add_instr(assign->closest_function_node->symbol->func, arB_instr_stl(local->local));
		return TRUE;
	}
	// Unknown assign expression
	return FALSE;
}

BOOL arC_syntax_tree_compile_load_local(arCompiler* c, arC_syntax_tree_node_load_local* local)
{
	arB_func_add_instr(local->closest_function_node->symbol->func, arB_instr_ldl(local->target->local));
	return TRUE;
}

BOOL arC_syntax_tree_compile_load_arg(arCompiler* c, arC_syntax_tree_node_load_arg* arg)
{
	arB_func_add_instr(arg->closest_function_node->symbol->func, arB_instr_lda(arg->target->arg));
	return TRUE;
}

void arC_syntax_tree_node_destroy(arC_syntax_tree* st)
{
	switch (st->type)
	{
	case arC_SYNTAX_TREE_PACKAGE:
	{
		arC_syntax_tree_node_package* package = (arC_syntax_tree_node_package*)st;
		if (package->symbol) {
			arC_package_destroy(package->symbol);
			package->symbol = NULL;
		}
	}
	break;
	}

	arC_syntax_tree* node = st->node;
	while (node) {
		arC_syntax_tree_node tail = node->tail;
		arC_syntax_tree_node_destroy(node);
		node = tail;
	}

	arFree(st);
}

arC_syntax_tree_node_package* arC_syntax_tree_node_package_new(const arString* name)
{
	arC_package* const symbol = arC_package_new(name);
	if (symbol == NULL)
		return NULL;
	arC_syntax_tree_node_package* const p = arMalloc(sizeof(arC_syntax_tree_node_package));
	if (p == NULL) {
		arFree(symbol);
		return NULL;
	}
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_PACKAGE);
	p->symbol = symbol;
	return p;
}

arC_syntax_tree_node_func* arC_syntax_tree_node_func_new(const arC_func_sign* signature)
{
	arC_func* const symbol = arC_func_new(signature);
	if (symbol == NULL)
		return NULL;

	arC_syntax_tree_node_func* const p = arMalloc(sizeof(arC_syntax_tree_node_func));
	if (p == NULL) {
		arFree(symbol);
		return NULL;
	}
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_FUNC);
	p->symbol = symbol;
	p->closest_package_node = NULL;
	return p;
}

arC_syntax_tree_node_import* arC_syntax_tree_node_import_new(arC_syntax_tree_node_package* package)
{
	arC_syntax_tree_node_import* const p = arSafeMalloc(arC_syntax_tree_node_import);
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_IMPORT);
	p->alias = *asC_symbol_name(package->symbol);
	p->package = package;
	return p;
}

BOOL arC_syntax_tree_node_import_root(arC_syntax_tree_node_package* package, const arC_state* s)
{
	arC_syntax_tree_node_import* import = arC_syntax_tree_node_import_new(s->compiler->root_node);
	if (import == NULL)
		return arC_message_out_of_memory(s);
	arC_syntax_tree_add(asC_syntax_tree(package), asC_syntax_tree(import));
	return TRUE;
}

BOOL arC_syntax_tree_compile(arCompiler* c, arC_syntax_tree* st)
{
	switch (st->type) {
	case arC_SYNTAX_TREE_PACKAGE:
		return arC_syntax_tree_compile_package(c, (arC_syntax_tree_node_package*)st);
	case arC_SYNTAX_TREE_FUNC:
		return arC_syntax_tree_compile_func(c, (arC_syntax_tree_node_func*)st);
	case arC_SYNTAX_TREE_RETURN:
		return arC_syntax_tree_compile_return(c, (arC_syntax_tree_node_return*)st);
	case arC_SYNTAX_TREE_BINOP:
		return arC_syntax_tree_compile_binop(c, (arC_syntax_tree_node_binop*)st);
	case arC_SYNTAX_TREE_UNARYOP:
		return arC_syntax_tree_compile_unaryop(c, (arC_syntax_tree_node_unaryop*)st);
	case arC_SYNTAX_TREE_CONST_VALUE:
		return arC_syntax_tree_compile_const_value(c, (arC_syntax_tree_node_const_value*)st);
	case arC_SYNTAX_TREE_ASSIGN:
		return arC_syntax_tree_compile_assign(c, (arC_syntax_tree_node_assign*)st);
	case arC_SYNTAX_TREE_LOAD_LOCAL:
		return arC_syntax_tree_compile_load_local(c, (arC_syntax_tree_node_load_local*)st);
	case arC_SYNTAX_TREE_LOAD_ARGUMENT:
		return arC_syntax_tree_compile_load_arg(c, (arC_syntax_tree_node_load_arg*)st);
	case arC_SYNTAX_TREE_IMPORT:
	case arC_SYNTAX_TREE_TYPE:
		return TRUE;
	case arC_SYNTAX_TREE_ERROR:
		return FALSE;
	default:
		return FALSE;
	}
	return TRUE;
}

arC_syntax_tree_node arC_syntax_tree_find_incl_imports0(arC_syntax_tree_node node, const arString* name, arInt32 types, BOOL recursive)
{
	ASSERT_NOT_NULL(node);
	ASSERT_NOT_NULL(name);

	// Is this a type of the same type that we are looking for? Then see if it's this node that has the supplied name
	if ((BIT(node->type) & types) != 0)
	{
		switch (node->type)
		{
		case arC_SYNTAX_TREE_TYPE:
			if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_type*)node)->symbol), name)) {
				return node;
			}
			break;
		case arC_SYNTAX_TREE_FUNC:
			if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_func*)node)->symbol), name)) {
				return node;
			}
			break;
		case arC_SYNTAX_TREE_IMPORT:
			if (arString_cmp(&((arC_syntax_tree_node_import*)node)->alias, name)) {
				return node;
			}
			break;
		case arC_SYNTAX_TREE_PACKAGE:
			if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_package*)node)->symbol), name)) {
				return node;
			}
			break;
		default:
			break;
		}
	}

	if (recursive) {
		arC_syntax_tree_node child = node->node;
		while (child != NULL) {
			arC_syntax_tree_node result = arC_syntax_tree_find_incl_imports0(child, name, types, FALSE);
			if (result != NULL)
				return result;
			child = child->tail;
		}
		if (node->parent != NULL) {
			return arC_syntax_tree_find_incl_imports0(node->parent, name, types, recursive);
		}
	}

	return NULL;
}

arC_syntax_tree_node arC_syntax_tree_find_incl_imports(arC_syntax_tree_node node, const arString* name, arInt32 types)
{
	ASSERT_NOT_NULL(node);
	ASSERT_NOT_NULL(name);
	if (types == 0)
		return NULL;
	return arC_syntax_tree_find_incl_imports0(node, name, types, TRUE);
}

arC_syntax_tree_node arC_syntax_tree_find_child_with_type(arC_syntax_tree_node node, const arString* name, arInt32 types)
{
	arC_syntax_tree_node child = node->node;
	while (child) {
		if ((child->type & types) != 0) {
			switch (child->type) {
			case arC_SYNTAX_TREE_TYPE:
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_type*)child)->symbol), name)) {
					return child;
				}
				break;
			case arC_SYNTAX_TREE_FUNC:
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_func*)child)->symbol), name)) {
					return child;
				}
				break;
			case arC_SYNTAX_TREE_IMPORT:
				if (arString_cmp(&((arC_syntax_tree_node_import*)child)->alias, name)) {
					return child;
				}
				break;
			case arC_SYNTAX_TREE_PACKAGE:
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_package*)child)->symbol), name)) {
					return child;
				}
				break;
			default:
				break;
			}
		}
		child = child->tail;
	}
	return NULL;
}

arC_syntax_tree_node_type* arC_syntax_tree_node_type_new(const arString* name)
{
	arC_type* const symbol = arC_type_new(name);
	if (symbol == NULL)
		return NULL;
	arC_syntax_tree_node_type* const p = arMalloc(sizeof(arC_syntax_tree_node_type));
	if (p == NULL) {
		arFree(symbol);
		return NULL;
	}
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_TYPE);
	p->symbol = symbol;
	return p;
}

void arC_syntax_tree_node_type_set_symbol(arC_syntax_tree_node_type* node, arC_type* type)
{
	node->symbol = type;
}

arC_syntax_tree_node_type* arC_syntax_tree_node_type_parse(arC_token* t, const arC_state* s)
{
	// Is this a pointer type - for example: *int32
	if (t->type == ARTOK_PTR) {
		arC_token_next(t);
		// Fetch the type the pointer is based on. If NULL then an error has occurred
		arC_syntax_tree_node_type* of_type = arC_syntax_tree_node_type_parse(t, s);
		if (of_type == NULL)
			return NULL;

		// TODO: Implement the rest!
		arC_message_not_implemented(s);
		return NULL;
	}
	// Is this an array type - for example: [2]int32
	else if (t->type == ARTOK_SQUARE_L) {
		// TODO: Implement the rest!
		arC_message_not_implemented(s);
		return NULL;
	}

	if (t->type != ARTOK_IDENTITY) {
		arC_message_expected_identifier(s);
		return NULL;
	}

	arC_syntax_tree_node_package* closet_package_node = s->package_node;

	//
	// There are some rules regarding where types are allowed to be defined:
	// 
	// 1. Types in a package can be defined anywhere in the package
	// 2. Types inside functions MUST be defined before the type is used
	// 3. Types are not allowed to be inside other types
	// 4. Imports should only be considered on the first level of access

	arC_syntax_tree_node parent_node = s->parent_node;
	arString name = t->string;
	arC_syntax_tree_node node = arC_syntax_tree_find_incl_imports(parent_node, &name, BIT(arC_SYNTAX_TREE_TYPE) | BIT(arC_SYNTAX_TREE_PACKAGE));
	if (node == NULL) {
		arC_token_next(t);
		// This might be a package or a type. If the next token is a dot then 
		// assume that the type we searched for is a package. If it's a package then all searches we are doing after this should 
		// only include children
		if (t->type == ARTOK_DOT) {
			arC_syntax_tree_node_package* const package_node = arC_syntax_tree_node_package_new(&name);
			if (package_node == NULL) {
				arC_message_out_of_memory(s);
				return NULL;
			}
			if (!arC_syntax_tree_node_import_root(package_node, s))
				return FALSE;
			node = asC_symbol(package_node);
			arC_syntax_tree_add(asC_symbol(closet_package_node), node);
			arC_package_add_package(closet_package_node->symbol, package_node->symbol);
			if (!arC_package_build_signature(package_node->symbol, s))
				return FALSE;
			closet_package_node = package_node;
		}
		else {
			arC_syntax_tree_node_type* const type_node = arC_syntax_tree_node_type_new(&name);
			if (type_node == NULL) {
				arC_message_out_of_memory(s);
				return NULL;
			}
			node = asC_symbol(type_node);
			arC_syntax_tree_add(parent_node, node);
			arC_package_add_type(closet_package_node->symbol, type_node->symbol);
		}
	}

	if (node != NULL && node->type == arC_SYNTAX_TREE_PACKAGE) {
		// If this is a package, then assume we have the format: Package1.Package2.Type
		while (TRUE) {
			parent_node = node;
			name = t->string;
			node = arC_syntax_tree_find_child_with_type(parent_node, &name, BIT(arC_SYNTAX_TREE_TYPE) | BIT(arC_SYNTAX_TREE_PACKAGE));
			arC_token_next(t);
			if (node == NULL) {
				if (t->type == ARTOK_DOT) {
					arC_syntax_tree_node_package* const package_node = arC_syntax_tree_node_package_new(&name);
					if (package_node == NULL) {
						arC_message_out_of_memory(s);
						return NULL;
					}
					if (!arC_syntax_tree_node_import_root(package_node, s))
						return FALSE;
					node = asC_symbol(package_node);
					arC_syntax_tree_add(parent_node, node);
					arC_package_add_package(closet_package_node->symbol, package_node->symbol);
					if (!arC_package_build_signature(package_node->symbol, s))
						return FALSE;
					closet_package_node = package_node;
				}
			}

			if (t->type != ARTOK_DOT)
				break;
		}

		// If we never found a node at the end of the package-chain then assume that that's a type (that's not defined yet)
		if (node == NULL) {
			// Prepare a node for the package
			arC_syntax_tree_node_type* const type_node = arC_syntax_tree_node_type_new(&name);
			if (type_node == NULL) {
				arC_message_out_of_memory(s);
				return NULL;
			}
			node = asC_symbol(type_node);
			arC_syntax_tree_add(parent_node, node);
			arC_package_add_type(closet_package_node->symbol, type_node->symbol);
		}
	}
	arC_token_next(t);
	assert(node->type == arC_SYNTAX_TREE_TYPE);
	return (arC_syntax_tree_node_type*)node;
}

BOOL arC_syntax_tree_node_resolve(arC_syntax_tree_node node, const struct arC_state* s)
{
	ASSERT_NOT_NULL(node);
	ASSERT_NOT_NULL(s);

	switch (node->type)
	{
	case arC_SYNTAX_TREE_PACKAGE:
		if (!arC_package_resolve(((arC_syntax_tree_node_package*)node)->symbol, s)) {
			return FALSE;
		}
		break;
	case arC_SYNTAX_TREE_TYPE:
		if (!arC_type_resolve(((arC_syntax_tree_node_type*)node)->symbol, s)) {
			return FALSE;
		}
		break;
	case arC_SYNTAX_TREE_FUNC:
		if (!arC_func_resolve(((arC_syntax_tree_node_func*)node)->symbol, s)) {
			return FALSE;
		}
		break;
	}

	arC_syntax_tree_node child = node->node;
	while (child) {
		if (!arC_syntax_tree_node_resolve(child, s))
			return FALSE;
		child = child->tail;
	}
	return TRUE;
}

arC_syntax_tree_node_unaryop* arC_syntax_tree_unaryop_new(arC_syntax_tree_node left, arC_tokens op)
{
	arC_syntax_tree_node_unaryop* const p = arSafeMalloc(arC_syntax_tree_node_unaryop);
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_UNARYOP);
	p->header.stack_type = left->stack_type;
	p->op = op;
	p->closest_function_node = NULL;
	arC_syntax_tree_add(asC_syntax_tree(p), left);
	return p;
}

arC_syntax_tree_node_binop* arC_syntax_tree_binop_new(arC_syntax_tree_node left, arC_syntax_tree_node right, arC_tokens op)
{
	arC_syntax_tree_node_binop* const p = arSafeMalloc(arC_syntax_tree_node_binop);
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_BINOP);
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
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_CONST_VALUE);
	p->closest_function_node = NULL;
	p->value.type = 0;
	return p;
}

arC_syntax_tree_node_load_local* arC_syntax_tree_load_local_new()
{
	arC_syntax_tree_node_load_local* const p = arSafeMalloc(arC_syntax_tree_node_load_local);
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_LOAD_LOCAL);
	p->target = NULL;
	p->closest_function_node = NULL;
	return p;
}

arC_syntax_tree_node_load_arg* arC_syntax_tree_load_argument_new()
{
	arC_syntax_tree_node_load_arg* const p = arSafeMalloc(arC_syntax_tree_node_load_arg);
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_LOAD_ARGUMENT);
	p->target = NULL;
	p->closest_function_node = NULL;
	return p;
}

arC_syntax_tree_node_return* arC_syntax_tree_return_new()
{
	arC_syntax_tree_node_return* const p = arSafeMalloc(arC_syntax_tree_node_return);
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_RETURN);
	return p;
}

arC_syntax_tree_node_assign* arC_syntax_tree_assign_new(arC_syntax_tree_node expr, arC_symbol* target)
{
	arC_syntax_tree_node_assign* const p = arSafeMalloc(arC_syntax_tree_node_assign);
	arC_syntax_tree_init(asC_syntax_tree(p), arC_SYNTAX_TREE_ASSIGN);
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

CONST_VM_STRING(arC_syntax_tree, int32, "int32", 5);
CONST_VM_STRING(arC_syntax_tree, float64, "float64", 7);

arC_syntax_tree_node arC_synax_tree_parse_atom(arCompiler* c, arC_token* t, const arC_state* state)
{
	if (t->type == ARTOK_VALUE_INT) {
		arC_syntax_tree_node_const_value* const val = arC_syntax_tree_const_value_new();
		if (val == NULL)
			return arC_synax_tree_out_of_memory(state);
		val->closest_function_node = state->func_node;
		arC_syntax_tree_node_type* stack_type_node =
			(arC_syntax_tree_node_type*)arC_syntax_tree_find_incl_imports(state->parent_node, 
				GET_CONST_VM_STRING(arC_syntax_tree, int32), BIT(arC_SYNTAX_TREE_TYPE));
		asC_syntax_tree_stack_type(val) = stack_type_node->symbol;
		val->value.i32 = arC_token_i4(t);
		val->value.type = val->header.stack_type->data_type;
		arC_token_next(t);
		return asC_syntax_tree(val);
	}
	else if (t->type == ARTOK_VALUE_DECIMAL) {
		arC_syntax_tree_node_const_value* const val = arC_syntax_tree_const_value_new();
		if (val == NULL)
			return arC_synax_tree_out_of_memory(state);
		val->closest_function_node = state->func_node;
		arC_syntax_tree_node_type* stack_type_node =
			(arC_syntax_tree_node_type*)arC_syntax_tree_find_incl_imports(state->parent_node, 
				GET_CONST_VM_STRING(arC_syntax_tree, float64), BIT(arC_SYNTAX_TREE_TYPE));
		asC_syntax_tree_stack_type(val) = stack_type_node->symbol;
		val->value.f64 = arC_token_f8(t);
		val->value.type = val->header.stack_type->data_type;
		arC_token_next(t);
		return asC_syntax_tree(val);
	}
	if (t->type == ARTOK_IDENTITY) {
		const arString identity = t->string;
		arC_symbol* symbol = arC_func_find_symbol(state->func_node->symbol, &identity);
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
			arC_func_add_local(state->func_node->symbol, local);

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
			arC_return* return_symbol = s->func_node->symbol->signature.returns;

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

				if (return_expr->stack_type != return_symbol->type) {
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
