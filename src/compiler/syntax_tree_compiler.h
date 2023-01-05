#ifndef _ARC_SYNTAX_TREE_COMPILER_H_
#define _ARC_SYNTAX_TREE_COMPILER_H_

#include "syntax_tree.h"

// Compile the supplied syntax tree
ARLANG_API BOOL arC_syntax_tree_compile(const arC_state* s, arC_syntax_tree* st);

#endif