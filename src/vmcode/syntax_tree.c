#include "syntax_tree.h"
#include "vmcd_messages.h"
#include "vmcd_types.h"
#include "vmcode.h"

void vmcd_syntax_tree_init(vmcd_syntax_tree_node st, vmcd_syntax_tree_type type)
{
	st->type = type;
	st->head = st->tail = NULL;
}

vmcd_syntax_tree_node vmcd_syntax_tree_next_factor(vmcd_scope* s)
{
	// value
	vm_string type_name;
	vmcd_token* const t = s->token;
	vmcd_syntax_tree_const_value* result = (vmcd_syntax_tree_const_value*)vm_malloc(sizeof(vmcd_syntax_tree_const_value));
	if (result == NULL) {
		vmcd_message_out_of_memory(s);
		return NULL;
	}
	vmcd_syntax_tree_init(&result->header, VMCD_STT_CONST_VALUE);
	
	switch (t->type) {
	case VMCD_TOKEN_VALUE_INT:
		type_name.start = "int32";
		type_name.end = type_name.start + 5;
		result->type = VMCD_KEYWORD(vmcd_package_find_type_include_imports(s->package, &type_name));
		result->value.i4 = vmcd_token_i4(t);
		break;
	case VMCD_TOKEN_VALUE_DECIMAL:
		type_name.start = "float32";
		type_name.end = type_name.start + 7;
		result->type = VMCD_KEYWORD(vmcd_package_find_type_include_imports(s->package, &type_name));
		result->value.f4 = vmcd_token_f4(t);
		break;
	default:
		vmcd_message_not_implemented(s);
		return NULL;
	}
	vmcd_token_next(t);
	return VMCD_SYNTAX_TREE(result);
}

vmcd_syntax_tree_node vmcd_syntax_tree_next_binop(vmcd_scope* s)
{
	// term ((bin_op) term)*
	vmcd_token* const t = s->token;
	vmcd_syntax_tree_node left = vmcd_syntax_tree_next_factor(s);
	if (left == NULL)
		return NULL;

	while (TRUE) {
		switch (t->type) {
		case VMCD_TOKEN_OP_PLUS:
		{
			vmcd_token_next(t);
			vmcd_syntax_tree_node right = vmcd_syntax_tree_next_factor(s);
			if (right == NULL)
				return NULL;
			vmcd_syntax_tree_binop* const binop = (vmcd_syntax_tree_binop*)vm_malloc(sizeof(vmcd_syntax_tree_binop));
			if (binop == NULL) {
				vmcd_message_out_of_memory(s);
				return NULL;
			}
			vmcd_syntax_tree_init(&binop->header, VMCD_STT_BINOP);
			binop->left = left;
			binop->op = VMCD_TOKEN_OP_PLUS;
			binop->right = right;
			binop->op_type = ((vmcd_syntax_tree_const_value*)left)->type; // TODO: This should be verified
			left = VMCD_SYNTAX_TREE(binop);
		}
		default:
			goto endwhile;
		}
	}
endwhile:
	return left;
}

vmcd_syntax_tree_node vmcd_syntax_tree_next_term(vmcd_scope* s)
{
	return vmcd_syntax_tree_next_binop(s);
}

vmcd_syntax_tree_node vmcd_syntax_tree_next_expr(vmcd_scope* s)
{
	return vmcd_syntax_tree_next_binop(s);
	
}

vmcd_syntax_tree_node vmcd_syntax_tree_next(struct vmcd_scope* s)
{
	vmcd_syntax_tree* st = NULL;
	vmcd_token* const t = s->token;
	if (vmcd_token_is_keyword(t)) {
		switch (t->type)
		{
		case VMCD_TOKEN_KEYWORD_RETURN:
		{
			vmcd_syntax_tree_return* const ret = (vmcd_syntax_tree_return*)vm_malloc(sizeof(vmcd_syntax_tree_return));
			if (ret == NULL) {
				vmcd_message_out_of_memory(s);
				return NULL;
			}
			vmcd_syntax_tree_init(&ret->header, VMCD_STT_RETURN);
			vmcd_token_next(t);
			ret->expression = vmcd_syntax_tree_next_expr(s);
			return VMCD_SYNTAX_TREE(ret);
		}
		default:
			break;
		}
	}
	vmcd_message_not_implemented(s);
	return NULL;
}

void vmcd_syntax_tree_free(vmcd_syntax_tree* st)
{
	while (st != NULL) {
		vmcd_syntax_tree* tail = st->tail;
		switch (st->type) {
		case VMCD_STT_CONST_VALUE:
			break;
		case VMCD_STT_BINOP:
			vmcd_syntax_tree_free(((vmcd_syntax_tree_binop*)st)->left);
			vmcd_syntax_tree_free(((vmcd_syntax_tree_binop*)st)->right);
			break;
		case VMCD_STT_RETURN:
			vmcd_syntax_tree_free(((vmcd_syntax_tree_return*)st)->expression);
			break;
		default:
			assert(FALSE && "Bad SyntaxTree type");
			break;
		}
		vm_free(st);
		st = tail;
	}
}

BOOL vmcd_syntax_tree_expression_compile(vmcd_syntax_tree* st, vmcd_func* f)
{
	switch (st->type)
	{
	case VMCD_STT_BINOP:
	{
		// push left
		// push right
		// op
		vmcd_syntax_tree_binop* const binop = (vmcd_syntax_tree_binop*)st;
		if (!vmcd_syntax_tree_expression_compile(binop->left, f))
			return FALSE;
		if (!vmcd_syntax_tree_expression_compile(binop->right, f))
			return FALSE;
		assert(binop->op == VMCD_TOKEN_OP_PLUS);
		vmp_func_add_instr(f->func, vmp_instr_add(vmcd_keyword_resolve_type(binop->op_type)));
	}
	break;
	case VMCD_STT_CONST_VALUE:
	{
		vmcd_syntax_tree_const_value* cv = (vmcd_syntax_tree_const_value*)st;
		vmp_func_add_instr(f->func, vmp_instr_ldc(vmcd_keyword_resolve_type(cv->type), cv->value));
	}
	break;
	case VMCD_STT_RETURN:
	{
		vmcd_syntax_tree_return* ret = (vmcd_syntax_tree_return*)st;
		if (!vmcd_syntax_tree_expression_compile(ret->expression, f))
			return FALSE;
		vmp_func_add_instr(f->func, vmp_instr_ret());
	}
	break;
	}
	return TRUE;
}

BOOL vmcd_syntax_tree_compile(vmcd_syntax_tree* st, vmcd_func* f)
{
	vmp_func_begin_body(f->func);
	while (st != NULL) {
		if (!vmcd_syntax_tree_expression_compile(st, f))
			return FALSE;
		st = st->tail;
	}
	vmp_func_begin_end(f->func);
	return TRUE;
}
