#include "search.h"
#include "symbol.h"

void arC_search_children_begin(arC_syntax_tree_node parent, const arString* query, arInt32 types, arC_search_children_context* ctx)
{
	ctx->parent = parent;
	ctx->node = parent->node;
	ctx->query = *query;
	ctx->types = types;
	ctx->version = parent->version;
}

arC_syntax_tree_node arC_search_children_next(arC_search_children_context* ctx)
{
	assert(ctx->version == ctx->parent->version && 
		"You are iterating over the syntax tree node at the same time as you are updating it");

	const arInt32 types = ctx->types;
	const arString* query = &ctx->query;
	arC_syntax_tree_node node = ctx->node;
	while (node) {
		if ((node->type & types) != 0) {
			switch (node->type) {
			case arC_SYNTAX_TREE_TYPE:
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_type*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_FUNC:
				if (arString_cmp(asC_symbol_signature(((arC_syntax_tree_node_func*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_func*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				if (arString_cmp(&((arC_syntax_tree_node_func*)node)->symbol->name, query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_IMPORT:
				if (arString_cmp(&((arC_syntax_tree_node_import*)node)->alias, query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_PACKAGE:
				if (arString_cmp(asC_symbol_signature(((arC_syntax_tree_node_package*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_package*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			default:
				break;
			}
		}
		node = node->tail;
	}
	return NULL;
}

void arC_search_upwards_begin(arC_syntax_tree_node node, const arString* query, arInt32 types,
	arInt32 direction, arC_search_upwards_context* ctx)
{
	ctx->parent = node->parent;
	ctx->node = node;
	ctx->query = *query;
	ctx->types = types;
	ctx->version = node->parent;
	ctx->recursive = TRUE;
	ctx->direction = direction;
}

arC_syntax_tree_node arC_search_forwards(arC_search_upwards_context* ctx)
{
	const arInt32 types = ctx->types;
	const arString* query = &ctx->query;
	arC_syntax_tree_node node = ctx->node;

	// Search backwards
	while (node) {
		if ((node->type & types) != 0) {
			switch (node->type) {
			case arC_SYNTAX_TREE_TYPE:
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_type*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_FUNC:
				if (arString_cmp(asC_symbol_signature(((arC_syntax_tree_node_func*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_func*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				if (arString_cmp(&((arC_syntax_tree_node_func*)node)->symbol->name, query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_IMPORT:
				if (arString_cmp(&((arC_syntax_tree_node_import*)node)->alias, query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_PACKAGE:
				if (arString_cmp(asC_symbol_signature(((arC_syntax_tree_node_package*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_package*)node)->symbol), query)) {
					ctx->node = node->tail;
					return node;
				}
				break;
			default:
				break;
			}
		}
		node = node->head;
	}

	if (ctx->recursive) {
		// Search among parents
		if (node->parent != NULL) {
			ctx->parent = node->parent->parent;
			ctx->node = node->parent;
			ctx->version = node->parent->version;
			ctx->direction = 1;
			return arC_search_backwards_next(ctx);
		}
	}
	return NULL;
}

arC_syntax_tree_node arC_search_backwards(arC_search_upwards_context* ctx)
{
	const arInt32 types = ctx->types;
	const arString* query = &ctx->query;
	arC_syntax_tree_node node = ctx->node;

	// Search backwards
	while (node) {
		if ((node->type & types) != 0) {
			switch (node->type) {
			case arC_SYNTAX_TREE_TYPE:
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_type*)node)->symbol), query)) {
					ctx->node = node->head;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_FUNC:
				if (arString_cmp(asC_symbol_signature(((arC_syntax_tree_node_func*)node)->symbol), query)) {
					ctx->node = node->head;
					return node;
				}
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_func*)node)->symbol), query)) {
					ctx->node = node->head;
					return node;
				}
				if (arString_cmp(&((arC_syntax_tree_node_func*)node)->symbol->name, query)) {
					ctx->node = node->head;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_IMPORT:
				if (arString_cmp(&((arC_syntax_tree_node_import*)node)->alias, query)) {
					ctx->node = node->head;
					return node;
				}
				break;
			case arC_SYNTAX_TREE_PACKAGE:
				if (arString_cmp(asC_symbol_signature(((arC_syntax_tree_node_package*)node)->symbol), query)) {
					ctx->node = node->head;
					return node;
				}
				if (arString_cmp(asC_symbol_name(((arC_syntax_tree_node_package*)node)->symbol), query)) {
					ctx->node = node->head;
					return node;
				}
				break;
			default:
				break;
			}
		}
		node = node->head;
	}

	if (ctx->recursive) {
		// Search among parents
		if (node->parent != NULL) {
			ctx->parent = node->parent->parent;
			ctx->node = node->parent;
			ctx->version = node->parent->version;
			ctx->direction = 1;
			return arC_search_backwards_next(ctx);
		}
	}
	return NULL;
}

arC_syntax_tree_node arC_search_upwards_next(arC_search_upwards_context* ctx)
{
	assert(ctx->version == ctx->parent->version &&
		"You are iterating over the syntax tree node at the same time as you are updating it");

	if (ctx->direction == -1)
		return arC_search_backwards(ctx);
	else
		return arC_search_forwards(ctx);
}

arC_syntax_tree_node arC_search_upwards_once(arC_syntax_tree_node node, const arString* query,
	arInt32 types, arInt32 direction)
{
	arC_search_upwards_context search;
	arC_search_upwards_begin(node, query, types, direction, &search);
	return arC_search_upwards_next(&search);
}
