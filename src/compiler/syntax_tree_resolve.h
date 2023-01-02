#ifndef _ARC_SYNTAX_TREE_RESOLVE_H_
#define _ARC_SYNTAX_TREE_RESOLVE_H_

#include "syntax_tree.h"

// Helper struct which prevents recursive dependencies, for example types that inherit from each other
typedef struct arC_recursion_tracker {
	// Root tree
	const struct arC_recursion_tracker* root;
	// Parent
	const struct arC_recursion_tracker* parent;
	// Node we are resolving
	const arC_syntax_tree* node;
} arC_recursion_tracker;

#define arC_recursion_tracker_next(rt, next) \
	{ rt->root, rt, next }

// Resolve all references found in the supplied syntax tree
ARLANG_API BOOL arC_syntax_tree_resolve_references(const arC_state* s, const arC_syntax_tree* st);

#endif
