#include "syntax_tree_resolve.h"
#include "messages.h"
#include "search.h"
#include "../arCompiler.h"
#include "../arString.h"
#include "../arStringPool.h"
#include "../arUtils.h"

// Is the supplied node part of the recursion tree? Then give us the item which closest path to where
// the recursion happened in the first place
const arC_syntax_tree* arC_recursion_tracker_find(const arC_recursion_tracker* rt, const arC_syntax_tree* st)
{
	// Search upwards in the syntax tree to see if we found the supplied tree node. If so, then
	// return the first child item that caused the recusion in the first place.
	//
	// Minimal recursion is: A -> B -> A

	// No parent? Then no recursion is possible
	if (rt->parent == NULL)
		return NULL;
	
	const arC_recursion_tracker* prev = rt;
	rt = rt->parent;
	while (rt != NULL) {
		if (rt->node == st) {
			return prev->node;
		}
		prev = rt;
		rt = rt->parent;
	}
	return NULL;
}

BOOL arC_syntax_tree_resolve_import(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_import* ref)
{
	if (ref->resolved.ref != NULL)
		return TRUE;
	
	// The last block in the "import block" tree should point to the actual package we are importing.
	// If it's ref is NULL then the package is not found
	const arC_syntax_tree_import_block* block = (const arC_syntax_tree_import_block*)ref->header.child_head;
	while (1) {
		// Check to see if the block failed to be resolved
		if (block->resolved.ref == NULL) {
			// TODO: The package name should be the signature and not the block name
			return arC_message_symbol_unresolved(s, &block->name);
		}
		// Current block is the last last element in the parent-child relationship
		if (block->header.child_head == NULL) {
			break;
		}
		block = (const arC_syntax_tree_import_block*)ref->header.child_head;
		ASSERT_NOT_NULL(block);
	}
	ref->resolved.ref = block->resolved.ref;
	return TRUE;
}

BOOL arC_syntax_tree_resolve_import_block(const arC_state* s, arC_syntax_tree_import_block* ref)
{
	assert(ref->header.parent != NULL);

	// Figure out where to start searching for the package
	arC_search_upwards_context ctx;
	if (ref->header.parent->type == arC_SYNTAX_TREE_IMPORT) {
		// We are searching from the current package and the root node's perspective
		arC_search_upwards_begin(asC_syntax_tree(ref->package), &ref->name, 
			BIT(arC_SYNTAX_TREE_PACKAGE), 0, &ctx);
	}
	else {
		// A block must have a block node as a parent
		assert(ref->header.parent->type == arC_SYNTAX_TREE_IMPORT_BLOCK);
		arC_syntax_tree_import_block* parent_block = (arC_syntax_tree_import_block*)ref->header.parent;
		// We are searching from the parent block's parent's point of view
		arC_search_upwards_begin(asC_syntax_tree(parent_block->package), &ref->name, 
			BIT(arC_SYNTAX_TREE_PACKAGE), 0, &ctx);
	}
	arC_syntax_tree_node hit = arC_search_upwards_next(&ctx);
	if (hit == NULL) {
		arC_message_symbol_unresolved(s, &ref->name);
		return FALSE;
	}
	ref->resolved.ref = (arC_syntax_tree_package*)hit;
	return TRUE;
}

BOOL arC_syntax_tree_resolve_typeref(const arC_state* s, arC_syntax_tree_typeref* ref)
{
	arC_search_upwards_context ctx;
	arC_search_upwards_begin(asC_syntax_tree(ref), &ref->name, ref->valid_types,
		arC_SEARCH_FLAG_BACKWARDS | arC_SEARCH_FLAG_INCLUDE_IMPORTS, &ctx);
	arC_syntax_tree_node hit = arC_search_upwards_next(&ctx);
	if (hit == NULL) {
		arC_message_symbol_unresolved(s, &ref->name);
		return FALSE;
	}
	ref->resolved.def = hit;
	// TODO: What if multiple hits are found?
	return TRUE;
}

BOOL arC_syntax_tree_resolve_typedef(const arC_state* s, const arC_recursion_tracker* rt, 
	arC_syntax_tree_typedef* def)
{
	ASSERT_NOT_NULL(def->package);

	// The type is already resolved
	if (def->resolved.stack_size != -1) {
		return TRUE;
	}

	const arC_syntax_tree* found = arC_recursion_tracker_find(rt, asC_syntax_tree(def));
	if (found != NULL) {
		// TODO: Recursion detected in the direction of <def> -> <found>
		arC_message_type_recursive_dependency(s, "insert type here");
		return FALSE;
	}

	// Does this type inherit from another type in a way that affects it's memory consumption
	if (def->inherits_from != NULL && def->resolved.inherits_from == NULL) {
		const arString str = def->inherits_from->name;
		arC_search_upwards_context ctx;
		arC_search_upwards_begin(asC_syntax_tree(def), &str,
			def->inherits_from->valid_types,
			arC_SEARCH_FLAG_BACKWARDS | arC_SEARCH_FLAG_INCLUDE_IMPORTS, &ctx);
		arC_syntax_tree_node hit = arC_search_upwards_next(&ctx);
		if (hit == NULL) {
			arC_message_symbol_unresolved(s, &str);
			return FALSE;
		}
		assert(hit->type == arC_SYNTAX_TREE_TYPEDEF &&
			"add support for other inherit-types here, such as functions");
		def->resolved.inherits_from = (arC_syntax_tree_typedef*)hit;

		// Try to resolve the inherited type. This is so that we can:
		// 1. Know the total stack size
		// 2. Know that recursion is not detected
		// 
		// The following recursions should be prevented:
		// type A : B {}
		// type B : A {}
		const arC_recursion_tracker child_rt = arC_recursion_tracker_next(rt, 
			asC_syntax_tree(def->resolved.inherits_from));
		if (!arC_syntax_tree_resolve_typedef(s, &child_rt, def->resolved.inherits_from)) {
			return FALSE;
		}
		
	}

	// If this type is indirectly dependent of a specific type. This type of dependency does not always affect
	// the type's underlying memory consumption.
	if (def->of_type != NULL && def->resolved.of_type == NULL) {
		const arString str = def->of_type->name;
		arC_search_upwards_context ctx;
		arC_search_upwards_begin(asC_syntax_tree(def), &str,
			def->of_type->valid_types,
			arC_SEARCH_FLAG_BACKWARDS | arC_SEARCH_FLAG_INCLUDE_IMPORTS, &ctx);
		arC_syntax_tree_node hit = arC_search_upwards_next(&ctx);
		if (def->resolved.of_type == NULL) {
			arC_message_symbol_unresolved(s, &def->of_type->name);
			return FALSE;
		}
		assert(hit->type == arC_SYNTAX_TREE_TYPEDEF &&
			"add support for other inherit-types here, such as functions");
		def->resolved.of_type = (arC_syntax_tree_typedef*)hit;

		// If this is an array then have to resolve the underlying type because we
		// have to know about it's memory consumption
		// 
		// The following recursions should be prevented:
		// type A : struct {
		//   Value [2]A
		// }
		if ((def->flags & arB_TYPE_FLAGS_ARRAY) != 0) {
			const arC_recursion_tracker child_rt = arC_recursion_tracker_next(rt, 
				asC_syntax_tree(def->resolved.of_type));
			if (!arC_syntax_tree_resolve_typedef(s, &child_rt, def->resolved.of_type)) {
				return FALSE;
			}
		}
	}

	// Special cases to consider:
	// type A : struct {}
	// type B : *A {}

	// Calculate the stack size etc
	def->resolved.stack_size = 0;
	if (def->resolved.inherits_from != NULL) {
		def->resolved.stack_size += def->resolved.inherits_from->resolved.stack_size;
		def->resolved.data_type = def->resolved.inherits_from->resolved.data_type;
	}
	if ((def->flags & arB_TYPE_FLAGS_PTR) != 0) {
		assert(def->resolved.inherits_from == NULL && 
			"You are not allowed to both be a pointer and inherit from a value at the same time");
		def->resolved.stack_size += sizeof(void*);
		def->resolved.data_type = ARLANG_PRIMITIVE_PTR;
	}
	// TODO: Iterate over all members
	return TRUE;
}

BOOL arC_syntax_tree_resolve_references0(const arC_state* s, const arC_recursion_tracker* rt,
	const arC_syntax_tree* st)
{
	ASSERT_NOT_NULL(s);
	ASSERT_NOT_NULL(st);
	// Resolve before
	switch (st->type)
	{
	case arC_SYNTAX_TREE_IMPORT_BLOCK:
		if (!arC_syntax_tree_resolve_import_block(s, (arC_syntax_tree_import_block*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_TYPEDEF:
		if (!arC_syntax_tree_resolve_typedef(s, rt, (arC_syntax_tree_typedef*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_TYPEDEF_MEMBERDEF:
		break;
	case arC_SYNTAX_TREE_TYPEREF:
		if (!arC_syntax_tree_resolve_typeref(s, (arC_syntax_tree_typeref*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_TYPEREF_MEMBERREF:
		break;		
	}

	// Resolve children
	arC_syntax_tree_node child = st->child_head;
	while (child) {
		const arC_recursion_tracker child_rt = arC_recursion_tracker_next(rt, child);
		if (!arC_syntax_tree_resolve_references0(s, &child_rt, child))
			return FALSE;
		child = child->tail;
	}

	// Resolve after
	switch (st->type)
	{
	case arC_SYNTAX_TREE_IMPORT:
		if (!arC_syntax_tree_resolve_import(s, rt, (arC_syntax_tree_import*)st))
			return FALSE;
		break;
	}

	return TRUE;
}

BOOL arC_syntax_tree_resolve_references(const arC_state* s, const arC_syntax_tree* st)
{
	arC_recursion_tracker rt = { NULL, NULL, st };
	return arC_syntax_tree_resolve_references0(s, &rt, st);
}
