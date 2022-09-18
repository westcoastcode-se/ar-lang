#ifndef _ARC_SYNTAX_TREE_H_
#define _ARC_SYNTAX_TREE_H_

#include "lexer.h"
#include "signatures.h"
#include "../arPrimitiveValue.h"

// Syntax tree types
typedef enum arC_syntax_tree_type
{
	arC_SYNTAX_TREE_ERROR = 1,
	arC_SYNTAX_TREE_PACKAGE,
	arC_SYNTAX_TREE_IMPORT,
	arC_SYNTAX_TREE_TYPE,
	arC_SYNTAX_TREE_FUNC,
	arC_SYNTAX_TREE_RETURN,
	arC_SYNTAX_TREE_ASSIGN,
	arC_SYNTAX_TREE_CONST_VALUE,
	arC_SYNTAX_TREE_LOAD_LOCAL,
	arC_SYNTAX_TREE_LOAD_ARGUMENT,
	arC_SYNTAX_TREE_CALLFUNC,
	arC_SYNTAX_TREE_BINOP,
	arC_SYNTAX_TREE_UNARYOP
} arC_syntax_tree_type;

// Base class for a operation to be executed in a function
typedef struct arC_syntax_tree {
	arC_syntax_tree_type type;

	// The parent node
	struct arC_syntax_tree* parent;

	// Child nodes
	struct arC_syntax_tree* node;
	struct arC_syntax_tree* node_end;

	// Type of the data. It can be either arC_type or arC_proxy
	struct arC_type* stack_type;

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

// Does the supplied node have child-nodes
ARLANG_API BOOL arC_syntax_tree_has_children(arC_syntax_tree_node st);

// Get the parent ST node
ARLANG_API arC_syntax_tree_node arC_syntax_tree_get_parent(arC_syntax_tree_node st);

// Add a new child node to the supplied node
ARLANG_API void arC_syntax_tree_add(arC_syntax_tree* st, arC_syntax_tree_node node);

// Remove a node from the syntax tree and replace it with a new one at the same location as the old one
ARLANG_API void arC_syntax_tree_remove_replace(arC_syntax_tree* st, arC_syntax_tree_node old_node, arC_syntax_tree_node new_node);

// Search for the closest ST parent of one of the supplied types
ARLANG_API arC_syntax_tree_node arC_syntax_tree_find_incl_parents(arC_syntax_tree_node st, arInt32 types);

#define asC_syntax_tree(st) (&st->header)
#define asC_syntax_tree_stack_type(st) (asC_syntax_tree(st)->stack_type)

// Package that contains child syntax trees
typedef struct arC_syntax_tree_node_package
{
	arC_syntax_tree header;
	// The actual, underlying, package for this syntax tree node
	struct arC_package* symbol;
} arC_syntax_tree_node_package;

// Import node
typedef struct arC_syntax_tree_node_import
{
	arC_syntax_tree header;
	// Alias
	arString alias;
	// The package we are importing
	arC_syntax_tree_node_package* package;
} arC_syntax_tree_node_import;

// Type that represents things like data types, typedefs and structs and things like that
typedef struct arC_syntax_tree_node_type
{
	arC_syntax_tree header;
	// The actual, underlying, type for this node
	struct arC_type* symbol;
	// TODO: Add fields
} arC_syntax_tree_node_type;

// Function that contains
typedef struct arC_syntax_tree_node_func
{
	arC_syntax_tree header;
	// The actual, underlying, function for this syntax tree node
	struct arC_func* symbol;
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

// Call another function
typedef struct arC_syntax_tree_node_callfunc
{
	arC_syntax_tree header;
	// Function to call
	struct arC_func* func;
	// Closest function node
	arC_syntax_tree_node_func* closest_function_node;
} arC_syntax_tree_node_callfunc;

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

#define asC_state(state, parent_node, package_node, func_node, type_node) \
	{ state->compiler, state->token, parent_node, package_node, func_node, type_node }

// Get the string pool associated with the supplied state
ARLANG_API struct arStringPool* arC_state_get_string_pool(const arC_state* s);

// Create a new package tree node
ARLANG_API arC_syntax_tree_node_package* arC_syntax_tree_node_package_new(const arC_signature_package* signature);

// Create a new function
ARLANG_API arC_syntax_tree_node_func* arC_syntax_tree_node_func_new(const arC_signature_func* signature);

// The package we are importing
ARLANG_API arC_syntax_tree_node_import* arC_syntax_tree_node_import_new(arC_syntax_tree_node_package* package);

// Create a new import statement for the root package and put it into the supplied package
ARLANG_API BOOL arC_syntax_tree_node_import_root(arC_syntax_tree_node_package* package, const arC_state* s);

// Compile the supplied syntax tree
ARLANG_API BOOL arC_syntax_tree_compile(struct arCompiler* c, arC_syntax_tree* st);

// Search for a node of a specific type. The query will search upwards to all parents until including the first level of all imports + root
ARLANG_API arC_syntax_tree_node arC_syntax_tree_find_incl_imports(arC_syntax_tree_node node, const arString* name, arInt32 types);

// Search for a tree node of the supplied name and type
ARLANG_API arC_syntax_tree_node arC_syntax_tree_find_child_with_type(arC_syntax_tree_node node, const arString* name, arInt32 types);

// Create a new syntax tree type
ARLANG_API arC_syntax_tree_node_type* arC_syntax_tree_node_type_new(const arC_signature_type* signature);

// Create a new syntax tree type
ARLANG_API void arC_syntax_tree_node_type_set_symbol(arC_syntax_tree_node_type* node, struct arC_type* type);

// Parse a new type from the currently working source code. This will create an unresolved/pending type if it isn't found
ARLANG_API arC_syntax_tree_node_type* arC_syntax_tree_node_type_parse(arC_token* t, const arC_state* s);

// Destroy the supplied syntax tree
ARLANG_API void arC_syntax_tree_node_destroy(arC_syntax_tree* st);

// Resolve syntax tree node types and initialize the necessary builder objects and symbols
ARLANG_API BOOL arC_syntax_tree_node_resolve(arC_syntax_tree_node node, const struct arC_state* s);

// Parse function
typedef arC_syntax_tree_node (*arC_syntax_tree_parse_fn)(struct arCompiler*, arC_token*, const struct arC_state*);

// Parse an expression
ARLANG_API arC_syntax_tree_node arC_syntax_tree_parse_keywords(struct arCompiler* c, arC_token* t, const struct arC_state* s);

#endif
