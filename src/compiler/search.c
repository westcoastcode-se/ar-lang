#include "search.h"

arC_syntax_tree_node arC_search_test(arC_syntax_tree_node node, arInt32 types, const arString* query)
{
	if ((BIT(node->type) & types) == 0) {
		return NULL;
	}

	switch (node->type) {
	case arC_SYNTAX_TREE_TYPEDEF:
		if (arString_cmp(&((arC_syntax_tree_typedef*)node)->name, query)) {
			return node;
		}
		break;
	case arC_SYNTAX_TREE_PACKAGE:
		if (arString_cmp(&((arC_syntax_tree_package*)node)->signature.name, query)) {
			return node;
		}
		break;
	case arC_SYNTAX_TREE_FUNCDEF:
		if (arString_cmp(&((arC_syntax_tree_funcdef*)node)->signature.name, query)) {
			return node;
		}
		break;
	}

	return NULL;
}

arC_syntax_tree_node arC_search_forwards_import_get_package(arC_syntax_tree_import* node)
{
	if (node->resolved.ref == NULL)
		return NULL;
	return asC_syntax_tree(node->resolved.ref);
}

arC_syntax_tree_node arC_search_forwards(arC_search_upwards_context* ctx)
{
	const arInt32 types = ctx->types;
	const arString* query = &ctx->query;
	arC_syntax_tree_node node = ctx->node;
	while (node) {
		arC_syntax_tree_node hit = arC_search_test(node, ctx->types, &ctx->query);
		if (hit != NULL) {
			ctx->node = hit->tail;
			return hit;
		}
		// Are we including imports in our search?
		if (node->type == arC_SYNTAX_TREE_IMPORT) {
			if (BIT_ISSET(ctx->flags, arC_SEARCH_FLAG_INCLUDE_IMPORTS)) {
				arC_search_upwards_context inner_ctx = *ctx;
				inner_ctx.flags &= ~arC_SEARCH_FLAG_INCLUDE_IMPORTS;
				inner_ctx.node = arC_search_forwards_import_get_package((arC_syntax_tree_import*)node);
				if (inner_ctx.node != NULL) {
					// Search from the first child in the package instead of the package itself
					inner_ctx.node = inner_ctx.node->child_head;
					inner_ctx.parent = node->parent;
					inner_ctx.recursive = FALSE;
					hit = arC_search_forwards(&inner_ctx);
					if (hit != NULL) {
						ctx->node = node->tail;
						return hit;
					}
				}
			}
		}
		node = node->tail;
	}

	node = ctx->node;
	if (ctx->recursive && node != NULL) {
		// Search among parents
		if (node->parent != NULL) {
			ctx->parent = node->parent->parent;
			ctx->node = ctx->parent->child_head; // Search from the first child in the parent
			ctx->version = node->parent->version;
			return arC_search_upwards_next(ctx);
		}
	}
	return NULL;
}

arC_syntax_tree_node arC_search_backwards(arC_search_upwards_context* ctx)
{
	const arInt32 types = ctx->types;
	const arString* query = &ctx->query;
	arC_syntax_tree_node node = ctx->node;
	while (node) {
		arC_syntax_tree_node hit = arC_search_test(node, ctx->types, &ctx->query);
		if (hit != NULL) {
			ctx->node = hit->head;
			return hit;
		}
		// Are we including imports in our search?
		if (node->type == arC_SYNTAX_TREE_IMPORT) {
			if (BIT_ISSET(ctx->flags, arC_SEARCH_FLAG_INCLUDE_IMPORTS)) {
				arC_search_upwards_context inner_ctx = *ctx;
				inner_ctx.flags &= ~arC_SEARCH_FLAG_INCLUDE_IMPORTS;
				inner_ctx.node = arC_search_forwards_import_get_package((arC_syntax_tree_import*)node);
				inner_ctx.parent = node->parent;
				inner_ctx.recursive = FALSE;
				hit = arC_search_forwards(&inner_ctx);
				if (hit != NULL) {
					ctx->node = node->head;
					return hit;
				}
			}
		}
		node = node->head;
	}

	node = ctx->node;
	if (ctx->recursive && node != NULL) {
		// Search among parents
		if (node->parent != NULL) {
			ctx->parent = node->parent->parent;
			ctx->node = ctx->parent->child_head; // Search from the first child in the parent
			ctx->version = node->parent->version;
			ctx->flags &= ~arC_SEARCH_FLAG_BACKWARDS;
			return arC_search_upwards_next(ctx);
		}
	}
	return NULL;
}

void arC_search_children_begin(arC_syntax_tree_node parent, const arString* query, arInt32 types, arC_search_children_context* ctx)
{
	ctx->parent = parent;
	ctx->node = parent->child_head;
	ctx->query = *query;
	ctx->types = types;
	ctx->version = parent->version;
}

arC_syntax_tree_node arC_search_children_next(arC_search_children_context* ctx)
{
	const arInt32 types = ctx->types;
	const arString* query = &ctx->query;
	arC_syntax_tree_node node = ctx->node;
	while (node) {
		arC_syntax_tree_node hit = arC_search_test(node, ctx->types, &ctx->query);
		if (hit != NULL) {
			ctx->node = hit;
			return node;
		}
		node = node->tail;
	}
	return NULL;
}

arC_syntax_tree_node arC_search_upwards_next(arC_search_upwards_context* ctx)
{
	if (BIT_ISSET(ctx->flags, arC_SEARCH_FLAG_BACKWARDS))
		return arC_search_backwards(ctx);
	else
		return arC_search_forwards(ctx);
}

arC_syntax_tree_node arC_search_children_once(arC_syntax_tree_node node, const arString* query, arInt32 types)
{
	arC_search_children_context search;
	arC_search_children_begin(node, query, types, &search);
	return arC_search_children_next(&search);
}

void arC_search_upwards_begin(arC_syntax_tree_node node, const arString* query, arInt32 types,
	arInt32 flags, arC_search_upwards_context* ctx)
{
	ctx->parent = node->parent;
	ctx->node = node;
	ctx->query = *query;
	ctx->types = types;
	ctx->version = node->parent->version;
	ctx->recursive = TRUE;
	ctx->flags = flags;
}

arC_syntax_tree_node arC_search_upwards_once(arC_syntax_tree_node node, const arString* query, arInt32 types, arInt32 flags)
{
	arC_search_upwards_context search;
	arC_search_upwards_begin(node, query, types, flags, &search);
	return arC_search_upwards_next(&search);
}
