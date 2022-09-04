#include "optimizations.h"
#include "symbol.h"
#include "tokens.h"
#include "../arCompiler.h"

// Merge two constants and put the result into the left-hand side memory location. Returns FALSE if the merge was not successfull - for example
// if the underlying memory size has to be uplifted (increased to fit the new constant value)
BOOL arC_syntax_tree_merge_constants(arC_token_type op, arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	if (lhs->type != rhs->type) {
		// TODO: Allow different types, for example 2 * 2.0 should result into a 4.0 decimal value
		return FALSE;
	}

	if (lhs->type == -1) {
		// Unknown type
		return FALSE;
	}

	const arInt32 data_type = lhs->type;

	switch (op)
	{
	case ARTOK_OP_PLUS:
		return arPrimitiveValue_add(lhs, rhs);
	case ARTOK_OP_MINUS:
		return arPrimitiveValue_sub(lhs, rhs);
	case ARTOK_OP_MULT:
		return arPrimitiveValue_mult(lhs, rhs);
	case ARTOK_OP_DIV:
		return arPrimitiveValue_div(lhs, rhs);
	default:
		return FALSE;
	}
	return TRUE;
}

arC_syntax_tree_node arC_synax_tree_merge_binop(const arC_state* s, arC_syntax_tree_node_binop* binop)
{
	arC_syntax_tree* const left = asC_syntax_tree(binop)->node;
	arC_syntax_tree* const right = left->tail;

	// Merge the two constant values into a single constant value node
	if (left->type == AR_SYNTAX_TREE_CONST_VALUE && right->type == AR_SYNTAX_TREE_CONST_VALUE) {
		arC_syntax_tree_node_const_value* const left_value = (arC_syntax_tree_node_const_value*)left;
		arC_syntax_tree_node_const_value* const right_value = (arC_syntax_tree_node_const_value*)right;

		// Try to merge the supplied values
		if (!arC_syntax_tree_merge_constants(binop->op, &left_value->value, &right_value->value)) {
			// TODO: Add support for uplifting the constant into more bytes
			return asC_syntax_tree(binop);
		}

		// Detach value nodes
		binop->header.node = binop->header.node_end = NULL;
		left_value->header.head = left_value->header.tail = NULL;
		right_value->header.head = right_value->header.tail = NULL;
		arC_syntax_tree_node_type_destroy(asC_syntax_tree(binop));
		arC_syntax_tree_node_type_destroy(asC_syntax_tree(right_value));

		// Return the node which is considered the "merged" node
		return asC_syntax_tree(left_value);
	}
	else {
		// TODO: Add support for this?
	}

	return asC_syntax_tree(binop);
}

arC_syntax_tree_node arC_synax_tree_merge_unaryop(const arC_state* s, arC_syntax_tree_node_unaryop* unaryop)
{
	arC_syntax_tree* const right = asC_syntax_tree(unaryop)->node;
	if (right->type == AR_SYNTAX_TREE_CONST_VALUE) {
		arC_syntax_tree_node_const_value* const right_value = (arC_syntax_tree_node_const_value*)right;

		// Compile time resolve the unaryop
		switch (unaryop->op) {
		case ARTOK_OP_MINUS:
		{
			arC_symbol* symbol = right_value->header.stack_type;
			if (symbol->type == arC_SYMBOL_TYPE) {
				arC_type* type = (arC_type*)symbol;
				// Integer type, float types and doubles
				if (type->data_type >= ARLANG_PRIMITIVE_I8 && type->data_type <= ARLANG_PRIMITIVE_UI64) {
					right_value->value.i64 = -right_value->value.i64;
				}
				else if (type->data_type >= ARLANG_PRIMITIVE_F32) {
					right_value->value.f32 = -right_value->value.f32;
				}
				else if (type->data_type >= ARLANG_PRIMITIVE_F64) {
					right_value->value.f64 = -right_value->value.f64;
				}
				else {
					// TODO: Add support for this?
					return asC_syntax_tree(unaryop);
				}
			}
			else {
				// TODO: Add support for this?
				return asC_syntax_tree(unaryop);
			}
		}
		break;
		case ARTOK_BIT_NOT:
		{
			arC_symbol* symbol = right_value->header.stack_type;
			if (symbol->type == arC_SYMBOL_TYPE) {
				arC_type* type = (arC_type*)symbol;
				// Integer type, float types and doubles
				if (type->data_type >= ARLANG_PRIMITIVE_I8 && type->data_type <= ARLANG_PRIMITIVE_UI64) {
					right_value->value.i64 = ~right_value->value.i64;
				}
				else {
					// TODO: Add support for this?
					return asC_syntax_tree(unaryop);
				}
			}
			else {
				// TODO: Add support for this?
				return asC_syntax_tree(unaryop);
			}
		}
		break;
		default:
			return asC_syntax_tree(unaryop);
		}

		// Detach value node and remove the unaryop
		unaryop->header.node = unaryop->header.node_end = NULL;
		right_value->header.head = right_value->header.tail = NULL;
		arC_syntax_tree_node_type_destroy(asC_syntax_tree(unaryop));

		// Return the node which is considered the "merged" node
		return asC_syntax_tree(right_value);
	}
	return asC_syntax_tree(unaryop);
}

arC_syntax_tree_node arC_synax_tree_merge_children(const arC_state* s, arC_syntax_tree_node node)
{
	if (node->type == AR_SYNTAX_TREE_BINOP) {
		return arC_synax_tree_merge_binop(s, (arC_syntax_tree_node_binop*)node);
	}
	else if (node->type == AR_SYNTAX_TREE_UNARYOP) {
		return arC_synax_tree_merge_unaryop(s, (arC_syntax_tree_node_unaryop*)node);
	}
	return node;
}

void arC_synax_tree_merge_binop2(const arC_state* s, arC_syntax_tree_node_binop* binop)
{
	arC_syntax_tree* const left = asC_syntax_tree(binop)->node;
	arC_syntax_tree* const right = left->tail;

	// Merge the two constant values into a single constant value node
	if (left->type == AR_SYNTAX_TREE_CONST_VALUE && right->type == AR_SYNTAX_TREE_CONST_VALUE) {
		arC_syntax_tree_node_const_value* const left_value = (arC_syntax_tree_node_const_value*)left;
		arC_syntax_tree_node_const_value* const right_value = (arC_syntax_tree_node_const_value*)right;

		// Try to merge the supplied values
		if (!arC_syntax_tree_merge_constants(binop->op, &left_value->value, &right_value->value)) {
			// TODO: Add support for uplifting the constant into more bytes
			return;
		}

		// Detach child-nodes
		binop->header.node = binop->header.node_end = NULL;
		left_value->header.head = left_value->header.tail = NULL;
		right_value->header.head = right_value->header.tail = NULL;
		arC_syntax_tree_remove_replace(binop->header.parent, asC_syntax_tree(binop), asC_syntax_tree(left_value));
		arC_syntax_tree_node_type_destroy(asC_syntax_tree(binop));
		arC_syntax_tree_node_type_destroy(asC_syntax_tree(right_value));
	}
	else {
		// TODO: Add support for this?
	}
}

void arC_synax_tree_merge_unaryop2(const arC_state* s, arC_syntax_tree_node_unaryop* unaryop)
{
	arC_syntax_tree* const right = asC_syntax_tree(unaryop)->node;
	if (right->type == AR_SYNTAX_TREE_CONST_VALUE) {
		arC_syntax_tree_node_const_value* const right_value = (arC_syntax_tree_node_const_value*)right;

		// Compile time resolve the unaryop
		switch (unaryop->op) {
		case ARTOK_OP_MINUS:
		{
			arC_symbol* symbol = right_value->header.stack_type;
			if (symbol->type == arC_SYMBOL_TYPE) {
				arC_type* type = (arC_type*)symbol;
				// Integer type, float types and doubles
				if (type->data_type >= ARLANG_PRIMITIVE_I8 && type->data_type <= ARLANG_PRIMITIVE_UI64) {
					right_value->value.i64 = -right_value->value.i64;
				}
				else if (type->data_type >= ARLANG_PRIMITIVE_F32) {
					right_value->value.f32 = -right_value->value.f32;
				}
				else if (type->data_type >= ARLANG_PRIMITIVE_F64) {
					right_value->value.f64 = -right_value->value.f64;
				}
				else {
					// TODO: Add support for this?
					return;
				}
			}
			else {
				// TODO: Add support for this?
				return;
			}
		}
		break;
		case ARTOK_BIT_NOT:
		{
			arC_symbol* symbol = right_value->header.stack_type;
			if (symbol->type == arC_SYMBOL_TYPE) {
				arC_type* type = (arC_type*)symbol;
				// Integer type, float types and doubles
				if (type->data_type >= ARLANG_PRIMITIVE_I8 && type->data_type <= ARLANG_PRIMITIVE_UI64) {
					right_value->value.i64 = ~right_value->value.i64;
				}
				else {
					// TODO: Add support for this?
					return;
				}
			}
			else {
				// TODO: Add support for this?
				return;
			}
		}
		break;
		default:
			return;
		}

		// Detach value node and remove the unaryop
		unaryop->header.node = unaryop->header.node_end = NULL;
		right_value->header.head = right_value->header.tail = NULL;
		arC_syntax_tree_remove_replace(unaryop->header.parent, asC_syntax_tree(unaryop), asC_syntax_tree(right_value));
		arC_syntax_tree_node_type_destroy(asC_syntax_tree(unaryop));
	}
}

void arC_synax_tree_merge_children2(const arC_state* s, arC_syntax_tree_node node)
{
	if (node->type == AR_SYNTAX_TREE_BINOP) {
		arC_synax_tree_merge_binop2(s, (arC_syntax_tree_node_binop*)node);
	}
	else if (node->type == AR_SYNTAX_TREE_UNARYOP) {
		arC_synax_tree_merge_unaryop2(s, (arC_syntax_tree_node_unaryop*)node);
	}
}

BOOL arC_synax_tree_merge(const arC_state* s, arC_syntax_tree_node parent)
{
	const arC_state inner_scope = {
		s->compiler, s->token, parent, s->package_node, s->func_node, NULL
	};

	// Merge children first and then move upwards
	arC_syntax_tree_node node = parent->node;
	while (node) {
		arC_syntax_tree_node tail = node->tail;
		if (!arC_synax_tree_merge(&inner_scope, node))
			return FALSE;
		node = tail;
	}

	arC_synax_tree_merge_children2(&inner_scope, parent);
	return TRUE;
}

BOOL arC_synax_tree_merge_func(const struct arC_state* s, arC_syntax_tree_node_func* parent)
{
	const arC_state inner_scope = {
		s->compiler, s->token, asC_syntax_tree(parent), s->package_node, parent, NULL
	};

	arC_syntax_tree_node node = parent->header.node;
	while (node) {
		arC_syntax_tree_node tail = node->tail;
		if (!arC_synax_tree_merge(&inner_scope, node))
			return FALSE;
		node = tail;
	}
	return TRUE;
}

BOOL arC_synax_tree_merge_package(const struct arC_state* s, arC_syntax_tree_node_package* parent)
{
	const arC_state inner_scope = {
		s->compiler, s->token, asC_syntax_tree(parent), parent, NULL, NULL
	};

	arC_syntax_tree_node node = parent->header.node;
	while (node) {
		switch (node->type) {
		case AR_SYNTAX_TREE_FUNC:
			if (!arC_synax_tree_merge_func(&inner_scope, (arC_syntax_tree_node_func*)node)) {
				return FALSE;
			}
		default:
			break;
		}
		node = node->tail;
	}
	return TRUE;
}

BOOL arC_synax_tree_optimize(const struct arC_state* s, arC_syntax_tree_root* root_node)
{
	const arC_state inner_scope = {
		s->compiler, s->token, asC_syntax_tree(root_node), NULL, NULL, NULL
	};

	arC_syntax_tree_node node = root_node->header.node;
	while (node) {
		switch (node->type) {
		case AR_SYNTAX_TREE_PACKAGE:
			if (!arC_synax_tree_merge_package(&inner_scope, (arC_syntax_tree_node_package*)node)) {
				return FALSE;
			}
		default:
			break;
		}
		node = node->tail;
	}
	return TRUE;
}
