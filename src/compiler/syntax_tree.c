#include "syntax_tree.h"
#include "search.h"
#include "messages.h"
#include "tokens.h"
#include "syntax_tree_optimize.h"
#include "../arCompiler.h"

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
		parent->child_tail->version++;
		parent->child_tail->tail = child;
		child->head = parent->child_tail;
		parent->child_tail = child;
	}
	child->parent = parent;
	parent->child_count++;

	// Increase the version so that we know that the nodes are updated
	parent->version++;
	child->version++;
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

arC_syntax_tree_ref* arC_syntax_tree_ref_new(const arC_state* s, arInt32 valid_types)
{
	arC_syntax_tree_ref* const p = (arC_syntax_tree_ref*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_ref), arC_SYNTAX_TREE_REF);
	if (p == NULL)
		return NULL;
	p->valid_types = valid_types;
	return p;
}

arC_syntax_tree_ref_block* arC_syntax_tree_ref_block_new(const arC_state* s)
{
	arC_syntax_tree_ref_block* const p = (arC_syntax_tree_ref_block*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_ref_block), arC_SYNTAX_TREE_REF_BLOCK);
	if (p == NULL)
		return NULL;
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

arC_syntax_tree_typeref* arC_syntax_tree_typeref_known(const arC_state* s, const arString* name, arInt32 valid_types)
{
	arC_syntax_tree_typeref* const typeref = arC_syntax_tree_typeref_new(s);
	if (typeref == NULL) return NULL;
	arC_syntax_tree_ref* const ref = arC_syntax_tree_ref_new(s, valid_types);
	if (ref == NULL) return NULL;
	arC_syntax_tree_ref_block* const block = arC_syntax_tree_ref_block_new(s);
	if (block == NULL) return NULL;
	block->query = *name;
	block->valid_types = valid_types;
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

arC_syntax_tree_funcdef_body_const_value* arC_syntax_tree_funcdef_body_const_value_new(const arC_state* s)
{
	arC_syntax_tree_funcdef_body_const_value* const p = (arC_syntax_tree_funcdef_body_const_value*)arC_syntax_tree_new(s,
		sizeof(arC_syntax_tree_funcdef_body_const_value), arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE);
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
			BIT(arC_SYNTAX_TREE_TYPEDEF));
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
			BIT(arC_SYNTAX_TREE_TYPEDEF));
		arC_syntax_tree_add_child(asC_syntax_tree(val), asC_syntax_tree(val->type));
		// TODO: We can resolve the types immediately because these are constants already defined by the root package
		arC_token_next(t);
		return asC_syntax_tree(val);
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
			arInt32 return_expressions_left = s->func_node->rets->count;

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
		printf("ref_block query=%.*s", arString_length(&ref->query), ref->query.start); 
		printf("[");
		for (int i = 0; i < 32; ++i) {
			if (BIT_ISSET(ref->valid_types, BIT(i))) {
				switch (i) {
				case arC_SYNTAX_TREE_PACKAGE:
					printf("package,");
					break;
				case arC_SYNTAX_TREE_TYPEDEF:
					printf("typedef,");
					break;
				default:
					printf("?%d,", i);
					break;
				}
			}
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
		break;
	}
	case arC_SYNTAX_TREE_FUNCDEF: {
		arC_syntax_tree_funcdef* def = (arC_syntax_tree_funcdef*)st;
		printf("funcdef name=%.*s", arString_length(&def->name), def->name.start);
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
