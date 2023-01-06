#ifndef _ARC_SYNTAX_TREE_SEARCH_H_
#define _ARC_SYNTAX_TREE_SEARCH_H_

#include "../arString.h"

typedef enum arC_syntax_tree_search_type
{
	arC_SYNTAX_TREE_SEARCH_TYPE_PACKAGE = 1 << 0,
	arC_SYNTAX_TREE_SEARCH_TYPE_TYPEDEF = 1 << 1,
	arC_SYNTAX_TREE_SEARCH_TYPE_FUNCDEF = 1 << 2,
	arC_SYNTAX_TREE_SEARCH_TYPE_ARG = 1 << 3,
	// .....
} arC_syntax_tree_search_type;

// Types we are searching for among the searchable types
typedef arInt32 arC_syntax_tree_search_type_bits;

// Item found during a search
typedef struct arC_syntax_tree_search_item
{
	struct arC_syntax_tree_search_memory* memory;
	// The distance to where the item was found
	arInt32 distance;
	// The node we found
	struct arC_syntax_tree* node;
	// Linked list
	struct arC_syntax_tree_search_item* tail;
} arC_syntax_tree_search_item;

// The items found when doing the search query
typedef struct arC_syntax_tree_search_items
{
	struct arC_syntax_tree_search_memory* memory;
	// The first item
	arC_syntax_tree_search_item* first;
	// The last item
	arC_syntax_tree_search_item* last;
	// Number of items found
	arInt32 count;
	// Linked list
	struct arC_syntax_tree_search_items* tail;
} arC_syntax_tree_search_items;

// Memory management used by the search functionality
typedef struct arC_syntax_tree_search_memory
{
	arC_syntax_tree_search_item* item_head;
	arC_syntax_tree_search_items* items_head;
} arC_syntax_tree_search_memory;

// Initialize the memory
ARLANG_API void arC_syntax_tree_search_memory_init(arC_syntax_tree_search_memory* memory);

// Release the memory
ARLANG_API void arC_syntax_tree_search_memory_release(arC_syntax_tree_search_memory* memory);

// The search query is done backwards from the current node. Useful if we are searching for inner types
#define arC_SEARCH_FLAG_BACKWARDS (1 << 0)
// The search query should include imports when searching for hits
#define arC_SEARCH_FLAG_INCLUDE_IMPORTS (1 << 1)
// The search query should include child nodes if the node-type allows for it
#define arC_SEARCH_FLAG_INCLUDE_CHILDREN (1 << 2)
// The search query should include parent node
#define arC_SEARCH_FLAG_INCLUDE_PARENTS (1 << 3)

// Search for nodes using the supplied query. Returns a linked list containing all items found using the
// supplied search query. Will return NULL if computer is out of memory.
ARLANG_API arC_syntax_tree_search_items* arC_syntax_tree_search_visit(const struct arC_state* state,
	struct arC_syntax_tree* node, const arString* query, arC_syntax_tree_search_type_bits types, arInt32 flags);

// Search for a single node using the supplied query. Returns NULL if nothing is found
ARLANG_API struct arC_syntax_tree* arC_syntax_tree_search_once(const struct arC_state* state,
	struct arC_syntax_tree* node, const arString* query, arC_syntax_tree_search_type type, arInt32 flags);

// Get the first node based on a type
ARLANG_API struct arC_syntax_tree* arC_syntax_tree_search_get_first_type(
	const struct arC_syntax_tree_search_items* items, arInt32 type);

// Cleanup memory
ARLANG_API void arC_syntax_tree_search_visit_destroy(arC_syntax_tree_search_items* items);

#endif
