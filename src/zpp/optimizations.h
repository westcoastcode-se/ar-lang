#ifndef _ZPP_OPTIMIZATIONS_H_
#define _ZPP_OPTIMIZATIONS_H_

#include "syntax_tree.h"

// Try to merge binary operators. This function is allowed to be executed when builing in debug.
extern zpp_syntax_tree_node zpp_synax_tree_merge_binop(const struct zpp_compiler_state* s, zpp_syntax_tree_binop* binop);

// Try to merge unary operators. This function is allowed to be executed when builing in debug.
extern zpp_syntax_tree_node zpp_synax_tree_merge_unaryop(const struct zpp_compiler_state* s, zpp_syntax_tree_unaryop* unaryop);

// Try to merge a tree node. This function is allowed to be executed when builing in debug.
extern zpp_syntax_tree_node zpp_synax_tree_merge_children(const struct zpp_compiler_state* s, zpp_syntax_tree_node node);

// Try to merge all tree nodes in the supplied root node
extern BOOL zpp_synax_tree_optimize(const struct zpp_compiler_state* s, zpp_syntax_tree_root* root_node);

#endif
