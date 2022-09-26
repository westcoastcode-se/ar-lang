#ifndef _ARC_SEARCH_H_
#define _ARC_SEARCH_H_

#include "syntax_tree.h"

// Context used when searching for syntax tree nodes among the children of a parent
typedef struct arC_search_children_context
{
	// The parent we are search from at the moment
	arC_syntax_tree_node parent;
	// The most recent node we've found
	arC_syntax_tree_node node;
	// Search query
	arString query;
	// A bitfield used to keep track of what we are searching for
	arInt32 types;
	// Version of the node we are iterating over
	arInt32 version;
} arC_search_children_context;

// Begin searching for tree nodes
ARLANG_API void arC_search_children_begin(arC_syntax_tree_node parent, const arString* query, arInt32 types,
	arC_search_children_context* ctx);

// Search for the next item
ARLANG_API arC_syntax_tree_node arC_search_children_next(arC_search_children_context* ctx);

// Context used when searching for tree nodes in the entire syntax tree structure
typedef struct arC_search_upwards_context
{
	// The parent we are search from at the moment
	arC_syntax_tree_node parent;
	// The most recent node we've found
	arC_syntax_tree_node node;
	// Search query
	arString query;
	// A bitfield used to keep track of what we are searching for
	arInt32 types;
	// Version of the node we are iterating over
	arInt32 version;
	// Search children and parents
	BOOL recursive;
	// Search direction (-1 = backwards, 1 = forwards)
	arInt32 direction;
} arC_search_upwards_context;

// Begin searching for tree nodes. Start at the supplied node and search upwards
ARLANG_API void arC_search_upwards_begin(arC_syntax_tree_node node, const arString* query, arInt32 types,
	arInt32 direction, arC_search_upwards_context* ctx);

// Search for the next item
ARLANG_API arC_syntax_tree_node arC_search_upwards_next(arC_search_upwards_context* ctx);

// Searching for tree nodes, once. Start at the supplied node and search upwards
ARLANG_API arC_syntax_tree_node arC_search_upwards_once(arC_syntax_tree_node node, const arString* query, 
	arInt32 types, arInt32 direction);

#endif
