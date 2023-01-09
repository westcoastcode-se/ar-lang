#include "syntax_tree.h"
#include "search.h"
#include "messages.h"
#include "tokens.h"
#include "syntax_tree_optimize.h"
#include "../arCompiler.h"
#include <inttypes.h>

arC_syntax_tree_node arC_syntax_tree_new(const arC_state* s, arInt32 size, arC_syntax_tree_type type)
{
	assert(size >= sizeof(arC_syntax_tree));
	arC_syntax_tree_node p = (arC_syntax_tree_node)arMemoryPool_get(s->memory_pool, size);
	if (p == NULL) {
		arC_message_out_of_memory(s);
		return NULL;
	}
	p->type = type;
	return p;
}

arStringPool* arC_state_get_string_pool(const arC_state* s)
{
	ASSERT_NOT_NULL(s->compiler);
	return &s->compiler->pipeline->string_pool;
}

arC_syntax_tree_node arC_syntax_tree_error()
{
	static arC_syntax_tree instance = { arC_SYNTAX_TREE_ERROR, NULL, NULL, NULL, 0, NULL, NULL, 0 };
	return &instance;
}

arC_syntax_tree_node arC_syntax_tree_out_of_memory(const arC_state* s)
{
	arC_message_out_of_memory(s);
	return arC_syntax_tree_error();
}

arC_syntax_tree_node arC_syntax_tree_not_implemented(const arC_state* s)
{
	arC_message_not_implemented(s, "N/A");
	return arC_syntax_tree_error();
}

arC_syntax_tree_node arC_syntax_tree_syntax_error(const arC_state* s, const char* msg)
{
	arC_message_syntax_error(s, msg);
	return arC_syntax_tree_error();
}

BOOL arC_syntax_tree_is_error(arC_syntax_tree_node node)
{
	return node == NULL || node == arC_syntax_tree_error();
}

BOOL arC_syntax_tree_has_parent(arC_syntax_tree_node st)
{
	return st->parent != NULL;
}

BOOL arC_syntax_tree_has_children(arC_syntax_tree_node st)
{
	return st->child_head != NULL;
}

void arC_syntax_tree_add_child(arC_syntax_tree* parent, arC_syntax_tree_node child)
{
	ASSERT_NOT_NULL(parent);
	ASSERT_NOT_NULL(child);
	assert(child->parent == NULL && "You are trying to add a node as a child while it already has a parent");

	// Update parent-child relationship
	if (parent->child_head == NULL)
		parent->child_head = parent->child_tail = child;
	else {
		ASSERT_NULL(parent->child_tail->tail);
		parent->child_tail->tail = child;
		child->head = parent->child_tail;
		parent->child_tail = child;
	}
	child->parent = parent;
	parent->child_count++;
}

arC_syntax_tree_node arC_syntax_tree_first_parent_of_type(const arC_syntax_tree_node node, arC_syntax_tree_type type)
{
	arC_syntax_tree_node parent = node->parent;
	while (parent) {
		if (parent->type == type) {
			return parent;
		}
		parent = parent->parent;
	}
	return NULL;
}

void arC_syntax_tree_detach(arC_syntax_tree_node node)
{
	assert(node->parent != NULL && "You are trying to detach a node that's not attached to any parent");

	// The node is at the end of the list
	arC_syntax_tree* const parent = node->parent;

	// If the node is the last child
	if (parent->child_tail == node) {
		parent->child_tail->head = node->head;
		parent->child_tail = node->head;
		node->head = NULL;
	}

	// If the node is the first child
	if (parent->child_head == node) {
		parent->child_head->tail = node->tail;
		parent->child_head = node->tail;
		node->tail = NULL;
	}

	if (node->head) {
		node->head->tail = node->tail;
	}
	if (node->tail) {
		node->tail->head = node->head;
	}

	node->head = node->tail = node->parent = NULL;
	parent->child_count--;
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
		st->child_head = new_node;
	else
		head->tail = new_node;

	if (tail == NULL)
		st->child_tail = new_node;
	else
		tail->head = new_node;
}

arC_syntax_tree_ref* arC_syntax_tree_ref_new(const arC_state* s)
{
	arC_syntax_tree_ref* const p = (arC_syntax_tree_ref*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_ref), arC_SYNTAX_TREE_REF);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_node arC_syntax_tree_ref_find_first(const arC_syntax_tree_ref* ref, arC_syntax_tree_type type)
{
	ASSERT_NOT_NULL(ref);
	for (int i = 0; i < ref->resolved.nodes_count; ++i) {
		arC_syntax_tree_node node = ref->resolved.nodes[i];
		if (node->type == type)
			return node;
	}
	return NULL;
}

arC_syntax_tree_ref_block* arC_syntax_tree_ref_block_new(const arC_state* s, arC_syntax_tree_search_type_bits types)
{
	arC_syntax_tree_ref_block* const p = (arC_syntax_tree_ref_block*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_ref_block), arC_SYNTAX_TREE_REF_BLOCK);
	if (p == NULL)
		return NULL;
	p->types = types;
	p->resolved.closest_distance = INT32_MAX;
	return p;
}

arC_syntax_tree_package* arC_syntax_tree_package_new(const arC_state* s)
{
	arC_syntax_tree_package* const p = (arC_syntax_tree_package*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_package), arC_SYNTAX_TREE_PACKAGE);
	if (p == NULL)
		return NULL;
	return p;
}

BOOL arC_syntax_tree_package_is_root(const arC_syntax_tree_package* p)
{
	return p->header.parent == NULL;
}

void arC_syntax_tree_package_add_import(arC_syntax_tree_package* package, arC_syntax_tree_import* import)
{
	arC_syntax_tree_add_child(asC_syntax_tree(package), asC_syntax_tree(import));
}

void arC_syntax_tree_package_add_typedef(arC_syntax_tree_package* package, arC_syntax_tree_typedef* type)
{
	arC_syntax_tree_add_child(asC_syntax_tree(package), asC_syntax_tree(type));
}

arC_syntax_tree_import* arC_syntax_tree_import_new(const arC_state* s)
{
	arC_syntax_tree_import* const p = (arC_syntax_tree_import*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_import), arC_SYNTAX_TREE_IMPORT);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_typedef* arC_syntax_tree_typedef_new(const arC_state* s)
{
	arC_syntax_tree_typedef* const p = (arC_syntax_tree_typedef*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_typedef), arC_SYNTAX_TREE_TYPEDEF);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_typeref* arC_syntax_tree_typeref_new(const arC_state* s)
{
	arC_syntax_tree_typeref* const p = (arC_syntax_tree_typeref*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_typeref), arC_SYNTAX_TREE_TYPEREF);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_typeref_implicit* arC_syntax_tree_typeref_implicit_new(const arC_state* s)
{
	arC_syntax_tree_typeref_implicit* const p = (arC_syntax_tree_typeref_implicit*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_typeref_implicit), arC_SYNTAX_TREE_TYPEREF_IMPLICIT);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_typeref* arC_syntax_tree_typeref_known(const arC_state* s, const arString* name, arC_syntax_tree_search_type_bits valid_types)
{
	arC_syntax_tree_typeref* const typeref = arC_syntax_tree_typeref_new(s);
	if (typeref == NULL) return NULL;
	arC_syntax_tree_ref* const ref = arC_syntax_tree_ref_new(s);
	if (ref == NULL) return NULL;
	arC_syntax_tree_ref_block* const block = arC_syntax_tree_ref_block_new(s, valid_types);
	if (block == NULL) return NULL;
	block->query = *name;
	arC_syntax_tree_add_child(asC_syntax_tree(ref), asC_syntax_tree(block));
	arC_syntax_tree_add_child(asC_syntax_tree(typeref), asC_syntax_tree(ref));
	return typeref;
}

arC_syntax_tree_funcdef* arC_syntax_tree_funcdef_new(const arC_state* s)
{
	arC_syntax_tree_funcdef* const p = (arC_syntax_tree_funcdef*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef), arC_SYNTAX_TREE_FUNCDEF);
	if (p == NULL)
		return NULL;
	return p;
}

const arString* arC_syntax_tree_funcdef_get_name(arC_syntax_tree_funcdef* funcdef)
{
	return &funcdef->head->name;
}

arC_syntax_tree_funcdef_head* arC_syntax_tree_funcdef_head_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_head* const p = (arC_syntax_tree_funcdef_head*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_head), arC_SYNTAX_TREE_FUNCDEF_HEAD);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_args* arC_syntax_tree_funcdef_args_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_args* const p = (arC_syntax_tree_funcdef_args*)arC_syntax_tree_new(s, 
		sizeof(arC_syntax_tree_funcdef_args), arC_SYNTAX_TREE_FUNCDEF_ARGS);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_arg* arC_syntax_tree_funcdef_arg_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_arg* const p = (arC_syntax_tree_funcdef_arg*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_arg), arC_SYNTAX_TREE_FUNCDEF_ARG);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_rets* arC_syntax_tree_funcdef_rets_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_rets* const p = (arC_syntax_tree_funcdef_rets*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_rets), arC_SYNTAX_TREE_FUNCDEF_RETS);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_ret* arC_syntax_tree_funcdef_ret_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_ret* const p = (arC_syntax_tree_funcdef_ret*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_ret), arC_SYNTAX_TREE_FUNCDEF_RET);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_locals* arC_syntax_tree_funcdef_locals_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_locals* const p = (arC_syntax_tree_funcdef_locals*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_locals), arC_SYNTAX_TREE_FUNCDEF_LOCALS);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_local* arC_syntax_tree_funcdef_local_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_local* const p = (arC_syntax_tree_funcdef_local*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_local), arC_SYNTAX_TREE_FUNCDEF_LOCAL);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcref* arC_syntax_tree_funcref_new(const arC_state* s)
{
	arC_syntax_tree_funcref* const p = (arC_syntax_tree_funcref*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcref), arC_SYNTAX_TREE_FUNCREF);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcref_args* arC_syntax_tree_funcref_args_new(const arC_state* s)
{
	arC_syntax_tree_funcref_args* const p = (arC_syntax_tree_funcref_args*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcref_args), arC_SYNTAX_TREE_FUNCREF_ARGS);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcref_arg* arC_syntax_tree_funcref_arg_new(const arC_state* s)
{
	arC_syntax_tree_funcref_arg* const p = (arC_syntax_tree_funcref_arg*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcref_arg), arC_SYNTAX_TREE_FUNCREF_ARG);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_body* arC_syntax_tree_funcdef_body_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_body* const p = (arC_syntax_tree_funcdef_body*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body), arC_SYNTAX_TREE_FUNCDEF_BODY);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_typedef* arC_syntax_tree_get_stack_type(arC_syntax_tree_node st)
{
	switch (st->type) {
	case arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE:
		return ((arC_syntax_tree_funcdef_body_const_value*)st)->type->resolved.def;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_RETURN:
		return ((arC_syntax_tree_funcdef_body_return*)st)->type->resolved.def;
	case arC_SYNTAX_TREE_FUNCDEF_ARG:
		return ((arC_syntax_tree_funcdef_arg*)st)->type->resolved.def;
	case arC_SYNTAX_TREE_FUNCDEF_LOCAL:
		return ((arC_syntax_tree_funcdef_local*)st)->type->resolved.def;
	case arC_SYNTAX_TREE_FUNCDEF_RET:
		return ((arC_syntax_tree_funcdef_ret*)st)->type->resolved.def;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP:
		return ((arC_syntax_tree_funcdef_body_binop*)st)->resolved.type;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_UNARYOP:
		return ((arC_syntax_tree_funcdef_body_unaryop*)st)->resolved.type;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_VARREF: {
		return arC_syntax_tree_get_stack_type(((arC_syntax_tree_funcdef_body_varref*)st)->resolved.node);
	}
	default:
		return NULL;
	}
}

arC_syntax_tree_funcdef_body_return* arC_syntax_tree_funcdef_body_return_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_body_return* const p = (arC_syntax_tree_funcdef_body_return*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_return), arC_SYNTAX_TREE_FUNCDEF_BODY_RETURN);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_body_assign* arC_syntax_tree_funcdef_body_assign_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_body_assign* const p = (arC_syntax_tree_funcdef_body_assign*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_assign), arC_SYNTAX_TREE_FUNCDEF_BODY_ASSIGN);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_body_const_value* arC_syntax_tree_funcdef_body_const_value_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_body_const_value* const p = (arC_syntax_tree_funcdef_body_const_value*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_const_value), arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_body_varref* arC_syntax_tree_funcdef_body_varref_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_body_varref* const p = (arC_syntax_tree_funcdef_body_varref*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_varref), arC_SYNTAX_TREE_FUNCDEF_BODY_VARREF);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_body_callfunc* arC_syntax_tree_funcdef_body_callfunc_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_body_callfunc* const p = (arC_syntax_tree_funcdef_body_callfunc*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_callfunc), arC_SYNTAX_TREE_FUNCDEF_BODY_CALLFUNC);
	if (p == NULL)
		return NULL;
	return p;
}

arC_syntax_tree_funcdef_body_unaryop* arC_syntax_tree_funcdef_body_unaryop_new(const arC_state* s, arC_syntax_tree_node right, arC_tokens op)
{
	arC_syntax_tree_funcdef_body_unaryop* const p = (arC_syntax_tree_funcdef_body_unaryop*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_unaryop), arC_SYNTAX_TREE_FUNCDEF_BODY_UNARYOP);
	if (p == NULL)
		return NULL;
	p->op = op;
	arC_syntax_tree_add_child(asC_syntax_tree(p), right);
	return p;
}

arC_syntax_tree_funcdef_body_binop* arC_syntax_tree_funcdef_body_binop_new(const arC_state* s, arC_syntax_tree_node left, arC_syntax_tree_node right, arC_tokens op)
{
	arC_syntax_tree_funcdef_body_binop* const p = (arC_syntax_tree_funcdef_body_binop*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_binop), arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP);
	if (p == NULL)
		return NULL;
	p->op = op;
	arC_syntax_tree_add_child(asC_syntax_tree(p), left);
	arC_syntax_tree_add_child(asC_syntax_tree(p), right);
	return p;
}

// Parse function
typedef arC_syntax_tree_node(*arC_syntax_tree_parse_fn)(arC_token*, const struct arC_state*);

arC_syntax_tree_node arC_syntax_tree_parse_comp_expr(arC_token* t, const arC_state* s);
arC_syntax_tree_node arC_syntax_tree_parse_term(arC_token* t, const arC_state* state);
arC_syntax_tree_node arC_syntax_tree_parse_factor(arC_token* t, const arC_state* state);
arC_syntax_tree_node arC_syntax_tree_parse_atom(arC_token* t, const arC_state* state);
arC_syntax_tree_node arC_syntax_tree_parse_expression(arC_token* t, const arC_state* s);
arC_syntax_tree_node arC_syntax_tree_parse_comp_expr(arC_token* t, const arC_state* s);
arC_syntax_tree_node arC_syntax_tree_parse_term(arC_token* t, const arC_state* s);

// A generic unary operator parser
arC_syntax_tree_node arC_syntax_tree_parse_unaryop(arC_token* const t, const arC_state* const s,
	arC_tokens token_type, arC_syntax_tree_parse_fn right_fn) {
	arC_token_next(t);
	arC_syntax_tree_node right = right_fn(t, s);
	if (arC_syntax_tree_is_error(right))
		return right;

	// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
	// node into a new constant node instead
	right = arC_syntax_tree_merge_children(s, right);

	arC_syntax_tree_funcdef_body_unaryop* const unaryop = arC_syntax_tree_funcdef_body_unaryop_new(s, right, token_type);
	if (unaryop == NULL)
		return NULL;
	unaryop->closest_function_node = s->func_node;

	// Merge the unaryop
	return arC_syntax_tree_merge_children(s, asC_syntax_tree(unaryop));
}

// A generic binary operator parser
arC_syntax_tree_node arC_syntax_tree_parse_binop(arC_token* const t, const arC_state* const s,
	const arC_tokens* const types, int types_count, arC_syntax_tree_parse_fn left_fn, arC_syntax_tree_parse_fn right_fn) {
	if (right_fn == NULL)
		right_fn = left_fn;
	arC_syntax_tree_node left = left_fn(t, s);
	if (arC_syntax_tree_is_error(left))
		return left;

	// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
	// node into a new constant node instead
	left = arC_syntax_tree_merge_children(s, left);

	while (1) {
		arC_tokens const token_type = t->type;
		int i;
		for (i = 0; i < types_count; ++i) {
			if (token_type == types[i]) {
				arC_token_next(t);
				arC_syntax_tree_node right = right_fn(t, s);
				if (arC_syntax_tree_is_error(right))
					return right;

				// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
				// node into a new constant node instead
				right = arC_syntax_tree_merge_children(s, right);

				arC_syntax_tree_funcdef_body_binop* const binop = arC_syntax_tree_funcdef_body_binop_new(s, left, right, token_type);
				if (binop == NULL)
					return arC_syntax_tree_error();
				binop->closest_function_node = s->func_node;

				// Merge the actual binop
				left = arC_syntax_tree_merge_children(s, asC_syntax_tree(binop));
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
arC_syntax_tree_node arC_syntax_tree_parse_binop_bitwise(arC_token* t, const arC_state* s, arC_syntax_tree_parse_fn left_fn,
	arC_syntax_tree_parse_fn right_fn)
{
	arC_syntax_tree_node left = left_fn(t, s);
	if (arC_syntax_tree_is_error(left))
		return left;

	// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
	// node into a new constant node instead
	left = arC_syntax_tree_merge_children(s, left);

	while (1) {
		switch (t->type) {
		case ARTOK_BIT_AND:
		{
			const arC_tokens token_type = t->type;
			arC_token_next(t);
			arC_syntax_tree_node right = right_fn(t, s);
			if (arC_syntax_tree_is_error(right))
				return right;

			// Try to merge all children. For example, if the node is another binop node with two constants, then replace the right
			// node into a new constant node instead
			right = arC_syntax_tree_merge_children(s, right);

			arC_syntax_tree_funcdef_body_binop* binop = arC_syntax_tree_funcdef_body_binop_new(s, left, right, token_type);
			if (binop == NULL)
				return arC_syntax_tree_error();
			binop->closest_function_node = s->func_node;

			// Merge the actual binop
			left = arC_syntax_tree_merge_children(s, asC_syntax_tree(binop));
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

arC_syntax_tree_node arC_syntax_tree_parse_atom(arC_token* t, const arC_state* s)
{
	if (t->type == ARTOK_VALUE_INT) {
		arC_syntax_tree_funcdef_body_const_value* const val = arC_syntax_tree_funcdef_body_const_value_new(s);
		if (val == NULL)
			return arC_syntax_tree_error();
		val->closest_function_node = s->func_node;
		val->value.i64 = arC_token_i8(t);
		val->value.type = ARLANG_PRIMITIVE_I32;
		val->type = arC_syntax_tree_typeref_known(s, GET_CONST_VM_STRING(arC_syntax_tree, int32),
			arC_SYNTAX_TREE_SEARCH_TYPE_TYPEDEF);
		arC_syntax_tree_add_child(asC_syntax_tree(val), asC_syntax_tree(val->type));
		// TODO: We can resolve the types immediately because these are constants already defined by the root package
		arC_token_next(t);
		return asC_syntax_tree(val);
	}
	else if (t->type == ARTOK_VALUE_DECIMAL) {
		arC_syntax_tree_funcdef_body_const_value* const val = arC_syntax_tree_funcdef_body_const_value_new(s);
		if (val == NULL)
			return arC_syntax_tree_error();
		val->closest_function_node = s->func_node;
		val->value.f64 = arC_token_f8(t);
		val->value.type = ARLANG_PRIMITIVE_F64;
		val->type = arC_syntax_tree_typeref_known(s, GET_CONST_VM_STRING(arC_syntax_tree, float64),
			arC_SYNTAX_TREE_SEARCH_TYPE_TYPEDEF);
		arC_syntax_tree_add_child(asC_syntax_tree(val), asC_syntax_tree(val->type));
		// TODO: We can resolve the types immediately because these are constants already defined by the root package
		arC_token_next(t);
		return asC_syntax_tree(val);
	}
	if (t->type == ARTOK_IDENTITY) {
		// If the supplied node is an identity then it might be a number of things. It might be
		// 1. A variable reference
		// 2. A variable declaration
		// 3. A reference to an argument
		// 4. A reference to a function
		// 5. ...

		const arString identity = t->string;
		arC_token_next(t);

		// A reference to the identity. We still don't know what type of node we are looking for,
		// so the first block should include all possibilities
		arC_syntax_tree_ref* ref = arC_syntax_tree_ref_new(s);
		if (ref == NULL) return arC_syntax_tree_error();
		arC_syntax_tree_ref_block* block = arC_syntax_tree_ref_block_new(s,
			arC_SYNTAX_TREE_SEARCH_TYPE_ARG | arC_SYNTAX_TREE_SEARCH_TYPE_LOCAL | arC_SYNTAX_TREE_SEARCH_TYPE_FUNCDEF |
			arC_SYNTAX_TREE_SEARCH_TYPE_PACKAGE);
		if (block == NULL)
			return arC_syntax_tree_error();
		block->query = identity;
		arC_syntax_tree_add_child(asC_syntax_tree(ref), asC_syntax_tree(block));

		arC_syntax_tree_node node = asC_syntax_tree(ref);
		while (1) {
			if (t->type == ARTOK_COMMA) {
				arC_message_not_implemented(s, "comma (multiple)");
				return arC_syntax_tree_error();
			}
			else if (t->type == ARTOK_DOT) {
				// dot is considered an identity delimiter
				arC_token_next(t);
			} else if (t->type == ARTOK_DECL_ASSIGN) {
				// <ref> :=
				arC_token_next(t);

				arC_syntax_tree_funcdef_local* const local = arC_syntax_tree_funcdef_local_new(s);
				if (local == NULL) return arC_syntax_tree_error();
				local->func = s->func_node;
				local->name = identity;
				local->type = arC_syntax_tree_typeref_new(s);
				if (local->type == NULL) return arC_syntax_tree_error();
				arC_syntax_tree_add_child(asC_syntax_tree(local), asC_syntax_tree(local->type));

				// Add the local definition to the container locals object
				arC_syntax_tree_add_child(asC_syntax_tree(s->func_node->body->locals), asC_syntax_tree(local));
				s->func_node->body->locals->count++;

				// Expression to be set in the local variable
				// TODO: take ,(comma) into consideration
				arC_syntax_tree_node decl_expression = arC_syntax_tree_parse_comp_expr(t, s);
				if (arC_syntax_tree_is_error(decl_expression))
					return decl_expression;

				// Add an implicit typeref
				arC_syntax_tree_typeref_implicit* const implicit = arC_syntax_tree_typeref_implicit_new(s);
				if (implicit == NULL) return arC_syntax_tree_error();
				implicit->implicit_from = decl_expression;
				arC_syntax_tree_add_child(asC_syntax_tree(local->type), asC_syntax_tree(implicit));

				// Create an assignment statement that combines an expression with var reference to a variable
				arC_syntax_tree_funcdef_body_assign* const assign = arC_syntax_tree_funcdef_body_assign_new(s);
				if (assign == NULL) return arC_syntax_tree_error();
				assign->closest_function_node = s->func_node;
				arC_syntax_tree_add_child(asC_syntax_tree(assign), decl_expression);

				// Create a reference to a variable. We already know the node so we can set the resolved
				// type immediately
				arC_syntax_tree_funcdef_body_varref* varref = arC_syntax_tree_funcdef_body_varref_new(s);
				if (varref == NULL) return arC_syntax_tree_error();
				arC_syntax_tree_add_child(asC_syntax_tree(assign), asC_syntax_tree(varref));
				varref->resolved.node = asC_syntax_tree(local);
				block->types = arC_SYNTAX_TREE_SEARCH_TYPE_LOCAL;
				node = asC_syntax_tree(assign);

			} else if (t->type == ARTOK_ASSIGN) {
				// <left> = <right>
				// <ref>.<block>... = <right>
				arC_token_next(t);

				// Expression to be set in the variable
				arC_syntax_tree_node decl_expression = arC_syntax_tree_parse_comp_expr(t, s);
				if (arC_syntax_tree_is_error(decl_expression))
					return decl_expression;

				// Create an assignment statement that combines an expression with var reference to a variable
				arC_syntax_tree_funcdef_body_assign* const assign = arC_syntax_tree_funcdef_body_assign_new(s);
				if (assign == NULL) return arC_syntax_tree_error();
				assign->closest_function_node = s->func_node;
				arC_syntax_tree_add_child(asC_syntax_tree(assign), decl_expression);

				// Create a reference to a variable. The variable might not be known yet, so lets
				// refer to it using a reference instead of directly resolve it
				arC_syntax_tree_funcdef_body_varref* varref = arC_syntax_tree_funcdef_body_varref_new(s);
				if (varref == NULL) return arC_syntax_tree_error();
				arC_syntax_tree_add_child(asC_syntax_tree(varref), asC_syntax_tree(ref));
				arC_syntax_tree_add_child(asC_syntax_tree(assign), asC_syntax_tree(varref));
				block->types = arC_SYNTAX_TREE_SEARCH_TYPE_ARG | arC_SYNTAX_TREE_SEARCH_TYPE_LOCAL;
				node = asC_syntax_tree(assign);
			}
			else if (t->type == ARTOK_PARAN_L) {
				arC_token_next(t);

				arC_syntax_tree_funcdef_body_callfunc* const callfunc = arC_syntax_tree_funcdef_body_callfunc_new(s);
				if (callfunc == NULL) return arC_syntax_tree_error();
				callfunc->closest_function_node = s->func_node;

				arC_syntax_tree_funcref* const funcref = arC_syntax_tree_funcref_new(s);
				if (funcref == NULL) return arC_syntax_tree_error();
				funcref->name = identity;

				arC_syntax_tree_funcref_args* const args = arC_syntax_tree_funcref_args_new(s);
				if (args == NULL) return arC_syntax_tree_error();
				funcref->args = args;
				arC_syntax_tree_add_child(asC_syntax_tree(funcref), asC_syntax_tree(args));

				// Parse each argument until we reach a )
				int call_index = 0;
				while (t->type != ARTOK_PARAN_R) {
					// Expressions to be sent as arguments
					arC_syntax_tree_node decl_expression = arC_syntax_tree_parse_comp_expr(t, s);
					if (arC_syntax_tree_is_error(decl_expression))
						return decl_expression;

					arC_syntax_tree_funcref_arg* const arg = arC_syntax_tree_funcref_arg_new(s);
					if (arg == NULL) return arC_syntax_tree_error();
					arC_syntax_tree_add_child(asC_syntax_tree(args), asC_syntax_tree(arg));
					arg->func = funcref;

					arC_syntax_tree_typeref* const arg_typeref = arC_syntax_tree_typeref_new(s);
					if (arg_typeref == NULL) return arC_syntax_tree_error();
					arg->type = arg_typeref;
					arC_syntax_tree_add_child(asC_syntax_tree(arg), asC_syntax_tree(arg_typeref));

					arC_syntax_tree_typeref_implicit* const implicit = arC_syntax_tree_typeref_implicit_new(s);
					if (implicit == NULL) return arC_syntax_tree_error();
					arC_syntax_tree_add_child(asC_syntax_tree(arg_typeref), asC_syntax_tree(implicit));
					implicit->implicit_from = decl_expression;

					arC_syntax_tree_add_child(asC_syntax_tree(callfunc), decl_expression);
				}
				// Ignore the )
				arC_token_next(t);

				arC_syntax_tree_add_child(asC_syntax_tree(callfunc), asC_syntax_tree(funcref));
				block->types = arC_SYNTAX_TREE_SEARCH_TYPE_FUNCDEF;
				callfunc->funcref = ref;
				node = asC_syntax_tree(callfunc);
			}
			else if (t->type == ARTOK_IDENTITY) {
				// <ref>.<block>.<child_block>
				arC_syntax_tree_ref_block* const child_block = arC_syntax_tree_ref_block_new(s,
					arC_SYNTAX_TREE_SEARCH_TYPE_ARG | arC_SYNTAX_TREE_SEARCH_TYPE_LOCAL | arC_SYNTAX_TREE_SEARCH_TYPE_FUNCDEF |
					arC_SYNTAX_TREE_SEARCH_TYPE_PACKAGE);
				if (child_block == NULL) return arC_syntax_tree_error();
				block->query = identity;
				arC_syntax_tree_add_child(asC_syntax_tree(block), asC_syntax_tree(child_block));
				block = child_block;

				arC_token_next(t);
			}
			else {
				break;
			}
		}
		return node;
	}
	else if (t->type == ARTOK_PARAN_L) {
		arC_token_next(t);
		arC_syntax_tree_node node = arC_syntax_tree_funcdef_body_parse(s);
		if (arC_syntax_tree_is_error(node))
			return node;
		// Expected right paranthesis
		if (t->type != ARTOK_PARAN_R) {
			arC_message_syntax_error(s, "expected )");
			return arC_syntax_tree_error();
		}
		arC_token_next(t);
		return node;
	}

	arC_message_syntax_error(s, "expected number, decimal, +, -, (");
	return arC_syntax_tree_error();
}

arC_syntax_tree_node arC_syntax_tree_parse_factor(arC_token* t, const arC_state* s)
{
	switch (t->type)
	{
	case ARTOK_OP_MINUS:
	case ARTOK_OP_PLUS:
	case ARTOK_BIT_NOT:
		return arC_syntax_tree_parse_unaryop(t, s, t->type, arC_syntax_tree_parse_factor);
	}

	return arC_syntax_tree_parse_atom(t, s);
}

arC_syntax_tree_node arC_syntax_tree_parse_term(arC_token* t, const arC_state* s)
{
	return arC_syntax_tree_parse_binop(t, s, ops_mult_div, 2,
		arC_syntax_tree_parse_factor, arC_syntax_tree_parse_factor);
}

arC_syntax_tree_node arC_syntax_tree_parse_expression(arC_token* t, const arC_state* s)
{
	return arC_syntax_tree_parse_binop(t, s, ops_plus_minus, 2,
		arC_syntax_tree_parse_term, arC_syntax_tree_parse_term);
}

arC_syntax_tree_node arC_syntax_tree_parse_comp_expr(arC_token* t, const arC_state* s)
{
	if (t->type == ARTOK_NOT) {
		return arC_syntax_tree_parse_unaryop(t, s, t->type, arC_syntax_tree_parse_comp_expr);
	}

	return arC_syntax_tree_parse_binop(t, s, ops_compares, 6,
		arC_syntax_tree_parse_expression, arC_syntax_tree_parse_expression);
}

arC_syntax_tree_node arC_syntax_tree_funcdef_body_parse(const arC_state* s)
{
	arC_token* const t = s->token;
	if (arC_token_is_keyword(t)) {
		switch (t->type) {
		case ARTOK_KEYWORD_RETURN:
		{
			arC_token_next(t);
			arC_syntax_tree_funcdef_body_return* const ret = arC_syntax_tree_funcdef_body_return_new(s);
			if (ret == NULL)
				return NULL;
			ret->closest_function_node = s->func_node;

			// The number of expressions that are left
			arInt32 return_expressions_left = s->func_node->head->rets->count;

			// Fetch all return statements 
			// TODO: Allow for skipping return values and let the compiler return the default value for you. For example, "error" should be allowed to return nil by default
			while (return_expressions_left > 0) {
				arC_syntax_tree_node return_expr = arC_syntax_tree_parse_comp_expr(t, s);
				if (arC_syntax_tree_is_error(return_expr))
					return return_expr;
				// Spearate each statement with a comma
				if (return_expressions_left > 1) {
					if (t->type != ARTOK_COMMA)
						return arC_syntax_tree_syntax_error(s, "expected ,");
					arC_token_next(t);
				}
				arC_syntax_tree_add_child(asC_syntax_tree(ret), return_expr);
				--return_expressions_left;
			}
			return asC_syntax_tree(ret);
		}
		break;
		case ARTOK_KEYWORD_VAR:
			return arC_syntax_tree_not_implemented(s);
		default:
			return arC_syntax_tree_not_implemented(s);
		}
	}

	return arC_syntax_tree_parse_comp_expr(t, s);
}

void _indent_console(int indent)
{
	for (int i = 0; i < indent; ++i)
		printf(" ");
}

void arC_syntax_tree_stdout0(const arC_syntax_tree* st, arInt32 indent, int child_idx)
{
	printf("[%p]", st);
	printf("[");
	if (BIT_ISSET(st->phase, arC_SYNTAX_TREE_PHASE_RESOLVE))
		printf("Y");
	else
		printf("N");
	if (BIT_ISSET(st->phase, arC_SYNTAX_TREE_PHASE_COMPILE))
		printf("Y");
	else
		printf("N");
	printf("] ");

	_indent_console(indent);
	if (child_idx == 0)
		printf("| - ");
	else
		printf("  - ");

	switch (st->type) {
	case arC_SYNTAX_TREE_REF: {
		arC_syntax_tree_ref* ref = (arC_syntax_tree_ref*)st;
		printf("ref");
		break;
	}
	case arC_SYNTAX_TREE_REF_BLOCK: {
		arC_syntax_tree_ref_block* ref = (arC_syntax_tree_ref_block*)st;
		printf("ref_block query=%.*s types=%d", arString_length(&ref->query), ref->query.start, ref->types);
		printf(" resolved=[");
		for (int i = 0; i < ref->resolved.nodes_count; ++i) {
			if (i != 0)
				printf(",");
			printf("%p", ref->resolved.nodes[i]);				
		}
		printf("]");
		break;
	}
	case arC_SYNTAX_TREE_PACKAGE: {
		arC_syntax_tree_package* package = (arC_syntax_tree_package*)st;
		printf("package name=%.*s", arString_length(&package->name), package->name.start);
		break;
	}
	case arC_SYNTAX_TREE_IMPORT: {
		arC_syntax_tree_import* ptr = (arC_syntax_tree_import*)st;
		printf("import");
		break;
	}
	case arC_SYNTAX_TREE_TYPEDEF: {
		arC_syntax_tree_typedef* type = (arC_syntax_tree_typedef*)st;
		printf("typedef name=%.*s", arString_length(&type->name), type->name.start);
		break;
	}
	case arC_SYNTAX_TREE_TYPEREF: {
		arC_syntax_tree_typeref* type = (arC_syntax_tree_typeref*)st;
		printf("typeref");
		if (type->resolved.def != NULL) {
			printf(" resolved=[%p]", type->resolved.def);
		}
		break;
	}
	case arC_SYNTAX_TREE_TYPEREF_IMPLICIT: {
		arC_syntax_tree_typeref_implicit* t = (arC_syntax_tree_typeref_implicit*)st;
		printf("typeref_implicit from=%p", t->implicit_from);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF: {
		arC_syntax_tree_funcdef* def = (arC_syntax_tree_funcdef*)st;
		printf("funcdef");
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_HEAD: {
		arC_syntax_tree_funcdef_head* def = (arC_syntax_tree_funcdef_head*)st;
		printf("funcdef_head name=%.*s", arString_length(&def->name), def->name.start);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_ARGS: {
		arC_syntax_tree_funcdef_args* def = (arC_syntax_tree_funcdef_args*)st;
		printf("args count=%d", def->count);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_ARG: {
		arC_syntax_tree_funcdef_arg* def = (arC_syntax_tree_funcdef_arg*)st;
		printf("arg name=%.*s", arString_length(&def->name), def->name.start);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_RETS: {
		arC_syntax_tree_funcdef_rets* def = (arC_syntax_tree_funcdef_rets*)st;
		printf("rets count=%d", def->count);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_RET: {
		arC_syntax_tree_funcdef_ret* def = (arC_syntax_tree_funcdef_ret*)st;
		printf("ret");
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_LOCALS: {
		arC_syntax_tree_funcdef_locals* def = (arC_syntax_tree_funcdef_locals*)st;
		printf("locals count=%d", def->count);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_LOCAL: {
		arC_syntax_tree_funcdef_local* def = (arC_syntax_tree_funcdef_local*)st;
		printf("local name=%.*s", arString_length(&def->name), def->name.start);
		break;
	}
	case arC_SYNTAX_TREE_FUNCREF: {
		arC_syntax_tree_funcref* def = (arC_syntax_tree_funcref*)st;
		printf("funcref name=%.*s", arString_length(&def->name), def->name.start);
		break;
	}
	case arC_SYNTAX_TREE_FUNCREF_ARGS: {
		arC_syntax_tree_funcref_args* def = (arC_syntax_tree_funcref_args*)st;
		printf("funcref_args");
		break;
	}
	case arC_SYNTAX_TREE_FUNCREF_ARG: {
		arC_syntax_tree_funcref_arg* def = (arC_syntax_tree_funcref_arg*)st;
		printf("funcref_arg");
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY: {
		arC_syntax_tree_funcdef_body* def = (arC_syntax_tree_funcdef_body*)st;
		printf("body");
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY_RETURN: {
		arC_syntax_tree_funcdef_body_return* def = (arC_syntax_tree_funcdef_body_return*)st;
		printf("return");
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY_ASSIGN: {
		arC_syntax_tree_funcdef_body_assign* def = (arC_syntax_tree_funcdef_body_assign*)st;
		printf("assign");
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE: {
		arC_syntax_tree_funcdef_body_const_value* def = (arC_syntax_tree_funcdef_body_const_value*)st;
		printf("const value=");
		switch (def->value.type)
		{
		case ARLANG_PRIMITIVE_I32:
			printf("%d", def->value.i32);
			break;
		case ARLANG_PRIMITIVE_I64:
			printf("%lld", def->value.i64);
			break;
		case ARLANG_PRIMITIVE_F32:
			printf("%f", def->value.f32);
			break;
		case ARLANG_PRIMITIVE_F64:
			printf("%f", def->value.f64);
			break;
		default:
			printf("?");
		}
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP: {
		arC_syntax_tree_funcdef_body_binop* def = (arC_syntax_tree_funcdef_body_binop*)st;
		printf("binop op=%d", def->op);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY_UNARYOP: {
		arC_syntax_tree_funcdef_body_unaryop* def = (arC_syntax_tree_funcdef_body_unaryop*)st;
		printf("unaryop op=%d", def->op);
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY_VARREF: {
		arC_syntax_tree_funcdef_body_varref* type = (arC_syntax_tree_funcdef_body_varref*)st;
		printf("varref");
		if (type->resolved.node != NULL) {
			printf(" resolved=[%p]", type->resolved.node);
		}
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF_BODY_CALLFUNC: {
		arC_syntax_tree_funcdef_body_callfunc* def = (arC_syntax_tree_funcdef_body_callfunc*)st;
		printf("callfunc");
		break;
	}
	default:
		break;
	}
	printf("\n");

	int index = 0;
	const arC_syntax_tree* child = st->child_head;
	while (child) {
		arC_syntax_tree_stdout0(child, indent + 2, index++);
		child = child->tail;
	}

}
void arC_syntax_tree_stdout(const arC_syntax_tree* st)
{
	printf("\n");
	arC_syntax_tree_stdout0(st, 0, 0);
}
