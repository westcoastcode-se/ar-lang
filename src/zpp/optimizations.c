#include "optimizations.h"
#include "symbols.h"
#include "compiler.h"

// Merge two constants and put the result into the left-hand side memory location. Returns FALSE if the merge was not successfull - for example
// if the underlying memory size has to be uplifted (increased to fit the new constant value)
BOOL zpp_syntax_tree_merge_constants(zpp_token_type op, vmp_const* lhs, const vmp_const* rhs)
{
	if (lhs->type != rhs->type) {
		// TODO: Allow different types, for example 2 * 2.0 should result into a 4.0 decimal value
		return FALSE;
	}

	if (lhs->type == -1) {
		// Unknown type
		return FALSE;
	}

	const vm_int32 data_type = lhs->type;

	switch (op)
	{
	case ZPP_TOKEN_OP_PLUS:
		return vmp_const_add(lhs, rhs);
	case ZPP_TOKEN_OP_MINUS:
		switch (data_type)
		{
		case VMI_INSTR_PROP_INT8:
			lhs->i1 = lhs->i1 - rhs->i1;
			break;
		case VMI_INSTR_PROP_UINT8:
			lhs->ui1 = lhs->ui1 - rhs->ui1;
			break;
		case VMI_INSTR_PROP_INT16:
			lhs->i2 = lhs->i2 - rhs->i2;
			break;
		case VMI_INSTR_PROP_UINT16:
			lhs->ui2 = lhs->ui2 - rhs->ui2;
			break;
		case VMI_INSTR_PROP_INT32:
			lhs->i4 = lhs->i4 - rhs->i4;
			break;
		case VMI_INSTR_PROP_UINT32:
			lhs->ui4 = lhs->ui4 - rhs->ui4;
			break;
		case VMI_INSTR_PROP_INT64:
			lhs->i8 = lhs->i8 - rhs->i8;
			break;
		case VMI_INSTR_PROP_UINT64:
			lhs->ui8 = lhs->ui8 - rhs->ui8;
			break;
		case VMI_INSTR_PROP_FLOAT32:
			lhs->f4 = lhs->f4 - rhs->f4;
			break;
		case VMI_INSTR_PROP_FLOAT64:
			lhs->f8 = lhs->f8 - rhs->f8;
			break;
		default:
			return FALSE;
		}
		break;
	case ZPP_TOKEN_OP_MULT:
		switch (data_type)
		{
		case VMI_INSTR_PROP_INT8:
			lhs->i1 = lhs->i1 * rhs->i1;
			break;
		case VMI_INSTR_PROP_UINT8:
			lhs->ui1 = lhs->ui1 * rhs->ui1;
			break;
		case VMI_INSTR_PROP_INT16:
			lhs->i2 = lhs->i2 * rhs->i2;
			break;
		case VMI_INSTR_PROP_UINT16:
			lhs->ui2 = lhs->ui2 * rhs->ui2;
			break;
		case VMI_INSTR_PROP_INT32:
			lhs->i4 = lhs->i4 * rhs->i4;
			break;
		case VMI_INSTR_PROP_UINT32:
			lhs->ui4 = lhs->ui4 * rhs->ui4;
			break;
		case VMI_INSTR_PROP_INT64:
			lhs->i8 = lhs->i8 * rhs->i8;
			break;
		case VMI_INSTR_PROP_UINT64:
			lhs->ui8 = lhs->ui8 * rhs->ui8;
			break;
		case VMI_INSTR_PROP_FLOAT32:
			lhs->f4 = lhs->f4 * rhs->f4;
			break;
		case VMI_INSTR_PROP_FLOAT64:
			lhs->f8 = lhs->f8 * rhs->f8;
			break;
		default:
			return FALSE;
		}
		break;
	case ZPP_TOKEN_OP_DIV:
		switch (data_type)
		{
		case VMI_INSTR_PROP_INT8:
			lhs->i1 = lhs->i1 / rhs->i1;
			break;
		case VMI_INSTR_PROP_UINT8:
			lhs->ui1 = lhs->ui1 / rhs->ui1;
			break;
		case VMI_INSTR_PROP_INT16:
			lhs->i2 = lhs->i2 / rhs->i2;
			break;
		case VMI_INSTR_PROP_UINT16:
			lhs->ui2 = lhs->ui2 / rhs->ui2;
			break;
		case VMI_INSTR_PROP_INT32:
			lhs->i4 = lhs->i4 / rhs->i4;
			break;
		case VMI_INSTR_PROP_UINT32:
			lhs->ui4 = lhs->ui4 / rhs->ui4;
			break;
		case VMI_INSTR_PROP_INT64:
			lhs->i8 = lhs->i8 / rhs->i8;
			break;
		case VMI_INSTR_PROP_UINT64:
			lhs->ui8 = lhs->ui8 / rhs->ui8;
			break;
		case VMI_INSTR_PROP_FLOAT32:
			lhs->f4 = lhs->f4 / rhs->f4;
			break;
		case VMI_INSTR_PROP_FLOAT64:
			lhs->f8 = lhs->f8 / rhs->f8;
			break;
		default:
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
		if (!zpp_syntax_tree_merge_constants(binop->op, &left_value->value, &right_value->value)) {
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

void zpp_synax_tree_merge_binop2(const zpp_compiler_state* s, zpp_syntax_tree_binop* binop)
{
	zpp_syntax_tree* const left = ZPP_SYNTAX_TREE(binop)->node;
	zpp_syntax_tree* const right = left->tail;

	// Merge the two constant values into a single constant value node
	if (left->type == ZPP_SYNTAX_TREE_CONST_VALUE && right->type == ZPP_SYNTAX_TREE_CONST_VALUE) {
		zpp_syntax_tree_const_value* const left_value = (zpp_syntax_tree_const_value*)left;
		zpp_syntax_tree_const_value* const right_value = (zpp_syntax_tree_const_value*)right;

		// Try to merge the supplied values
		if (!zpp_syntax_tree_merge_constants(binop->op, &left_value->value, &right_value->value)) {
			// TODO: Add support for uplifting the constant into more bytes
			return;
		}

		// Detach child-nodes
		binop->header.node = binop->header.node_end = NULL;
		left_value->header.head = left_value->header.tail = NULL;
		right_value->header.head = right_value->header.tail = NULL;
		zpp_syntax_tree_remove_replace(binop->header.parent, ZPP_SYNTAX_TREE(binop), ZPP_SYNTAX_TREE(left_value));
		zpp_syntax_tree_destroy(ZPP_SYNTAX_TREE(binop));
		zpp_syntax_tree_destroy(ZPP_SYNTAX_TREE(right_value));
	}
	else {
		// TODO: Add support for this?
	}
}

void zpp_synax_tree_merge_unaryop2(const zpp_compiler_state* s, zpp_syntax_tree_unaryop* unaryop)
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
					return;
				}
			}
			else {
				// TODO: Add support for this?
				return;
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
		zpp_syntax_tree_remove_replace(unaryop->header.parent, ZPP_SYNTAX_TREE(unaryop), ZPP_SYNTAX_TREE(right_value));
		zpp_syntax_tree_destroy(ZPP_SYNTAX_TREE(unaryop));
	}
}

void zpp_synax_tree_merge_children2(const zpp_compiler_state* s, zpp_syntax_tree_node node)
{
	if (node->type == ZPP_SYNTAX_TREE_BINOP) {
		zpp_synax_tree_merge_binop2(s, (zpp_syntax_tree_binop*)node);
	}
	else if (node->type == ZPP_SYNTAX_TREE_UNARYOP) {
		zpp_synax_tree_merge_unaryop2(s, (zpp_syntax_tree_unaryop*)node);
	}
}

BOOL zpp_synax_tree_merge(const zpp_compiler_state* s, zpp_syntax_tree_node parent)
{
	const zpp_compiler_state inner_scope = {
		s->compiler, s->token, parent, s->package_node, s->func_node, NULL
	};

	// Merge children first and then move upwards
	zpp_syntax_tree_node node = parent->node;
	while (node) {
		zpp_syntax_tree_node tail = node->tail;
		if (!zpp_synax_tree_merge(&inner_scope, node))
			return FALSE;
		node = tail;
	}

	zpp_synax_tree_merge_children2(&inner_scope, parent);
	return TRUE;
}

BOOL zpp_synax_tree_merge_func(const struct zpp_compiler_state* s, zpp_syntax_tree_func* parent)
{
	const zpp_compiler_state inner_scope = {
		s->compiler, s->token, ZPP_SYNTAX_TREE(parent), s->package_node, parent, NULL
	};

	zpp_syntax_tree_node node = parent->header.node;
	while (node) {
		zpp_syntax_tree_node tail = node->tail;
		if (!zpp_synax_tree_merge(&inner_scope, node))
			return FALSE;
		node = tail;
	}
	return TRUE;
}

BOOL zpp_synax_tree_merge_package(const struct zpp_compiler_state* s, zpp_syntax_tree_package* parent)
{
	const zpp_compiler_state inner_scope = {
		s->compiler, s->token, ZPP_SYNTAX_TREE(parent), parent, NULL, NULL
	};

	zpp_syntax_tree_node node = parent->header.node;
	while (node) {
		switch (node->type) {
		case ZPP_SYNTAX_TREE_FUNC:
			if (!zpp_synax_tree_merge_func(&inner_scope, (zpp_syntax_tree_func*)node)) {
				return FALSE;
			}
		default:
			break;
		}
		node = node->tail;
	}
	return TRUE;
}

BOOL zpp_synax_tree_optimize(const struct zpp_compiler_state* s, zpp_syntax_tree_root* root_node)
{
	const zpp_compiler_state inner_scope = {
		s->compiler, s->token, ZPP_SYNTAX_TREE(root_node), NULL, NULL, NULL
	};

	zpp_syntax_tree_node node = root_node->header.node;
	while (node) {
		switch (node->type) {
		case ZPP_SYNTAX_TREE_PACKAGE:
			if (!zpp_synax_tree_merge_package(&inner_scope, (zpp_syntax_tree_package*)node)) {
				return FALSE;
			}
		default:
			break;
		}
		node = node->tail;
	}
	return TRUE;
}
