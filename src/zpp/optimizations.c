#include "optimizations.h"
#include "symbols.h"
#include "compiler.h"

// Merge two constants and put the result into the supplied memory. Returns FALSE if the merge was not successfull - for example
// if the underlying memory size has to be uplifted (increased to fit the new constant value)
BOOL zpp_syntax_tree_merge_constants(zpp_token_type op, const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	if (lhs->type != rhs->type) {
		// TODO: Allow different types
		return FALSE;
	}

	const vm_int32 data_type = lhs->type;

	switch (op)
	{
	case ZPP_TOKEN_OP_PLUS:
		if (data_type >= VMI_INSTR_PROP_INT8 && data_type <= VMI_INSTR_PROP_UINT64) {
			result->i8 = lhs->i8 + rhs->i8;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT32) {
			result->f4 = lhs->f4 + rhs->f4;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT64) {
			result->f8 = lhs->f8 + rhs->f8;
		}
		else {
			// TODO: Allow using + and - on pointers and other such types
			return FALSE;
		}
		break;
	case ZPP_TOKEN_OP_MINUS:
		if (data_type >= VMI_INSTR_PROP_INT8 && data_type <= VMI_INSTR_PROP_UINT64) {
			result->i8 = lhs->i8 - rhs->i8;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT32) {
			result->f4 = lhs->f4 - rhs->f4;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT64) {
			result->f8 = lhs->f8 - rhs->f8;
		}
		else {
			// TODO: Allow using + and - on pointers and other such types
			return FALSE;
		}
		break;
	case ZPP_TOKEN_OP_MULT:
		if (data_type >= VMI_INSTR_PROP_INT8 && data_type <= VMI_INSTR_PROP_UINT64) {
			result->i8 = lhs->i8 * rhs->i8;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT32) {
			result->f4 = lhs->f4 * rhs->f4;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT64) {
			result->f8 = lhs->f8 * rhs->f8;
		}
		else {
			return FALSE;
		}
		break;
	case ZPP_TOKEN_OP_DIV:
		if (data_type >= VMI_INSTR_PROP_INT8 && data_type <= VMI_INSTR_PROP_UINT64) {
			result->i8 = lhs->i8 / rhs->i8;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT32) {
			result->f4 = lhs->f4 / rhs->f4;
		}
		else if (data_type >= VMI_INSTR_PROP_FLOAT64) {
			result->f8 = lhs->f8 / rhs->f8;
		}
		else {
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

zpp_syntax_tree_node zpp_synax_tree_merge_binop(const zpp_compiler_state* s, zpp_syntax_tree_binop* binop)
{
	zpp_syntax_tree* const left = ZPP_SYNTAX_TREE(binop)->node;
	zpp_syntax_tree* const right = left->tail;

	// Merge the two constant values into a single constant value node
	if (left->type == ZPP_SYNTAX_TREE_CONST_VALUE && right->type == ZPP_SYNTAX_TREE_CONST_VALUE) {
		zpp_syntax_tree_const_value* const left_value = (zpp_syntax_tree_const_value*)left;
		zpp_syntax_tree_const_value* const right_value = (zpp_syntax_tree_const_value*)right;

		// Try to merge the supplied values
		if (!zpp_syntax_tree_merge_constants(binop->op, &left_value->value, &right_value->value, &left_value->value)) {
			// TODO: Add support for uplifting the constant into more bytes
			return ZPP_SYNTAX_TREE(binop);
		}

		// Detach value nodes
		binop->header.node = binop->header.node_end = NULL;
		left_value->header.head = left_value->header.tail = NULL;
		right_value->header.head = right_value->header.tail = NULL;
		zpp_syntax_tree_destroy(ZPP_SYNTAX_TREE(binop));
		zpp_syntax_tree_destroy(ZPP_SYNTAX_TREE(right_value));

		// Return the node which is considered the "merged" node
		return ZPP_SYNTAX_TREE(left_value);
	}
	else {
		// TODO: Add support for this?
	}

	return ZPP_SYNTAX_TREE(binop);
}

zpp_syntax_tree_node zpp_synax_tree_merge_unaryop(const zpp_compiler_state* s, zpp_syntax_tree_unaryop* unaryop)
{
	zpp_syntax_tree* const right = ZPP_SYNTAX_TREE(unaryop)->node;
	if (right->type == ZPP_SYNTAX_TREE_CONST_VALUE) {
		zpp_syntax_tree_const_value* const right_value = (zpp_syntax_tree_const_value*)right;

		// Compile time resolve the unaryop
		switch (unaryop->op) {
		case ZPP_TOKEN_OP_MINUS:
		{
			zpp_symbol* symbol = right_value->header.stack_type;
			if (symbol->type == ZPP_SYMBOL_TYPE) {
				zpp_type* type = (zpp_type*)symbol;
				// Integer type, float types and doubles
				if (type->data_type >= VMI_INSTR_PROP_INT8 && type->data_type <= VMI_INSTR_PROP_UINT64) {
					right_value->value.i8 = -right_value->value.i8;
				}
				else if (type->data_type >= VMI_INSTR_PROP_FLOAT32) {
					right_value->value.f4 = -right_value->value.f4;
				}
				else if (type->data_type >= VMI_INSTR_PROP_FLOAT64) {
					right_value->value.f8 = -right_value->value.f8;
				}
				else {
					// TODO: Add support for this?
					return ZPP_SYNTAX_TREE(unaryop);
				}
			}
			else {
				// TODO: Add support for this?
				return ZPP_SYNTAX_TREE(unaryop);
			}
		}
		break;
		case ZPP_TOKEN_BIT_NOT:
		{
			zpp_symbol* symbol = right_value->header.stack_type;
			if (symbol->type == ZPP_SYMBOL_TYPE) {
				zpp_type* type = (zpp_type*)symbol;
				// Integer type, float types and doubles
				if (type->data_type >= VMI_INSTR_PROP_INT8 && type->data_type <= VMI_INSTR_PROP_UINT64) {
					right_value->value.i8 = ~right_value->value.i8;
				}
				else {
					// TODO: Add support for this?
					return ZPP_SYNTAX_TREE(unaryop);
				}
			}
			else {
				// TODO: Add support for this?
				return ZPP_SYNTAX_TREE(unaryop);
			}
		}
		break;
		default:
			return ZPP_SYNTAX_TREE(unaryop);
		}

		// Detach value node and remove the unaryop
		unaryop->header.node = unaryop->header.node_end = NULL;
		right_value->header.head = right_value->header.tail = NULL;
		zpp_syntax_tree_destroy(ZPP_SYNTAX_TREE(unaryop));

		// Return the node which is considered the "merged" node
		return ZPP_SYNTAX_TREE(right_value);
	}
	return ZPP_SYNTAX_TREE(unaryop);
}

zpp_syntax_tree_node zpp_synax_tree_merge_children(const zpp_compiler_state* s, zpp_syntax_tree_node node)
{
	if (node->type == ZPP_SYNTAX_TREE_BINOP) {
		return zpp_synax_tree_merge_binop(s, (zpp_syntax_tree_binop*)node);
	}
	else if (node->type == ZPP_SYNTAX_TREE_UNARYOP) {
		return zpp_synax_tree_merge_unaryop(s, (zpp_syntax_tree_unaryop*)node);
	}
	return node;
}
