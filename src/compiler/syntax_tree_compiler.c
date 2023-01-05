#include "syntax_tree_compiler.h"
#include "messages.h"
#include "tokens.h"
#include "../arCompiler.h"

BOOL arC_syntax_tree_compile_package(const arC_state* s, arC_syntax_tree_package* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;
	
	// Set compiled symbol's properties
	arB_package_set_signature(p->compiled.symbol, &p->resolve.signature);
	arBuilder_add_package(s->compiler->pipeline, p->compiled.symbol);
	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);

	// Iterate over all the children
	arC_syntax_tree* child = asC_syntax_tree(p)->child_head;
	while (child != NULL) {
		if (!arC_syntax_tree_compile(s, child))
			return FALSE;
		child = child->tail;
	}
	return TRUE;
}

BOOL arC_syntax_tree_compile_typedef(const arC_state* s, arC_syntax_tree_typedef* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;

	if (p->inherits_from != NULL) {
		arC_syntax_tree_typeref* const typeref = p->inherits_from;
		arC_syntax_tree_typedef* const type = typeref->resolved.def;
		if (!arC_syntax_tree_compile_typedef(s, type)) {
			return FALSE;
		}
	}

	if (p->of_type != NULL) {
		arC_syntax_tree_typeref* const typeref = p->of_type;
		arC_syntax_tree_typedef* const type = typeref->resolved.def;
		if (!arC_syntax_tree_compile_typedef(s, type)) {
			return FALSE;
		}
	}

	arB_type_set_signature(p->compiled.symbol, &p->resolved.signature);
	p->compiled.symbol->size = p->resolved.stack_size;
	p->compiled.symbol->data_type = p->resolved.data_type;
	p->compiled.symbol->flags = p->flags;

	// Add the type to the package
	int ret = arB_package_add_type(p->package->compiled.symbol, p->compiled.symbol);
	if (ret != VMP_LIST_ADDED) {
		return FALSE;
	}
	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);

	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef_ret(const arC_state* s, arC_syntax_tree_funcdef_ret* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;
	arC_syntax_tree_typedef* const type = p->type->resolved.def;
	p->compiled.symbol->type = type->compiled.symbol;
	arB_func_add_return(p->func->compiled.symbol, p->compiled.symbol);
	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef_rets(const arC_state* s, arC_syntax_tree_funcdef_rets* p)
{
	arC_syntax_tree* child = p->header.child_head;
	while (child) {
		if (child->type == arC_SYNTAX_TREE_FUNCDEF_RET) {
			if (!arC_syntax_tree_compile_funcdef_ret(s, (arC_syntax_tree_funcdef_ret*)child))
				return FALSE;
		}
		child = child->tail;
	}
	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef_arg(const arC_state* s, arC_syntax_tree_funcdef_arg* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;
	arC_syntax_tree_typedef* const type = p->type->resolved.def;
	p->compiled.symbol->type = type->compiled.symbol;
	arB_arg_set_name(p->compiled.symbol, &p->name);
	arB_func_add_arg(p->func->compiled.symbol, p->compiled.symbol);
	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef_args(const arC_state* s, arC_syntax_tree_funcdef_args* p)
{
	arC_syntax_tree* child = p->header.child_head;
	while (child) {
		if (child->type == arC_SYNTAX_TREE_FUNCDEF_ARG) {
			if (!arC_syntax_tree_compile_funcdef_arg(s, (arC_syntax_tree_funcdef_arg*)child))
				return FALSE;
		}
		child = child->tail;
	}
	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef(const arC_state* s, arC_syntax_tree_funcdef* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE)) {
		return TRUE;
	}

	if (p->body == NULL) {
		// No body present?
		return FALSE;
	}

	arB_func_set_signature(p->compiled.symbol, &p->resolve.signature);

	// Add arguments
	if (p->args != NULL) {
		if (!arC_syntax_tree_compile_funcdef_args(s, p->args)) {
			return FALSE;
		}
	}

	// Add returns
	if (p->rets != NULL) {
		if (!arC_syntax_tree_compile_funcdef_rets(s, p->rets)) {
			return FALSE;
		}
	}

	// add local values
	/*arC_local* local = f->locals;
	while (local != NULL) {
		if (!arC_type_resolve(local->type, s))
			return FALSE;
		local->local = arB_func_new_local(f->func, local->type->type);
		arB_local_set_name(local->local, asC_symbol_name(local));
		local = local->tail;
	}*/

	// Iterate over the body nodes
	arB_func_body_begin(p->compiled.symbol);
	arC_syntax_tree* child = p->body->header.child_head;
	while (child != NULL) {
		if (!arC_syntax_tree_compile(s, child))
			return FALSE;
		child = child->tail;
	}
	arB_func_body_end(p->compiled.symbol);


	// TODO: Add support for functions in functions
	if (p->header.parent->type != arC_SYNTAX_TREE_PACKAGE) {
		return arC_message_not_implemented(s, "#7 add support for functions inside functions");
	}
	arC_syntax_tree_package* package = (arC_syntax_tree_package*)p->header.parent;
	arB_package_add_func(package->compiled.symbol, p->compiled.symbol);

	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_return(const arC_state* s, arC_syntax_tree_funcdef_body_return* ret)
{
	if (asC_syntax_tree_phase_done(ret, arC_SYNTAX_TREE_PHASE_COMPILE)) {
		return TRUE;
	}

	arC_syntax_tree* node = asC_syntax_tree(ret)->child_head;
	while (node != NULL) {
		if (!arC_syntax_tree_compile(s, node))
			return FALSE;
		node = node->tail;
	}
	arB_func_add_instr(ret->closest_function_node->compiled.symbol, arB_instr_ret());
	asC_syntax_tree_phase_set(ret, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_binop(const arC_state* s, arC_syntax_tree_funcdef_body_binop* binop)
{
	ASSERT_NOT_NULL(binop);
	assert(binop->header.child_count == 2 && "A binop node is only allowed to have two children");

	if (asC_syntax_tree_phase_done(binop, arC_SYNTAX_TREE_PHASE_COMPILE)) {
		return TRUE;
	}

	// push left
	// push right
	// op

	arC_syntax_tree* const left = asC_syntax_tree(binop)->child_head;
	if (!arC_syntax_tree_compile(s, left))
		return FALSE;
	arC_syntax_tree* const right = left->tail;
	if (!arC_syntax_tree_compile(s, right))
		return FALSE;

	arB_func* const func = binop->closest_function_node->compiled.symbol;
	arB_type* const type = arC_syntax_tree_get_stack_type(right)->compiled.symbol;
	if (func == NULL || type == NULL) {
		return FALSE;
	}

	switch (binop->op)
	{
	case ARTOK_OP_MINUS:
		arB_func_add_instr(func, arB_instr_sub(type));
		break;
	case ARTOK_OP_PLUS:
		arB_func_add_instr(func, arB_instr_add(type));
		break;
	case ARTOK_OP_MULT:
		arB_func_add_instr(func, arB_instr_mul(type));
		break;
	case ARTOK_OP_DIV:
		arB_func_add_instr(func, arB_instr_div(type));
		break;
	case ARTOK_TEST_LT:
		arB_func_add_instr(func, arB_instr_clt(type));
		break;
	case ARTOK_TEST_GT:
		arB_func_add_instr(func, arB_instr_cgt(type));
		break;
	default:
		return arC_message_feature_missing(s, "binop operator not implemented");
	}

	asC_syntax_tree_phase_set(binop, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_unaryop(const arC_state* s, arC_syntax_tree_funcdef_body_unaryop* unaryop)
{
	ASSERT_NOT_NULL(unaryop);
	assert(unaryop->header.child_count == 1 && "A unaryop node is only allowed to have one child");

	if (asC_syntax_tree_phase_done(unaryop, arC_SYNTAX_TREE_PHASE_COMPILE)) {
		return TRUE;
	}

	// push expression
	// op

	arC_syntax_tree* const node = asC_syntax_tree_first_child(unaryop);
	if (!arC_syntax_tree_compile(s, node))
		return FALSE;

	arB_func* const func = unaryop->closest_function_node->compiled.symbol;
	arB_type* const type = arC_syntax_tree_get_stack_type(node)->compiled.symbol;
	if (func == NULL || type == NULL) {
		return FALSE;
	}

	switch (unaryop->op)
	{
	case ARTOK_OP_MINUS:
		arB_func_add_instr(func, arB_instr_neg(type));
		break;
	case ARTOK_BIT_NOT:
		arB_func_add_instr(func, arB_instr_bit_not(type));
		break;
	default:
		return arC_message_feature_missing(s, "unaryop operator not implemented");
	}

	asC_syntax_tree_phase_set(unaryop, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_const_value(const arC_state* s, arC_syntax_tree_funcdef_body_const_value* val)
{
	arB_func* const func = val->closest_function_node->compiled.symbol;
	arB_type* const type = val->type->resolved.def->compiled.symbol;
	arB_func_add_instr(func, arB_instr_ldc(type, val->value));
	return TRUE;
}

BOOL arC_syntax_tree_compile(const arC_state* s, arC_syntax_tree* st)
{
	switch (st->type) {
	case arC_SYNTAX_TREE_PACKAGE:
		return arC_syntax_tree_compile_package(s, (arC_syntax_tree_package*)st);
	case arC_SYNTAX_TREE_TYPEDEF:
		return arC_syntax_tree_compile_typedef(s, (arC_syntax_tree_typedef*)st);
	case arC_SYNTAX_TREE_FUNCDEF:
		return arC_syntax_tree_compile_funcdef(s, (arC_syntax_tree_funcdef*)st);
	case arC_SYNTAX_TREE_FUNCDEF_BODY_RETURN:
		return arC_syntax_tree_compile_return(s, (arC_syntax_tree_funcdef_body_return*)st);
	case arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP:
		return arC_syntax_tree_compile_binop(s, (arC_syntax_tree_funcdef_body_binop*)st);
	case arC_SYNTAX_TREE_FUNCDEF_BODY_UNARYOP:
		return arC_syntax_tree_compile_unaryop(s, (arC_syntax_tree_funcdef_body_unaryop*)st);
	case arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE:
		return arC_syntax_tree_compile_const_value(s, (arC_syntax_tree_funcdef_body_const_value*)st);
	default:
	{
		// Iterate over all the children
		arC_syntax_tree* child = st->child_head;
		while (child != NULL) {
			if (!arC_syntax_tree_compile(s, child))
				return FALSE;
			child = child->tail;
		}
		break;
	}
	}

	return TRUE;
}

