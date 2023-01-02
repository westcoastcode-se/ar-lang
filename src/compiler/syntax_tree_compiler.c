#include "syntax_tree_compiler.h"
#include "messages.h"
#include "../arCompiler.h"

BOOL arC_syntax_tree_compile_package(arCompiler* c, arC_syntax_tree_package* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;
	
	// Set compiled symbol's properties
	arB_package_set_signature(p->compiled.symbol, &p->signature.signature);
	arBuilder_add_package(c->pipeline, p->compiled.symbol);
	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);

	// Iterate over all the children
	arC_syntax_tree* child = asC_syntax_tree(p)->child_head;
	while (child != NULL) {
		if (!arC_syntax_tree_compile(c, child))
			return FALSE;
		child = child->tail;
	}
	return TRUE;
}

BOOL arC_syntax_tree_compile_typedef(arCompiler* c, arC_syntax_tree_typedef* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;

	// Make sure that all inherited type are resolved
	if (p->resolved.inherits_from != NULL) {
		if (!arC_syntax_tree_compile_typedef(c, p->resolved.inherits_from)) {
			return FALSE;
		}
	}

	// Make sure that all unrefed types are resolved
	if (p->resolved.of_type != NULL) {
		if (!arC_syntax_tree_compile_typedef(c, p->resolved.of_type)) {
			return FALSE;
		}
	}

	arB_type_set_signature(p->compiled.symbol, &p->resolved.signature.signature);
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

BOOL arC_syntax_tree_compile_funcdef_ret(arCompiler* c, arC_syntax_tree_funcdef_ret* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;
	arB_func_add_return(p->func->compiled.symbol, p->compiled.symbol);
	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef_rets(arCompiler* c, arC_syntax_tree_funcdef_rets* p)
{
	arC_syntax_tree* child = p->header.child_head;
	while (child) {
		if (child->type == arC_SYNTAX_TREE_FUNCDEF_RET) {
			if (!arC_syntax_tree_compile_funcdef_ret(c, (arC_syntax_tree_funcdef_ret*)child))
				return FALSE;
		}
		child = child->tail;
	}
}

BOOL arC_syntax_tree_compile_funcdef_arg(arCompiler* c, arC_syntax_tree_funcdef_arg* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE))
		return TRUE;
	p->compiled.symbol->type = p->resolved.def->compiled.symbol;
	arB_arg_set_name(p->compiled.symbol, &p->name);
	arB_func_add_arg(p->func->compiled.symbol, p->compiled.symbol);
	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);
	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef_args(arCompiler* c, arC_syntax_tree_funcdef_args* p)
{
	arC_syntax_tree* child = p->header.child_head;
	while (child) {
		if (child->type == arC_SYNTAX_TREE_FUNCDEF_ARG) {
			if (!arC_syntax_tree_compile_funcdef_arg(c, (arC_syntax_tree_funcdef_arg*)child))
				return FALSE;
		}
		child = child->tail;
	}
}

BOOL arC_syntax_tree_compile_funcdef(arCompiler* c, arC_syntax_tree_funcdef* p)
{
	if (asC_syntax_tree_phase_done(p, arC_SYNTAX_TREE_PHASE_COMPILE)) {
		return TRUE;
	}

	if (p->body == NULL) {
		// No body present?
		return FALSE;
	}

	arB_func_set_signature(p->compiled.symbol, asC_syntax_tree_signature(p));

	// Add arguments
	if (p->args != NULL) {
		if (!arC_syntax_tree_compile_funcdef_args(c, p->args)) {
			return FALSE;
		}
	}

	// Add returns
	if (p->rets != NULL) {
		if (!arC_syntax_tree_compile_funcdef_rets(c, p->rets)) {
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

	asC_syntax_tree_phase_set(p, arC_SYNTAX_TREE_PHASE_COMPILE);
	return FALSE;
}

BOOL arC_syntax_tree_compile(arCompiler* c, arC_syntax_tree* st)
{
	switch (st->type) {
	case arC_SYNTAX_TREE_PACKAGE:
		return arC_syntax_tree_compile_package(c, (arC_syntax_tree_package*)st);
	case arC_SYNTAX_TREE_TYPEDEF:
		return arC_syntax_tree_compile_typedef(c, (arC_syntax_tree_typedef*)st);
	case arC_SYNTAX_TREE_FUNCDEF:
		return arC_syntax_tree_compile_funcdef(c, (arC_syntax_tree_funcdef*)st);
	default:
	{
		// Iterate over all the children
		arC_syntax_tree* child = st->child_head;
		while (child != NULL) {
			if (!arC_syntax_tree_compile(c, child))
				return FALSE;
			child = child->tail;
		}
		break;
	}
	}

	return TRUE;
}

