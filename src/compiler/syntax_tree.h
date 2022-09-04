#ifndef _ARC_SYNTAX_TREE_H_
#define _ARC_SYNTAX_TREE_H_

#include "lexer.h"
#include "../arPrimitiveValue.h"

// Syntax tree types
typedef enum arC_syntax_tree_type
{
	AR_SYNTAX_TREE_ERROR = 1,
	AR_SYNTAX_TREE_ROOT,
	AR_SYNTAX_TREE_PACKAGE,
	AR_SYNTAX_TREE_IMPORT,
	AR_SYNTAX_TREE_TYPE,
	AR_SYNTAX_TREE_FUNC,
	AR_SYNTAX_TREE_RETURN,
	AR_SYNTAX_TREE_ASSIGN,
	AR_SYNTAX_TREE_CONST_VALUE,
	AR_SYNTAX_TREE_LOAD_LOCAL,
	AR_SYNTAX_TREE_LOAD_ARGUMENT,
	AR_SYNTAX_TREE_BINOP,
	AR_SYNTAX_TREE_UNARYOP
} arC_syntax_tree_type;

// Base class for a operation to be executed in a function
typedef struct arC_syntax_tree {
	arC_syntax_tree_type type;

	// The parent node
	struct arC_syntax_tree* parent;

	// Child nodes
	struct arC_syntax_tree* node;
	struct arC_syntax_tree* node_end;

	// Type of the data on the stack this syntax item results into
	struct arC_symbol* stack_type;

	// Intrusive Linked List (siblings)
	struct arC_syntax_tree* head;
	struct arC_syntax_tree* tail;

} arC_syntax_tree, *arC_syntax_tree_node;

// Get a node that's considered an error. This is used in combination of with the messaging framework where
// the actual information on the error can be found
ARLANG_API arC_syntax_tree_node arC_syntax_tree_error();

// Is the supplied node an error node
ARLANG_API BOOL arC_syntax_tree_is_error(arC_syntax_tree_node st);

// Does the supplied node have a parent
ARLANG_API BOOL arC_syntax_tree_has_parent(arC_syntax_tree_node st);

// Get the parent ST node
ARLANG_API arC_syntax_tree_node arC_syntax_tree_get_parent(arC_syntax_tree_node st);

// Add a new child node to the supplied node
ARLANG_API void arC_syntax_tree_add(arC_syntax_tree* st, arC_syntax_tree_node node);

// Remove a node from the syntax tree and replace it with a new one at the same location as the old one
ARLANG_API void arC_syntax_tree_remove_replace(arC_syntax_tree* st, arC_syntax_tree_node old_node, arC_syntax_tree_node new_node);

// Search for the closest ST parent of the supplied type
ARLANG_API arC_syntax_tree_node arC_syntax_tree_find_parent_type(arC_syntax_tree_node st, arC_syntax_tree_type type);

#define asC_syntax_tree(st) (&st->header)
#define asC_syntax_tree_stack_type(st) (asC_syntax_tree(st)->stack_type)

// A root tree node. This keep track of all package nodes and global
// functions, types and variables (such as int32)
typedef struct arC_syntax_tree_root
{
	arC_syntax_tree header;
} arC_syntax_tree_root;

// Package that contains child syntax trees
typedef struct arC_syntax_tree_node_package
{
	arC_syntax_tree header;

	// The actual, underlying, package for this syntax tree node
	struct arC_package* package;
} arC_syntax_tree_node_package;

// Import node
typedef struct arC_syntax_tree_node_import
{
	arC_syntax_tree header;

	// The actual, underlying, package for this syntax tree node
	struct arC_package* package;
} arC_syntax_tree_node_import;

// Type that represents things like data types, typedefs and structs and things like that
typedef struct arC_syntax_tree_node_type
{
	arC_syntax_tree header;

	// The actual, underlying, type for this node
	struct arC_type* type;

	// TODO: Add fields
} arC_syntax_tree_node_type;

// Function that contains
typedef struct arC_syntax_tree_node_func
{
	arC_syntax_tree header;

	// The actual, underlying, function for this syntax tree node
	struct arC_func* function;

	// Closest package node
	arC_syntax_tree_node_package* closest_package_node;
} arC_syntax_tree_node_func;

// A constant value
typedef struct arC_syntax_tree_node_const_value
{
	arC_syntax_tree header;
	// The constant value to be pushed on the stack
	arPrimitiveValue value;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_const_value;

// Syntax tree for the return statement
typedef struct arC_syntax_tree_node_return
{
	arC_syntax_tree header;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_return;

// Node for a = and := expression
typedef struct arC_syntax_tree_node_assign
{
	arC_syntax_tree header;
	// The target variable
	struct arC_symbol* target;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_assign;

// Node for loading a local variable
typedef struct arC_syntax_tree_node_load_local
{
	arC_syntax_tree header;
	// The target variable
	struct arC_local* target;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_load_local;

// Node for loading an argument
typedef struct arC_syntax_tree_node_load_arg
{
	arC_syntax_tree header;
	// The target variable
	struct arC_arg* target;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_load_arg;

// Syntax tree for binary operators, such as + and -
typedef struct arC_syntax_tree_node_binop
{
	arC_syntax_tree header;
	// Operator to use between the two nodes
	arC_token_type op;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_binop;

// Syntax tree for unary operators, such as -
typedef struct arC_syntax_tree_node_unaryop
{
	arC_syntax_tree header;
	// The operator
	arC_token_type op;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_unaryop;

typedef struct arC_state
{
	// The compiler
	struct arCompiler* compiler;
	// The token 
	arC_token* token;
	// The parent node
	arC_syntax_tree_node parent_node;
	// The closest parent package node
	arC_syntax_tree_node_package* package_node;
	// The closest parent function node
	arC_syntax_tree_node_func* func_node;
	// The closest parent type node
	arC_syntax_tree_node_type* type_node;
} arC_state;

// Create a new root node
ARLANG_API arC_syntax_tree_root* arC_syntax_tree_root_new();

// Add the supplied type as a global type
ARLANG_API void arC_syntax_tree_root_add(arC_syntax_tree_root* root, arC_syntax_tree_node node);

// Create a new package tree node
ARLANG_API arC_syntax_tree_node_package* arC_syntax_tree_node_package_new(struct arC_package* pkg);

// Add the supplied function node to the package
ARLANG_API void arC_syntax_tree_node_package_add_func(arC_syntax_tree_node_package* package, arC_syntax_tree_node_func* func);

// Add a new child node
ARLANG_API void arC_syntax_tree_node_package_add(arC_syntax_tree_node_package* package, arC_syntax_tree_node node);

// Create a new function
ARLANG_API arC_syntax_tree_node_func* arC_syntax_tree_node_func_new(struct arC_func* func);

// Compile the supplied syntax tree
ARLANG_API BOOL arC_syntax_tree_compile(struct arCompiler* c, arC_syntax_tree* st);

// Search for a symbol of a specific name
ARLANG_API struct arC_symbol* arC_syntax_tree_find_symbol(arC_syntax_tree* st, const arString* name);

// Search for a symbol of a specific name. Search recursively upwards until you reach the package tree node
ARLANG_API struct arC_symbol* arC_syntax_tree_find_symbol_incl_imports(arC_syntax_tree* st, const arString* name);

// Create a new syntax tree type
ARLANG_API arC_syntax_tree_node_type* arC_syntax_tree_node_type_new(struct arC_type* type);

// Destroy the supplied syntax tree
ARLANG_API void arC_syntax_tree_node_type_destroy(arC_syntax_tree* st);

// Parse function
typedef arC_syntax_tree_node (*arC_syntax_tree_parse_fn)(struct arCompiler*, arC_token*, const struct arC_state*);

// Parse an expression
ARLANG_API arC_syntax_tree_node arC_syntax_tree_parse_keywords(struct arCompiler* c, arC_token* t, const struct arC_state* s);

#endif
