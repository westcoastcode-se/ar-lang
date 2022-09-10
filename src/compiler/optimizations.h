#ifndef _ARC_OPTIMIZATIONS_H_
#define _ARC_OPTIMIZATIONS_H_

#include "syntax_tree.h"

// Try to merge binary operators. This function is allowed to be executed when builing in debug.
extern arC_syntax_tree_node arC_synax_tree_merge_binop(const struct arC_state* s, arC_syntax_tree_node_binop* binop);

// Try to merge unary operators. This function is allowed to be executed when builing in debug.
extern arC_syntax_tree_node arC_synax_tree_merge_unaryop(const struct arC_state* s, arC_syntax_tree_node_unaryop* unaryop);

// Try to merge a tree node. This function is allowed to be executed when builing in debug.
extern arC_syntax_tree_node arC_synax_tree_merge_children(const struct arC_state* s, arC_syntax_tree_node node);

// Try to merge all tree nodes in the supplied root node
extern BOOL arC_synax_tree_optimize(const struct arC_state* s, arC_syntax_tree_node_package* root_node);

#endif
