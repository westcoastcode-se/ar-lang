#include "syntax_tree_search.h"
#include "syntax_tree.h"

//
void arC_syntax_tree_search_visit0(arC_syntax_tree* node, const arString* query,
	arC_syntax_tree_search_type_bits types,
	arInt32 flags, arC_syntax_tree_search_items* items, arInt32 distance);

// Search forwards
void arC_syntax_tree_search_forwards(arC_syntax_tree* node, arC_syntax_tree* origin, const arString* query,
	arC_syntax_tree_search_type_bits types,
	arInt32 flags, arC_syntax_tree_search_items* items, arInt32 distance);

static arC_syntax_tree_search_items arC_syntax_tree_search_items_empty = {NULL, NULL, NULL, 0, NULL};

arC_syntax_tree_node arC_syntax_tree_search_get_package(arC_syntax_tree_import* node)
{
	if (node->resolved.ref == NULL)
		return NULL;
	return asC_syntax_tree(node->resolved.ref);
}

void arC_syntax_tree_search_memory_init(arC_syntax_tree_search_memory* memory)
{
	memory->item_head = NULL;
	memory->items_head = NULL;
}

arC_syntax_tree_search_items* arC_syntax_tree_search_memory_get_items(arC_syntax_tree_search_memory* mem)
{
	ASSERT_NOT_NULL(mem);
	if (mem->items_head != NULL) {
		arC_syntax_tree_search_items* const items = mem->items_head;
		mem->items_head = items->tail;
		items->tail = NULL;
		items->count = 0;
		return items;
	}
	else {
		arC_syntax_tree_search_items* const items = (arC_syntax_tree_search_items*)arMalloc(sizeof(arC_syntax_tree_search_items));
		if (items == NULL)
			return NULL;
		items->first = items->last = NULL;
		items->memory = mem;
		items->tail = NULL;
		items->count = 0;
		return items;
	}
}

arC_syntax_tree_search_item* arC_syntax_tree_search_memory_get_item(arC_syntax_tree_search_memory* mem)
{
	ASSERT_NOT_NULL(mem);
	if (mem->item_head != NULL) {
		arC_syntax_tree_search_item* const item = mem->item_head;
		mem->item_head = item->tail;
		item->tail = NULL;
		return item;
	}
	else {
		arC_syntax_tree_search_item* const item = (arC_syntax_tree_search_item*)arMalloc(sizeof(arC_syntax_tree_search_item));
		item->memory = mem;
		item->tail = NULL;
		return item;
	}
}

void arC_syntax_tree_search_memory_add_items(arC_syntax_tree_search_memory* mem, arC_syntax_tree_search_items* items)
{
	if (mem->items_head) {
		items->tail = mem->items_head;
		mem->items_head = items;
	}
	else {
		items->tail = NULL;
		mem->items_head = items;
	}
}

void arC_syntax_tree_search_memory_add_item(arC_syntax_tree_search_memory* mem, arC_syntax_tree_search_item* item)
{
	if (mem->item_head) {
		item->tail = mem->item_head;
		mem->item_head = item;
	}
	else {
		item->tail = NULL;
		mem->item_head = item;
	}
}

void arC_syntax_tree_search_memory_release(arC_syntax_tree_search_memory* memory)
{
	ASSERT_NOT_NULL(memory);

	arC_syntax_tree_search_items* items = memory->items_head;
	while (items) {
		arC_syntax_tree_search_items* const tail = items->tail;
		arFree(items);
		items = tail;
	}
	memory->items_head = NULL;

	arC_syntax_tree_search_item* item = memory->item_head;
	while (item) {
		arC_syntax_tree_search_item* const tail = item->tail;
		arFree(item);
		item = tail;
	}
	memory->item_head = NULL;
}

BOOL arC_syntax_tree_search_has_name(arC_syntax_tree* node, const arString* name)
{
	switch (node->type) {
	case arC_SYNTAX_TREE_TYPEDEF:
		return arString_cmp(&((arC_syntax_tree_typedef*)node)->name, name);
	case arC_SYNTAX_TREE_PACKAGE:
		return arString_cmp(&((arC_syntax_tree_package*)node)->name, name);
	case arC_SYNTAX_TREE_FUNCDEF:
		return arString_cmp(&((arC_syntax_tree_funcdef*)node)->head->name, name);
	case arC_SYNTAX_TREE_FUNCDEF_ARG:
		return arString_cmp(&((arC_syntax_tree_funcdef_arg*)node)->name, name);
	case arC_SYNTAX_TREE_FUNCDEF_LOCAL:
		return arString_cmp(&((arC_syntax_tree_funcdef_local*)node)->name, name);
	default:
		return FALSE;
	}
}

BOOL arC_syntax_tree_search_is_type(arC_syntax_tree* node, arC_syntax_tree_search_type_bits types)
{
	switch (node->type)
	{
	case arC_SYNTAX_TREE_TYPEDEF:
		return BIT_ISSET(types, arC_SYNTAX_TREE_SEARCH_TYPE_TYPEDEF);
	case arC_SYNTAX_TREE_PACKAGE:
		return BIT_ISSET(types, arC_SYNTAX_TREE_SEARCH_TYPE_PACKAGE);
	case arC_SYNTAX_TREE_FUNCDEF:
		return BIT_ISSET(types, arC_SYNTAX_TREE_SEARCH_TYPE_FUNCDEF);
	case arC_SYNTAX_TREE_FUNCDEF_ARG:
		return BIT_ISSET(types, arC_SYNTAX_TREE_SEARCH_TYPE_ARG);
	case arC_SYNTAX_TREE_FUNCDEF_LOCAL:
		return BIT_ISSET(types, arC_SYNTAX_TREE_SEARCH_TYPE_LOCAL);
	default:
		return FALSE;
	}
}

// Should we include children
BOOL arC_syntax_tree_search_include_children(arC_syntax_tree* node, arInt32 flags)
{
	if ((flags & arC_SEARCH_FLAG_INCLUDE_CHILDREN) == 0)
		return FALSE;
	return TRUE;
}

void arC_syntax_tree_search_items_add(arC_syntax_tree_search_items* items, arC_syntax_tree* node, arInt32 distance)
{
	arC_syntax_tree_search_item* const item = arC_syntax_tree_search_memory_get_item(items->memory);
	item->distance = distance;
	item->node = node;
	if (items->first == NULL) {
		items->first = items->last = item;
	}
	else {
		items->last->tail = item;
		items->last = item;
	}
	items->count++;
}

void arC_syntax_tree_search_backwards(arC_syntax_tree* node, const arString* query,
	arC_syntax_tree_search_type_bits types,
	arInt32 flags, arC_syntax_tree_search_items* items, arInt32 distance)
{
	if (node == NULL)
		return;

	arC_syntax_tree* n = node;
	while (n) {
		// Check if the supplied node has a matching name
		if (arC_syntax_tree_search_is_type(n, types) && arC_syntax_tree_search_has_name(n, query)) {
			arC_syntax_tree_search_items_add(items, n, distance);
		}

		// If the node is an import node and we are allowed to traverse imports
		// then make sure to include those in the search
		if (n->type == arC_SYNTAX_TREE_IMPORT) {
			if (BIT_ISSET(flags, arC_SEARCH_FLAG_INCLUDE_IMPORTS)) {
				// Also, don't search among the imports or children with the exception
				// of the first level
				const arInt32 imports_flags = flags &
					~(arC_SEARCH_FLAG_INCLUDE_IMPORTS | arC_SEARCH_FLAG_INCLUDE_CHILDREN);
				arC_syntax_tree_node package_node = arC_syntax_tree_search_get_package((arC_syntax_tree_import*)n);
				arC_syntax_tree_search_forwards(package_node->child_head, NULL, query, types, imports_flags, items, distance + 1);
			}
		}
		else if (n->type == arC_SYNTAX_TREE_FUNCDEF_ARGS || n->type == arC_SYNTAX_TREE_FUNCDEF_LOCALS) {
			// Special case to allow to search among the arguments or locals as if they are siblings and not children
			const arInt32 args_flags = flags & ~(arC_SEARCH_FLAG_INCLUDE_PARENTS) | arC_SEARCH_FLAG_INCLUDE_CHILDREN;
			arC_syntax_tree_search_forwards(n->child_head, n, query, types, args_flags, items, distance + 1);
		}

		n = n->head;
	}

	// Should the search include the parent node in it's traversal?
	if (BIT_ISSET(flags, arC_SEARCH_FLAG_INCLUDE_PARENTS) && node->parent != NULL) {
		// Don't include a package's parent package
		if (node->parent->type == arC_SYNTAX_TREE_PACKAGE) {
			arInt32 parents_flags = flags & ~(arC_SEARCH_FLAG_INCLUDE_PARENTS);
			arC_syntax_tree_search_forwards(node->parent, node, query, types, parents_flags, items, distance + 1);
		}
		else {
			arC_syntax_tree_search_backwards(node->parent, query, types, flags, items, distance + 1);
		}
	}
}

void arC_syntax_tree_search_forwards(arC_syntax_tree* node, arC_syntax_tree* origin, const arString* query,
	arC_syntax_tree_search_type_bits types,
	arInt32 flags, arC_syntax_tree_search_items* items, arInt32 distance)
{
	if (node == NULL)
		return;

	arC_syntax_tree* n = node;
	while (n) {
		// Check if the supplied node has a matching name
		if (arC_syntax_tree_search_is_type(n, types) && arC_syntax_tree_search_has_name(n, query)) {
			arC_syntax_tree_search_items_add(items, n, distance);
		}

		// If the node is an import node and we are allowed to traverse imports
		// then make sure to include those in the search
		if (node->type == arC_SYNTAX_TREE_IMPORT) {
			if (BIT_ISSET(flags, arC_SEARCH_FLAG_INCLUDE_IMPORTS)) {
				// Also, don't search among the imports or children with the exception
				// of the first level
				const arInt32 imports_flags = flags & 
					~(arC_SEARCH_FLAG_INCLUDE_IMPORTS | arC_SEARCH_FLAG_INCLUDE_CHILDREN);
				arC_syntax_tree_node package_node = arC_syntax_tree_search_get_package((arC_syntax_tree_import*)n);
				arC_syntax_tree_search_forwards(package_node->child_head, NULL, query, types, imports_flags, items, distance + 1);
			}
		}

		// Search among the children if the
		if (arC_syntax_tree_search_include_children(n, flags) && n != node) {
			arC_syntax_tree* child = n->child_head;
			while (child) {
				arC_syntax_tree_search_forwards(child, NULL, query, types, flags, items, distance + 1);
				child = child->tail;
			}
		}

		n = n->tail;
	}
}

void arC_syntax_tree_search_visit0(arC_syntax_tree* node, const arString* query, 
	arC_syntax_tree_search_type_bits types,
	arInt32 flags, arC_syntax_tree_search_items* items, arInt32 distance)
{
	if (BIT_ISSET(flags, arC_SEARCH_FLAG_BACKWARDS))
		arC_syntax_tree_search_backwards(node, query, types, flags, items, distance);
	else 
		arC_syntax_tree_search_forwards(node, NULL, query, types, flags, items, distance);
}

arC_syntax_tree_search_items* arC_syntax_tree_search_visit(const arC_state* state,
	arC_syntax_tree* node, const arString* query, arC_syntax_tree_search_type_bits types, arInt32 flags)
{
	arC_syntax_tree_search_items* const items = arC_syntax_tree_search_memory_get_items(state->search_memory);
	if (items == NULL)
		return &arC_syntax_tree_search_items_empty;
	items->first = items->last = NULL;
	arC_syntax_tree_search_visit0(node, query, types, flags, items, 0);
	return items;
}

arC_syntax_tree* arC_syntax_tree_search_once(const arC_state* s, arC_syntax_tree* node, const arString* query, arC_syntax_tree_search_type type, arInt32 flags)
{
	arC_syntax_tree_search_items* items =
		arC_syntax_tree_search_visit(s, node, query, type, flags);
	if (items->count > 0) {
		arC_syntax_tree* result = items->first->node;
		arC_syntax_tree_search_visit_destroy(items);
		return result;
	}
	arC_syntax_tree_search_visit_destroy(items);
	return NULL;
}

arC_syntax_tree* arC_syntax_tree_search_get_first_type(const arC_syntax_tree_search_items* items, arInt32 type)
{
	if (items == NULL)
		return NULL;
	if (items->first == NULL)
		return NULL;
	arC_syntax_tree_search_item* item = items->first;
	while (item) {
		if (item->node->type == type)
			return item->node;
		item = item->tail;
	}
	return NULL;
}

void arC_syntax_tree_search_visit_destroy(arC_syntax_tree_search_items* items)
{
	ASSERT_NOT_NULL(items);
	if (items == &arC_syntax_tree_search_items_empty)
		return;

	arC_syntax_tree_search_memory* const mem = items->memory;
	arC_syntax_tree_search_item* item = items->first;
	while (item) {
		arC_syntax_tree_search_item* const tail = item->tail;
		arC_syntax_tree_search_memory_add_item(mem, item);
		item = tail;
	}

	arC_syntax_tree_search_memory_add_items(mem, items);
}
