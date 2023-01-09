#include "syntax_tree_resolve.h"
#include "messages.h"
#include "syntax_tree_search.h"
#include "../arCompiler.h"
#include "../arString.h"
#include "../arStringPool.h"
#include "../arUtils.h"

BOOL arC_syntax_tree_resolve_references0(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree* st);

// Resolve all child nodes
BOOL arC_syntax_tree_resolve_children(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree* st)
{
	arC_syntax_tree_node child = st->child_head;
	while (child) {
		const arC_recursion_tracker child_rt = arC_recursion_tracker_next(rt, child);
		if (!arC_syntax_tree_resolve_references0(s, &child_rt, child))
			return FALSE;
		child = child->tail;
	}
	return TRUE;
}

// Resolve a node
BOOL arC_syntax_tree_resolve_node(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree* node)
{
	if (BIT_ISSET(node->phase, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;
	if (!arC_syntax_tree_resolve_children(s, rt, node)) {
		return FALSE;
	}
	node->phase |= arC_SYNTAX_TREE_PHASE_RESOLVE;
	return TRUE;
}

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

BOOL arC_syntax_tree_resolve_package_signature(const arC_state* s, arC_syntax_tree_package* ref)
{
	// Calculate the package signature
	arByte signature_data[1024];
	arByte* sig = signature_data;
	// If We don't have a parent AND the parent isn't the root parent
	if (ref->header.parent != NULL && ref->header.parent->parent) {
		arC_syntax_tree_package* const parent = (arC_syntax_tree_package*)ref->header.parent;
		sig = arString_cpy(sig, &parent->resolve.signature);
		sig = arStrcpy(sig, ".", 1);
	}
	sig = arString_cpy(sig, &ref->name);
	const arString* const result = arStringPool_stringsz(arC_state_get_string_pool(s), signature_data, (int)(sig - signature_data));
	if (result == NULL)
		return arC_message_out_of_memory(s);
	else
		ref->resolve.signature = *result;
	return TRUE;
}

BOOL arC_syntax_tree_resolve_ref(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_ref* ref)
{
	if (asC_syntax_tree_phase_done(ref, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;
	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(ref)))
		return FALSE;

	// TODO: Take distance to the node into consideration
	if (ref->resolved.nodes[0] == NULL) {
		const arC_syntax_tree_ref_block* block = (arC_syntax_tree_ref_block*)asC_syntax_tree_first_child(ref);
		while (1) {
			if (block->header.child_head != block->header.child_tail) {
				// TODO: Take metadata into consideration, which should be
				//       sibling nodes
				return arC_message_feature_missing(s, "import syntax trees are not allowed to have siblings yet");
			}
			// Current block is the last last element in the parent-child relationship
			if (block->header.child_head == NULL) {
				break;
			}
			block = (const arC_syntax_tree_ref_block*)block->header.child_head;
		}

		for (int i = 0; i < block->resolved.nodes_count; ++i)
			ref->resolved.nodes[i] = block->resolved.nodes[i];
		ref->resolved.nodes_count = block->resolved.nodes_count;
	}

	asC_syntax_tree_phase_set(ref, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_ref_block(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_ref_block* ref)
{
	if (asC_syntax_tree_phase_done(ref, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (ref->resolved.nodes[0] == NULL) {
		// Is the parents a reference then continue search the syntax tree from that node's point of view
		if (ref->header.parent->type == arC_SYNTAX_TREE_REF_BLOCK) {
			int found_index = 0;

			// Make sure to search among all the parents results because we don't really know which result
			// is the actual parent until we've iterated over all blocks
			const arC_syntax_tree_ref_block* const parent_block = (arC_syntax_tree_ref_block*)ref->header.parent;
			for (int i = 0; i < parent_block->resolved.nodes_count; ++i) {
				arC_syntax_tree_node resolved = parent_block->resolved.nodes[i];
				arC_syntax_tree_search_items* const items = arC_syntax_tree_search_visit(s, resolved, &ref->query, ref->types, arC_SEARCH_FLAG_INCLUDE_CHILDREN);
				if (items->count > 0) {
					arC_syntax_tree_search_item* item = items->first;
					while (item) {
						if (found_index >= arC_syntax_tree_ref_block_max_nodes)
							return arC_message_resolve_not_implemented(s, "arC_syntax_tree_ref_block_max_nodes is to small");
						ref->resolved.nodes[found_index++] = item->node;
						// TODO: Add support for sorting items. Although, theoretically, the 
						//       "arC_syntax_tree_search_visit" function should automatically give us
						//       the closest nodes first
						if (ref->resolved.closest_distance > item->distance) {
							ref->resolved.closest_distance = item->distance;
						}
						item = item->tail;
					}
				}
				arC_syntax_tree_search_visit_destroy(items);
			}
			ref->resolved.nodes_count = found_index;
		}
		else {
			int found_index = 0;
			arC_syntax_tree_search_items* const items = arC_syntax_tree_search_visit(s, asC_syntax_tree(ref), &ref->query, ref->types,
				arC_SEARCH_FLAG_BACKWARDS | arC_SEARCH_FLAG_INCLUDE_IMPORTS | arC_SEARCH_FLAG_INCLUDE_CHILDREN |
				arC_SEARCH_FLAG_INCLUDE_PARENTS);
			if (items->count > 0) {
				arC_syntax_tree_search_item* item = items->first;
				while (item) {
					if (found_index >= arC_syntax_tree_ref_block_max_nodes)
						return arC_message_resolve_not_implemented(s, "arC_syntax_tree_ref_block_max_nodes is to small");
					ref->resolved.nodes[found_index++] = item->node;
					// TODO: Add support for sorting items. Although, theoretically, the 
					//       "arC_syntax_tree_search_visit" function should automatically give us
					//       the closest nodes first
					if (ref->resolved.closest_distance > item->distance) {
						ref->resolved.closest_distance = item->distance;
					}
					item = item->tail;
				}
				ref->resolved.nodes_count = found_index;
			}
			arC_syntax_tree_search_visit_destroy(items);
		}
	}
	asC_syntax_tree_phase_set(ref, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(ref));
}

BOOL arC_syntax_tree_resolve_package(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_package* ref)
{
	if (asC_syntax_tree_phase_done(ref, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;
	arC_syntax_tree_resolve_package_signature(s, ref);
	ref->compiled.symbol = arB_package_new(&ref->name);
	asC_syntax_tree_phase_set(ref, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(ref));
}

BOOL arC_syntax_tree_resolve_import(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_import* ref)
{
	if (asC_syntax_tree_phase_done(ref, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;
	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(ref)))
		return FALSE;

	if (ref->resolved.ref == NULL) {
		// Hierarchy should be import -> typeref -> ref_block...
		const arC_syntax_tree_ref* const ref2 = (arC_syntax_tree_ref*)asC_syntax_tree_first_child(ref);
		ref->resolved.ref = 
			(arC_syntax_tree_package*)arC_syntax_tree_ref_find_first(ref2, arC_SYNTAX_TREE_PACKAGE);
		if (ref->resolved.ref == NULL) {
			return arC_message_feature_missing(s, "TODO: Could not find package (What is the package here?)");
		}
	}
	asC_syntax_tree_phase_set(ref, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_arg(const arC_state* s, const arC_recursion_tracker* rt, 
	arC_syntax_tree_funcdef_arg* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node))) {
		return FALSE;
	}

	node->compiled.symbol = arB_arg_new();
	arB_arg_set_name(node->compiled.symbol, &node->name);
	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_ret(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcdef_ret* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node))) {
		return FALSE;
	}

	node->compiled.symbol = arB_return_new();
	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_local(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcdef_local* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node))) {
		return FALSE;
	}

	node->compiled.symbol = arB_local_new();
	arB_local_set_name(node->compiled.symbol, &node->name);
	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_typeref(const arC_state* s, const arC_recursion_tracker* rt, 
	arC_syntax_tree_typeref* typeref)
{
	if (asC_syntax_tree_phase_done(typeref, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	// Resolve type typeref block nodes before resolving typeref node
	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(typeref)))
		return FALSE;

	if (typeref->resolved.def == NULL) {
		// Hierarchy could be typeref -> typeref -> ref_block...
		arC_syntax_tree* const first_child = asC_syntax_tree_first_child(typeref);
		if (first_child == NULL)
			return arC_message_feature_missing(s, "TODO: Could not find type (What is the type signature here?)");

		if (first_child->type == arC_SYNTAX_TREE_REF) {
			const arC_syntax_tree_ref* const ref = (arC_syntax_tree_ref*)first_child;
			typeref->resolved.def =
				(arC_syntax_tree_typedef*)arC_syntax_tree_ref_find_first(ref, arC_SYNTAX_TREE_TYPEDEF);
		}
		else if (first_child->type == arC_SYNTAX_TREE_TYPEREF_IMPLICIT) {
			const arC_syntax_tree_typeref_implicit* const ref = (arC_syntax_tree_typeref_implicit*)first_child;
			typeref->resolved.def = ref->resolved.def;
		}

		if (typeref->resolved.def == NULL) {
			return arC_message_feature_missing(s, "TODO: Could not find type (What is the type signature here?)");
		}
	}
	asC_syntax_tree_phase_set(typeref, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_funcdef_signature(const arC_state* s, arC_syntax_tree_funcdef* node)
{
	// Calculate the package signature
	int bytes_left = 1024;
	arByte signature_data[1024];
	arByte* sig = signature_data;

	// TODO: Add support for functions in functions
	if (node->header.parent->type != arC_SYNTAX_TREE_PACKAGE) {
		return arC_message_not_implemented(s, "#7 add support for functions inside functions");
	}

	arC_syntax_tree_package* package = (arC_syntax_tree_package*)node->header.parent;
	sig = arString_cpy_s(sig, &bytes_left, &package->resolve.signature);
	if (bytes_left == 0) return arC_message_not_implemented(s, "dynamic set memory size?");
	sig = arStrcpy_s(sig, &bytes_left, "#", 1);
	if (bytes_left == 0) return arC_message_not_implemented(s, "dynamic set memory size?");
	sig = arString_cpy_s(sig, &bytes_left, &node->name);
	if (bytes_left == 0) return arC_message_not_implemented(s, "dynamic set memory size?");

	sig = arStrcpy_s(sig, &bytes_left, "(", 1);
	if (bytes_left == 0) return arC_message_not_implemented(s, "dynamic set memory size?");
	if (node->args != NULL) {
		arC_syntax_tree* arg_node = node->args->header.child_head;
		while (arg_node) {
			if (arg_node != node->args->header.child_head) {
				sig = arStrcpy_s(sig, &bytes_left, ",", 1);
			}
			arC_syntax_tree_funcdef_arg* const arg = (arC_syntax_tree_funcdef_arg*)arg_node;
			arC_syntax_tree_typedef* const type = arg->type->resolved.def;
			const arString* type_sig = &type->resolved.signature;
			sig = arStrcpy_s(sig, &bytes_left, type_sig->start, arString_length(type_sig));
			arg_node = arg_node->tail;
		}
	}
	sig = arStrcpy_s(sig, &bytes_left, ")", 1);
	if (bytes_left == 0) return arC_message_not_implemented(s, "dynamic set memory size?");

	const arString* const result = arStringPool_stringsz(arC_state_get_string_pool(s), signature_data, (int)(sig - signature_data));
	if (result == NULL)
		return arC_message_out_of_memory(s);
	else
		node->resolve.signature = *result;
	return TRUE;
}

BOOL arC_syntax_tree_resolve_funcdef(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcdef* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;
	node->compiled.symbol = arB_func_new(&node->name);
	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node)))
		return FALSE;
	return arC_syntax_tree_resolve_funcdef_signature(s, node);
}

BOOL arC_syntax_tree_resolve_funcref(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcref* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;
	
	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node)))
		return FALSE;


	

	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_funcdef_body_binop(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcdef_body_binop* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node)))
		return FALSE;

	arC_syntax_tree_node left = node->header.child_head;
	arC_syntax_tree_typedef* const left_type = arC_syntax_tree_get_stack_type(left);
	
	arC_syntax_tree_node right = left->tail;
	arC_syntax_tree_typedef* const right_type = arC_syntax_tree_get_stack_type(right);

	// TODO: Verify that the resolved types of each node are compatible for the operator
	// TODO: Type might be modified due to the operator
	node->resolved.type = right_type;

	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_funcdef_body_unaryop(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcdef_body_unaryop* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node))) {
		return FALSE;
	}

	arC_syntax_tree_node left = node->header.child_head;
	arC_syntax_tree_typedef* const left_type = arC_syntax_tree_get_stack_type(left);
	// TODO: The type might be modified due to the operator
	node->resolved.type = left_type;

	// TODO: ??

	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_funcdef_body_const_value(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcdef_body_const_value* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node));
}

BOOL arC_syntax_tree_resolve_funcdef_body_varref(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_funcdef_body_varref* node)
{
	if (asC_syntax_tree_phase_done(node, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(node))) {
		return FALSE;
	}

	if (node->resolved.node == NULL) {
		// Hierarchy should be varref -> typeref -> ref_block...
		const arC_syntax_tree_ref* const ref2 = (arC_syntax_tree_ref*)asC_syntax_tree_first_child(node);
		node->resolved.node = ref2->resolved.nodes[0];
		if (node->resolved.node == NULL) {
			return arC_message_feature_missing(s, "TODO: Could not find variable definition here (What is the type signature here?)");
		}
	}

	asC_syntax_tree_phase_set(node, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_typedef(const arC_state* s, const arC_recursion_tracker* rt, 
	arC_syntax_tree_typedef* def)
{
	ASSERT_NOT_NULL(def->package);

	// The type is already resolved
	if (asC_syntax_tree_phase_done(def, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;
	def->compiled.symbol = arB_type_new(&def->name);

	// Resolve children first
	if (!arC_syntax_tree_resolve_children(s, rt, asC_syntax_tree(def)))
		return FALSE;

	const arC_syntax_tree* found = arC_recursion_tracker_find(rt, asC_syntax_tree(def));
	if (found != NULL) {
		// TODO: Recursion detected in the direction of arg:<def> -> local:<found>
		arC_message_type_recursive_dependency(s, "insert type here");
		return FALSE;
	}

	// Does this type inherit from another type in a way that affects it's memory consumption
	if (def->inherits_from != NULL) {
		arC_syntax_tree_typeref* const inherits_from = def->inherits_from;
		if (inherits_from->resolved.def == NULL) {
			const arC_recursion_tracker child_rt1 = arC_recursion_tracker_next(rt, asC_syntax_tree(inherits_from));
			if (!arC_syntax_tree_resolve_typeref(s, &child_rt1, inherits_from))
				return FALSE;

			// Try to resolve the inherited type. This is so that we can:
			// 1. Know the total stack size
			// 2. Know that recursion is not detected
			// 
			// The following recursions should be prevented:
			// type A : B {}
			// type B : A {}
			const arC_recursion_tracker child_rt2 = arC_recursion_tracker_next(rt, 
				asC_syntax_tree(inherits_from->resolved.def));
			if (!arC_syntax_tree_resolve_typedef(s, &child_rt2, inherits_from->resolved.def)) {
				return FALSE;
			}
		}
	}

	// If this type is indirectly dependent of a specific type. This type of dependency does not always affect
	// the type's underlying memory consumption.
	if (def->of_type != NULL) {
		arC_syntax_tree_typeref* const of_type = def->of_type;
		if (of_type->resolved.def == NULL) {
			const arC_recursion_tracker child_rt = arC_recursion_tracker_next(rt, asC_syntax_tree(of_type));
			if (!arC_syntax_tree_resolve_typeref(s, &child_rt, of_type))
				return FALSE;

			// If this is an array then have to resolve the underlying type because we
			// have to know about it's memory consumption
			// 
			// The following recursions should be prevented:
			// type A : struct {
			//   Value [2]A
			// }
			if ((def->flags & arB_TYPE_FLAGS_ARRAY) != 0) {
				const arC_recursion_tracker child_rt = arC_recursion_tracker_next(rt, 
					asC_syntax_tree(of_type->resolved.def));
				if (!arC_syntax_tree_resolve_typedef(s, &child_rt, of_type->resolved.def)) {
					return FALSE;
				}
			}
		}
	}

	// Special cases to consider:
	// type A : struct {}
	// type B : *A {}

	// Calculate the stack size etc
	def->resolved.stack_size = 0;
	if (def->inherits_from != NULL) {
		arC_syntax_tree_typedef* const inherits_from = def->inherits_from->resolved.def;
		def->resolved.stack_size += inherits_from->resolved.stack_size;
		def->resolved.data_type = inherits_from->resolved.data_type;
	}
	if ((def->flags & arB_TYPE_FLAGS_PTR) != 0) {
		// All pointers inherit from *void
		def->resolved.stack_size += sizeof(void*);
		def->resolved.data_type = ARLANG_PRIMITIVE_PTR;
	}
	// TODO: Iterate over all members

	asC_syntax_tree_phase_set(def, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

arC_syntax_tree_typedef* arC_syntax_tree_resolve_typeref_implicit0(const arC_state* s,
	const arC_recursion_tracker* rt, arC_syntax_tree_node node)
{
	//
	if (node->type == arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE) {
		arC_syntax_tree_funcdef_body_const_value* constval = (arC_syntax_tree_funcdef_body_const_value*)node;
		// constant values have a child reference
		arC_syntax_tree_typeref* ref = (arC_syntax_tree_typeref*)asC_syntax_tree_first_child(constval);
		if (!arC_syntax_tree_resolve_typeref(s, rt, ref)) {
			return NULL;
		}
		return ref->resolved.def;
	}
	else if (node->type == arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP) {
		arC_syntax_tree_funcdef_body_binop* const binop = (arC_syntax_tree_funcdef_body_binop*)node;
		if (!arC_syntax_tree_resolve_funcdef_body_binop(s, rt, binop)) {
			return NULL;
		}
		return binop->resolved.type;
	}
	else if (node->type == arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP) {
		arC_syntax_tree_funcdef_body_unaryop* const unaryop = (arC_syntax_tree_funcdef_body_unaryop*)node;
		if (!arC_syntax_tree_resolve_funcdef_body_unaryop(s, rt, unaryop)) {
			return NULL;
		}
		return unaryop->resolved.type;
	}
	else {
		// TODO: Add more implicit resolve features
		return NULL;
	}

	return NULL;
}

BOOL arC_syntax_tree_resolve_typeref_implicit(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree_typeref_implicit* implicit)
{
	if (asC_syntax_tree_phase_done(implicit, arC_SYNTAX_TREE_PHASE_RESOLVE))
		return TRUE;

	if (implicit->resolved.def == NULL) {
		implicit->resolved.def = arC_syntax_tree_resolve_typeref_implicit0(s, rt, implicit->implicit_from);
		// Could not implicitly resolve the type from statements
		if (implicit->resolved.def == NULL) {
			return arC_message_resolve_not_implemented(s, "TODO: Could not find type (What is the type signature here?)");
		}
	}

	asC_syntax_tree_phase_set(implicit, arC_SYNTAX_TREE_PHASE_RESOLVE);
	return TRUE;
}

BOOL arC_syntax_tree_resolve_references0(const arC_state* s, const arC_recursion_tracker* rt,
	arC_syntax_tree* st)
{
	ASSERT_NOT_NULL(s);
	ASSERT_NOT_NULL(st);

	// Resolve before
	switch (st->type)
	{
	case arC_SYNTAX_TREE_REF:
		if (!arC_syntax_tree_resolve_ref(s, rt, (arC_syntax_tree_ref*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_REF_BLOCK:
		if (!arC_syntax_tree_resolve_ref_block(s, rt, (arC_syntax_tree_ref_block*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_PACKAGE:
		if (!arC_syntax_tree_resolve_package(s, rt, (arC_syntax_tree_package*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_IMPORT:
		if (!arC_syntax_tree_resolve_import(s, rt, (arC_syntax_tree_import*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_TYPEDEF:
		if (!arC_syntax_tree_resolve_typedef(s, rt, (arC_syntax_tree_typedef*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_TYPEREF:
		if (!arC_syntax_tree_resolve_typeref(s, rt, (arC_syntax_tree_typeref*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_TYPEREF_IMPLICIT:
		if (!arC_syntax_tree_resolve_typeref_implicit(s, rt, (arC_syntax_tree_typeref_implicit*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF_ARG:
		if (!arC_syntax_tree_resolve_arg(s, rt, (arC_syntax_tree_funcdef_arg*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF_RET:
		if (!arC_syntax_tree_resolve_ret(s, rt, (arC_syntax_tree_funcdef_ret*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF_LOCAL:
		if (!arC_syntax_tree_resolve_local(s, rt, (arC_syntax_tree_funcdef_local*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF:
		if (!arC_syntax_tree_resolve_funcdef(s, rt, (arC_syntax_tree_funcdef*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCREF:
		if (!arC_syntax_tree_resolve_funcref(s, rt, (arC_syntax_tree_funcref*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE:
		if (!arC_syntax_tree_resolve_funcdef_body_const_value(s, rt, (arC_syntax_tree_funcdef_body_const_value*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP:
		if (!arC_syntax_tree_resolve_funcdef_body_binop(s, rt, (arC_syntax_tree_funcdef_body_binop*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_UNARYOP:
		if (!arC_syntax_tree_resolve_funcdef_body_unaryop(s, rt, (arC_syntax_tree_funcdef_body_unaryop*)st))
			return FALSE;
		break;
	case arC_SYNTAX_TREE_FUNCDEF_BODY_VARREF:
		if (!arC_syntax_tree_resolve_funcdef_body_varref(s, rt, (arC_syntax_tree_funcdef_body_varref*)st))
			return FALSE;
		break;
	default:
		if (!arC_syntax_tree_resolve_node(s, rt, st)) {
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL arC_syntax_tree_resolve_references(const arC_state* s, arC_syntax_tree* st)
{
	arC_recursion_tracker rt = { NULL, NULL, st };
	return arC_syntax_tree_resolve_references0(s, &rt, st);
}
