#ifndef _ZPP_SYNTAX_TREE_H_
#define _ZPP_SYNTAX_TREE_H_

#include "lexer.h"
#include "../vmp/vmp.h"

// Syntax tree types
typedef enum zpp_syntax_tree_object_type
{
	ZPP_SYNTAX_TREE_ERROR = 1,
	ZPP_SYNTAX_TREE_ROOT,
	ZPP_SYNTAX_TREE_PACKAGE,
	ZPP_SYNTAX_TREE_IMPORT,
	ZPP_SYNTAX_TREE_TYPE,
	ZPP_SYNTAX_TREE_FUNC,
	ZPP_SYNTAX_TREE_RETURN,
	ZPP_SYNTAX_TREE_ASSIGN,
	ZPP_SYNTAX_TREE_CONST_VALUE,
	ZPP_SYNTAX_TREE_LOAD_LOCAL,
	ZPP_SYNTAX_TREE_LOAD_ARGUMENT,
	ZPP_SYNTAX_TREE_BINOP,
	ZPP_SYNTAX_TREE_UNARYOP
} zpp_syntax_tree_object_type;

// Base class for a operation to be executed in a function
typedef struct zpp_syntax_tree {
	zpp_syntax_tree_object_type type;

	// The parent node
	struct zpp_syntax_tree* parent;

	// Child nodes
	struct zpp_syntax_tree* node;
	struct zpp_syntax_tree* node_end;

	// Type of the data on the stack this syntax item results into
	struct zpp_symbol* stack_type;

	// Intrusive Linked List (siblings)
	struct zpp_syntax_tree* head;
	struct zpp_syntax_tree* tail;

} zpp_syntax_tree, *zpp_syntax_tree_node;

// Get a node that's considered an error. This is used in combination of with the messaging framework where
// the actual information on the error can be found
extern zpp_syntax_tree_node zpp_syntax_tree_error();

// Is the supplied node an error node
extern BOOL zpp_syntax_tree_is_error(zpp_syntax_tree_node st);

// Does the supplied node have a parent
extern BOOL zpp_syntax_tree_has_parent(zpp_syntax_tree_node st);

// Get the parent ST node
extern zpp_syntax_tree_node zpp_syntax_tree_get_parent(zpp_syntax_tree_node st);

// Add a new child node to the supplied node
extern void zpp_syntax_tree_add(zpp_syntax_tree* st, zpp_syntax_tree_node node);

// Remove a node from the syntax tree and replace it with a new one at the same location as the old one
extern void zpp_syntax_tree_remove_replace(zpp_syntax_tree* st, zpp_syntax_tree_node old_node, zpp_syntax_tree_node new_node);

// Search for the closest ST parent of the supplied type
extern zpp_syntax_tree_node zpp_syntax_tree_find_parent_of_type(zpp_syntax_tree_node st, zpp_syntax_tree_object_type type);

// Initialize the tree node
extern void zpp_syntax_tree_init(zpp_syntax_tree* st, zpp_syntax_tree_object_type type);

#define ZPP_SYNTAX_TREE(st) (&st->header)
#define ZPP_SYNTAX_TREE_STACK_TYPE(st) (ZPP_SYNTAX_TREE(st)->stack_type)

// A root tree node. This keep track of all package nodes and global
// functions, types and variables (such as int32)
typedef struct zpp_syntax_tree_root
{
	zpp_syntax_tree header;
} zpp_syntax_tree_root;

// Package that contains child syntax trees
typedef struct zpp_syntax_tree_package
{
	zpp_syntax_tree header;

	// The actual, underlying, package for this syntax tree node
	struct zpp_package* package;
} zpp_syntax_tree_package;

// Import node
typedef struct zpp_syntax_tree_import
{
	zpp_syntax_tree header;

	// The actual, underlying, package for this syntax tree node
	struct zpp_package* package;
} zpp_syntax_tree_import;

// Type that represents things like data types, typedefs and structs and things like that
typedef struct zpp_syntax_tree_type
{
	zpp_syntax_tree header;

	// The actual, underlying, type for this node
	struct zpp_type* type;

	// TODO: Add fields
} zpp_syntax_tree_type;

// Function that contains
typedef struct zpp_syntax_tree_func
{
	zpp_syntax_tree header;

	// The actual, underlying, function for this syntax tree node
	struct zpp_func* function;

	// Closest package node
	zpp_syntax_tree_package* closest_package_node;
} zpp_syntax_tree_func;

// A constant value
typedef struct zpp_syntax_tree_const_value
{
	zpp_syntax_tree header;
	// The constant value to be pushed on the stack
	vmp_constant value;
	// Closest function node
	zpp_syntax_tree_func* closest_function_node;
} zpp_syntax_tree_const_value;

// Syntax tree for the return statement
typedef struct zpp_syntax_tree_return
{
	zpp_syntax_tree header;
	// Closest function node
	zpp_syntax_tree_func* closest_function_node;
} zpp_syntax_tree_return;

// Node for a = and := expression
typedef struct zpp_syntax_tree_assign
{
	zpp_syntax_tree header;
	// The target variable
	struct zpp_symbol* target;
	// Closest function node
	zpp_syntax_tree_func* closest_function_node;
} zpp_syntax_tree_assign, zpp_syntax_tree_assign_local;

// Node for loading a local variable
typedef struct zpp_syntax_tree_load_local
{
	zpp_syntax_tree header;
	// The target variable
	struct zpp_local* target;
	// Closest function node
	zpp_syntax_tree_func* closest_function_node;
} zpp_syntax_tree_load_local;

// Node for loading an argument
typedef struct zpp_syntax_tree_load_argument
{
	zpp_syntax_tree header;
	// The target variable
	struct zpp_argument* target;
	// Closest function node
	zpp_syntax_tree_func* closest_function_node;
} zpp_syntax_tree_load_argument;

// Syntax tree for binary operators, such as + and -
typedef struct zpp_syntax_tree_binop
{
	zpp_syntax_tree header;
	// Operator to use between the two nodes
	zpp_token_type op;
	// Closest function node
	zpp_syntax_tree_func* closest_function_node;
} zpp_syntax_tree_binop;

// Syntax tree for unary operators, such as -
typedef struct zpp_syntax_tree_unaryop
{
	zpp_syntax_tree header;
	// The operator
	zpp_token_type op;
	// Closest function node
	zpp_syntax_tree_func* closest_function_node;
} zpp_syntax_tree_unaryop;

// Destroy the supplied syntax tree, including all child nodes
extern void zpp_syntax_tree_free(zpp_syntax_tree* st);

// Create a new root node
extern zpp_syntax_tree_root* zpp_syntax_tree_root_new();

// Add the supplied type as a global type
extern void zpp_syntax_tree_root_add(zpp_syntax_tree_root* root, zpp_syntax_tree_node node);

// Create a new package tree node
extern zpp_syntax_tree_package* zpp_syntax_tree_package_new(struct zpp_package* pkg);

// Add the supplied function node to the package
extern void zpp_syntax_tree_package_add_func(zpp_syntax_tree_package* package, zpp_syntax_tree_func* func);

// Add a new child node
extern void zpp_syntax_tree_package_add(zpp_syntax_tree_package* package, zpp_syntax_tree_node node);

// Create a new function
extern zpp_syntax_tree_func* zpp_syntax_tree_func_new(struct zpp_func* func);

// Compile the supplied syntax tree
extern BOOL zpp_synax_tree_compile(struct zpp_compiler* c, zpp_syntax_tree* st);

// Search for a symbol of a specific name
extern struct zpp_symbol* zpp_syntax_tree_find_symbol(zpp_syntax_tree* st, const vm_string* name);

// Search for a symbol of a specific name. Search recursively upwards until you reach the package tree node
extern struct zpp_symbol* zpp_syntax_tree_find_symbol_include_imports(zpp_syntax_tree* st, const vm_string* name);

// Create a new syntax tree type
extern zpp_syntax_tree_type* zpp_syntax_tree_type_new(struct zpp_type* type);

// Destroy the supplied syntax tree
extern void zpp_syntax_tree_destroy(zpp_syntax_tree* st);

// Parse function
typedef zpp_syntax_tree_node (*zpp_synax_tree_parse_fn)(struct zpp_compiler*, zpp_token*, const struct zpp_compiler_state*);

// Parse an expression
extern zpp_syntax_tree_node zpp_synax_tree_parse_keywords(struct zpp_compiler* c, zpp_token* t, const struct zpp_compiler_state* s);

#endif
