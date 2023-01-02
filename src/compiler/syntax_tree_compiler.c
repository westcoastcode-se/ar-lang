#include "syntax_tree_compiler.h"
#include "messages.h"
#include "../arCompiler.h"

BOOL arC_syntax_tree_compile_package(arCompiler* c, arC_syntax_tree_package* p)
{
	// Already compiled
	if (p->compiled.symbol != NULL)
		return TRUE;
	
	// Create the symbol used when building the bytecode
	arB_package* const symbol = p->compiled.symbol = arB_package_new(&p->signature.name);
	if (symbol == NULL)
		return FALSE;

	arB_package_set_signature(symbol, &p->signature.signature);
	arBuilder_add_package(c->pipeline, symbol);

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
	// Already compiled
	if (p->compiled.symbol != NULL)
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

	// Create the symbol used when building the bytecode
	arB_type* const type = p->compiled.symbol = arB_type_new(&p->name);
	if (type == NULL)
		return FALSE;

	arB_type_set_signature(type, &p->resolved.signature.signature);
	type->size = p->resolved.stack_size;
	type->data_type = p->resolved.data_type;
	type->flags = p->flags;

	// #11 we should allow types inside functions
	arB_package* symbol = p->package->compiled.symbol;
	int ret = arB_package_add_type(symbol, type);
	if (ret != VMP_LIST_ADDED) {
		return FALSE;
	}

	return TRUE;
}

BOOL arC_syntax_tree_compile_funcdef(arCompiler* c, arC_syntax_tree_funcdef* p)
{
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

