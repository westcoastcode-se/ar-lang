#ifndef _VMCODE_SYNTAX_TREE_H_
#define _VMCODE_SYNTAX_TREE_H_

#include "vmcd_lexer.h"
#include "../vmp/vmp.h"

// All operations that's allowed by the compiler
typedef enum vmcd_syntax_tree_type {
	VMCD_STT_CONST_VALUE,
	VMCD_STT_BINOP,
	VMCD_STT_UNARYOP,
	VMCD_STT_RETURN,
} vmcd_syntax_tree_type;

// Base class for a operation to be executed in a function
typedef struct vmcd_syntax_tree {
	vmcd_syntax_tree_type type;

	// Type of the data on the stack this syntax item results into
	struct vmcd_keyword* stack_type;

	// Intrusive Linked List
	struct vmcd_syntax_tree* head;
	struct vmcd_syntax_tree* tail;
} vmcd_syntax_tree, *vmcd_syntax_tree_node;

#define VMCD_SYNTAX_TREE(st) (&st->header)
#define VMCD_SYNTAX_TREE_STACK_TYPE(st) (VMCD_SYNTAX_TREE(st)->stack_type)

typedef struct vmcd_syntax_tree_const_value {
	vmcd_syntax_tree header;

	// The constant value to be pushed on the stack
	vmp_constant value;
} vmcd_syntax_tree_const_value;

// A binary operation, such as +, -, / or *
typedef struct vmcd_syntax_tree_binop {
	vmcd_syntax_tree header;

	// Left side of the constant value
	vmcd_syntax_tree_node left;

	// The operator to be executed
	vmcd_token_type op;

	// THe right side of the operator
	vmcd_syntax_tree_node right;
} vmcd_syntax_tree_binop;

// A unary operator, used to mark a value as negative or positive
typedef struct vmcd_syntax_tree_unaryop {
	vmcd_syntax_tree header;

	// The operator (+|-)
	vmcd_token_type op;

	// The node to apply the operator on
	vmcd_syntax_tree_node node;
} vmcd_syntax_tree_unaryop;

// A return statement
typedef struct vmcd_syntax_tree_return
{
	vmcd_syntax_tree header;
	// The expression to be returned
	vmcd_syntax_tree_node expression;
} vmcd_syntax_tree_return;

extern vmcd_syntax_tree_node vmcd_syntax_tree_next_term(struct vmcd_scope* s);

extern vmcd_syntax_tree_node vmcd_syntax_tree_next_factor(struct vmcd_scope* s);

extern vmcd_syntax_tree_node vmcd_syntax_tree_next_expr(struct vmcd_scope* s);

// Parse the next item and return a syntax tree node. This function will push an error message onto the parser if 
// it fails for some reason. Otherwise a node is returned. The node can be returned and be added at the end of the syntax tree list
// in the function
extern vmcd_syntax_tree_node vmcd_syntax_tree_next(struct vmcd_scope* s);

// Destroy the supplied syntax tree
extern void vmcd_syntax_tree_free(vmcd_syntax_tree* st);

// Compile the syntax tree and convert it into instructions underst
extern BOOL vmcd_syntax_tree_compile(vmcd_syntax_tree* st, struct vmcd_func* f);


#endif
