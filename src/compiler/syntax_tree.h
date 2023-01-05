#ifndef _ARC_SYNTAX_TREE_H_
#define _ARC_SYNTAX_TREE_H_

#include "lexer.h"
#include "signature.h"
#include "../arPrimitiveValue.h"
#include "../arMemoryPool.h"
#include "../arBuilder.h"

// The type of syntax tree node this is
typedef enum arC_syntax_tree_type
{
	arC_SYNTAX_TREE_ERROR = 1,

	arC_SYNTAX_TREE_REF,
	arC_SYNTAX_TREE_REF_BLOCK,

	arC_SYNTAX_TREE_PACKAGE,
	arC_SYNTAX_TREE_IMPORT,
	arC_SYNTAX_TREE_TYPEDEF,
	arC_SYNTAX_TREE_TYPEREF,

	arC_SYNTAX_TREE_FUNCDEF,
	arC_SYNTAX_TREE_FUNCDEF_ARGS,
	arC_SYNTAX_TREE_FUNCDEF_ARG,
	arC_SYNTAX_TREE_FUNCDEF_RETS,
	arC_SYNTAX_TREE_FUNCDEF_RET,
	arC_SYNTAX_TREE_FUNCDEF_LOCALS,
	arC_SYNTAX_TREE_FUNCDEF_LOCAL,
	arC_SYNTAX_TREE_FUNCDEF_BODY,

	arC_SYNTAX_TREE_FUNCDEF_BODY_RETURN,
	arC_SYNTAX_TREE_FUNCDEF_BODY_ASSIGN,
	arC_SYNTAX_TREE_FUNCDEF_BODY_CONST_VALUE,
	arC_SYNTAX_TREE_FUNCDEF_BODY_VARREF,
	arC_SYNTAX_TREE_FUNCDEF_BODY_BINOP,
	arC_SYNTAX_TREE_FUNCDEF_BODY_UNARYOP,
	arC_SYNTAX_TREE_FUNCDEF_BODY_CALLFUNC,
} arC_syntax_tree_type;

// Bits used to keep track of which phase a syntax_tree node has completed
typedef enum arC_syntax_tree_phase_bits {
	arC_SYNTAX_TREE_PHASE_RESOLVE = (1 << 0),
	arC_SYNTAX_TREE_PHASE_COMPILE = (1 << 1)
} arC_syntax_tree_phase_bits;
typedef arInt32 arC_syntax_tree_phase;

// Base type for all syntax tree nodes
typedef struct arC_syntax_tree
{
	// The type of the tree node
	arC_syntax_tree_type type;	
	// Parent tree node
	struct arC_syntax_tree* parent;
	// Children
	struct arC_syntax_tree* child_head;
	struct arC_syntax_tree* child_tail;
	// The number of children
	arInt32 child_count;
	// Siblings
	struct arC_syntax_tree* head;
	struct arC_syntax_tree* tail;
	// Which phases this node is processed
	arC_syntax_tree_phase phase;
	// Version, used as a way to know if we are iterating over a tree while being updated
	// at the same time
	arInt32 version;
} arC_syntax_tree, * arC_syntax_tree_node;

#define asC_syntax_tree(st) (&st->header)
#define asC_syntax_tree_first_child(st) (st->header.child_head)
#define asC_syntax_tree_type(st) (st->header.type)
#define asC_syntax_tree_phase(st) (st->header.phase)
#define asC_syntax_tree_phase_set(st, p) (st->header.phase |= p)
#define asC_syntax_tree_phase_done(st, phase) ((asC_syntax_tree_phase(st) & phase) == phase)

// A reference to a syntax tree node that's not resolved yet. A ref contains a number of blocks
// where the last block contains the actual node we are searching for
typedef struct arC_syntax_tree_ref
{
	arC_syntax_tree header;
	// What node types are valid for when resolving the underlying type
	arInt32 valid_types;
	// Properties set during the resolve phase
	struct arC_syntax_tree_ref_resolved {
		struct arC_syntax_tree* node;
	} resolved;
} arC_syntax_tree_ref;

// A reference to a node from the previous block's point of view
typedef struct arC_syntax_tree_ref_block
{
	arC_syntax_tree header;
	// Search query used when searching in the syntax tree
	arString query;
	// What node types are valid for when resolving the underlying type
	arInt32 valid_types;
	// Properties set during the resolve phase
	struct arC_syntax_tree_ref_block_resolved {
		struct arC_syntax_tree* node;
	} resolved;
} arC_syntax_tree_ref_block;

// The package statement
typedef struct arC_syntax_tree_package
{
	arC_syntax_tree header;

	// The name of the package
	arString name;
	// Properties set during the compile phase
	struct arC_syntax_tree_package_resolve {
		// The full signature of the package
		arString signature;
	} resolve;
	// Properties set during the compile phase
	struct arC_syntax_tree_package_compiled {
		// The type symbol
		arB_package* symbol;
	} compiled;
} arC_syntax_tree_package;

// The import statement. Will contain child nodes which refer to the actual 
// package we are trying to import
typedef struct arC_syntax_tree_import
{
	arC_syntax_tree header;

	// Properties set during the resolve phase
	struct import_resolved {
		struct arC_syntax_tree_package* ref;
	} resolved;
} arC_syntax_tree_import;

// The type statement
typedef struct arC_syntax_tree_typedef
{
	arC_syntax_tree header;
	// Name of the type
	arString name;
	// Flags related to this definition
	arInt32 flags;
	// Inherits from? (for example, *int32 inheits from *void) 
	struct arC_syntax_tree_typeref* inherits_from;
	// Of what type? (For example, if *int32 or [2]int32 then this should point towards int32)
	struct arC_syntax_tree_typeref* of_type;
	// The package this block is part of
	struct arC_syntax_tree_package* package;
	// Properties set during the resolve phase
	struct arC_syntax_tree_typedef_resolved {
		// The virtual machine data type
		arInt8 data_type;
		// The underlying size that this object will take on the stack
		arInt32 stack_size;
		// Signature
		arString signature;
	} resolved;
	// Properties set during the compile phase
	struct arC_syntax_tree_typedef_compiled {
		// The type symbol
		arB_type* symbol;
	} compiled;
} arC_syntax_tree_typedef;

// A reference to a type statement. It will have a child node which refers to the actual type
typedef struct arC_syntax_tree_typeref
{
	arC_syntax_tree header;

	// Properties set during the resolve phase
	struct arC_syntax_tree_typeref_resolved {
		// The type definition, if found
		struct arC_syntax_tree_typedef* def;
	} resolved;
} arC_syntax_tree_typeref;

// The func statement
typedef struct arC_syntax_tree_funcdef
{
	arC_syntax_tree header;

	// The name of the function
	arString name;
	// The arguments
	struct arC_syntax_tree_funcdef_args* args;
	// The returns
	struct arC_syntax_tree_funcdef_rets* rets;
	// Locals
	struct arC_syntax_tree_funcdef_locals* locals;
	// The body
	struct arC_syntax_tree_funcdef_body* body;
	// Properties set during the compile phase
	struct arC_syntax_tree_funcdef_resolve {
		// The full signature of the function
		arString signature;
	} resolve;
	// Properties set during the compile phase
	struct arC_syntax_tree_funcdef_compiled {
		// The type symbol
		arB_func* symbol;
	} compiled;
} arC_syntax_tree_funcdef;

// The function arguments block
typedef struct arC_syntax_tree_funcdef_args
{
	arC_syntax_tree header;

	// Number of arguments
	arInt32 count;
} arC_syntax_tree_funcdef_args;

// The argument statement
typedef struct arC_syntax_tree_funcdef_arg
{
	arC_syntax_tree header;

	// Name of the argument
	arString name;
	// The function this argument is part of
	struct arC_syntax_tree_funcdef* func;
	// Reference to the type (is also a child of the argument)
	struct arC_syntax_tree_typeref* type;
	// Properties set during the compile phase
	struct funcdef_arg_compiled {
		// Argument
		arB_arg* symbol;
	} compiled;
} arC_syntax_tree_funcdef_arg;

// The function returns block
typedef struct arC_syntax_tree_funcdef_rets
{
	arC_syntax_tree header;

	// Number of returns
	arInt32 count;
} arC_syntax_tree_funcdef_rets;

// The return statement
typedef struct arC_syntax_tree_funcdef_ret
{
	arC_syntax_tree header;
	// The function this return is part of
	struct arC_syntax_tree_funcdef* func;
	// TODO: Add support for named returns?
	// Reference to the type (is also a child of the argument)
	struct arC_syntax_tree_typeref* type;
	// Properties set during the compile phase
	struct funcdef_ret_compiled {
		// Return
		arB_return* symbol;
	} compiled;
} arC_syntax_tree_funcdef_ret;

// The function locals block
typedef struct arC_syntax_tree_funcdef_locals
{
	arC_syntax_tree header;

	// Number of locals
	arInt32 count;
} arC_syntax_tree_funcdef_locals;

// A local
typedef struct arC_syntax_tree_funcdef_local
{
	arC_syntax_tree header;

	// Name of the local
	arString name;
	// The function this local is part of
	struct arC_syntax_tree_funcdef* func;
	// Reference to the type (is also a child of the locals)
	struct arC_syntax_tree_typeref* type;
	// Properties set during the resolve phase
	struct funcdef_local_resolved {
		// The type
		struct arC_syntax_tree_typedef* def;
	} resolved;
	// Properties set during the compile phase
	struct funcdef_local_compiled {
		// Local
		arB_local* symbol;
	} compiled;
} arC_syntax_tree_funcdef_local;

// The body for the function
typedef struct arC_syntax_tree_funcdef_body
{
	arC_syntax_tree header;
} arC_syntax_tree_funcdef_body;

// A return statement
typedef struct arC_syntax_tree_funcdef_body_return
{
	arC_syntax_tree header;

	// The closest function node
	struct arC_syntax_tree_funcdef* closest_function_node;
	// Reference to the type
	struct arC_syntax_tree_typeref* type;
} arC_syntax_tree_funcdef_body_return;

// A constant value
typedef struct arC_syntax_tree_funcdef_body_const_value
{
	arC_syntax_tree header;
	// The constant value to be pushed on the stack
	arPrimitiveValue value;
	// The closest function node
	struct arC_syntax_tree_funcdef* closest_function_node;
	// Reference to the type (is also a child of the locals)
	struct arC_syntax_tree_typeref* type;
} arC_syntax_tree_funcdef_body_const_value;

// A reference to a variable node
typedef struct arC_syntax_tree_funcdef_body_varref
{
	arC_syntax_tree header;
	// Properties set during the resolve phase
	struct arC_syntax_tree_funcdef_body_varref_resolved {
		// The type
		struct arC_syntax_tree* node;
	} resolved;
} arC_syntax_tree_funcdef_body_varref;

// Syntax tree for binary operators, such as + and -
typedef struct arC_syntax_tree_funcdef_body_binop
{
	arC_syntax_tree header;
	// Operator to use between the two nodes
	arC_token_type op;
	// The closest function node
	struct arC_syntax_tree_funcdef* closest_function_node;
} arC_syntax_tree_funcdef_body_binop;

// Syntax tree for unary operators, such as -
typedef struct arC_syntax_tree_funcdef_body_unaryop
{
	arC_syntax_tree header;
	// The operator
	arC_token_type op;
	// Closest function node
	arC_syntax_tree_funcdef* closest_function_node;
} arC_syntax_tree_funcdef_body_unaryop;

// Get the name of the syntax tree node
#define asC_syntax_tree_name(st) &st->name

// The state
typedef struct arC_state
{
	// The compiler
	struct arCompiler* compiler;
	// Memory pool
	arMemoryPool* memory_pool;
	// The token 
	arC_token* token;
	// The parent node
	arC_syntax_tree_node parent_node;
	// The closest parent package node
	arC_syntax_tree_package* package_node;
	// The closest parent function node
	arC_syntax_tree_funcdef* func_node;
	// The closest parent type node
	arC_syntax_tree_typedef* type_node;
} arC_state;

#define asC_state(state, parent_node, package_node, func_node, type_node) \
	{ state->compiler, state->memory_pool, state->token, parent_node, package_node, func_node, type_node }


// Get a string pool associated with the supplied state
arStringPool* arC_state_get_string_pool(const arC_state* s);

// Get a node that's considered an error. This is used in combination of with the messaging framework where
// the actual information on the error can be found
ARLANG_API arC_syntax_tree_node arC_syntax_tree_error();

// Is the supplied node an error node
ARLANG_API BOOL arC_syntax_tree_is_error(arC_syntax_tree_node st);

// Does the supplied node have a parent
ARLANG_API BOOL arC_syntax_tree_has_parent(arC_syntax_tree_node st);

// Does the supplied node have child-nodes
ARLANG_API BOOL arC_syntax_tree_has_children(arC_syntax_tree_node st);

// Add a new child node to the supplied node
ARLANG_API void arC_syntax_tree_add_child(arC_syntax_tree* parent, arC_syntax_tree_node child);

// Detach the supplied node from it's parent
ARLANG_API void arC_syntax_tree_detach(arC_syntax_tree_node node);

// Remove the old node and replace it with a new node at the same position
ARLANG_API void arC_syntax_tree_remove_replace(arC_syntax_tree* st, arC_syntax_tree_node old_node, arC_syntax_tree_node new_node);

// Create a new reference node
ARLANG_API arC_syntax_tree_ref* arC_syntax_tree_ref_new(const arC_state* s, arInt32 valid_types);

// Create a new reference node
ARLANG_API arC_syntax_tree_ref_block* arC_syntax_tree_ref_block_new(const arC_state* s);

// Create a new package tree node
ARLANG_API arC_syntax_tree_package* arC_syntax_tree_package_new(const arC_state* s);

// Is the parent package the root package?
ARLANG_API BOOL arC_syntax_tree_package_is_root(const arC_syntax_tree_package* p);

// Add a new import
ARLANG_API void arC_syntax_tree_package_add_import(arC_syntax_tree_package* package, arC_syntax_tree_import* import);

// Add a new type
ARLANG_API void arC_syntax_tree_package_add_typedef(arC_syntax_tree_package* package, arC_syntax_tree_typedef* type);

// Create a new import tree node
ARLANG_API arC_syntax_tree_import* arC_syntax_tree_import_new(const arC_state* s);

// Create a new type definition
ARLANG_API arC_syntax_tree_typedef* arC_syntax_tree_typedef_new(const arC_state* s);

// Create a new type reference
ARLANG_API arC_syntax_tree_typeref* arC_syntax_tree_typeref_new(const arC_state* s);

// Create a new type reference
ARLANG_API arC_syntax_tree_typeref* arC_syntax_tree_typeref_known(const arC_state* s, 
	const arString* name, arInt32 valid_types);

// Create a new function definition
ARLANG_API arC_syntax_tree_funcdef* arC_syntax_tree_funcdef_new(const arC_state* s);

// Create a new arguments definition
ARLANG_API arC_syntax_tree_funcdef_args* arC_syntax_tree_funcdef_args_new(const arC_state* s);

// Create a new arguments definition
ARLANG_API arC_syntax_tree_funcdef_arg* arC_syntax_tree_funcdef_arg_new(const arC_state* s);

// Create a new returns definition
ARLANG_API arC_syntax_tree_funcdef_rets* arC_syntax_tree_funcdef_rets_new(const arC_state* s);

// Create a new returns definition
ARLANG_API arC_syntax_tree_funcdef_ret* arC_syntax_tree_funcdef_ret_new(const arC_state* s);

// Create a new body definition
ARLANG_API arC_syntax_tree_funcdef_body* arC_syntax_tree_funcdef_body_new(const arC_state* s);

// Get the type which is pushed onto the stack after the supplied node is processed
ARLANG_API arC_syntax_tree_typedef* arC_syntax_tree_get_stack_type(arC_syntax_tree_node st);

// Create a new return statement for a body node
ARLANG_API arC_syntax_tree_funcdef_body_return* arC_syntax_tree_funcdef_body_return_new(const arC_state* s);

// Create a new constant statement for a body node
ARLANG_API arC_syntax_tree_funcdef_body_const_value* arC_syntax_tree_funcdef_body_const_value_new(const arC_state* s);

// Create a node which refers to a variable definition. Variable definitions might be:
// 1. A local variable
// 2. An argument
// 3. Global variable
// 3. Constant
ARLANG_API arC_syntax_tree_funcdef_body_varref* arC_syntax_tree_funcdef_body_varref_new(const arC_state* s);

// Create a new syntax tree node based on the supplied statement
ARLANG_API arC_syntax_tree_node arC_syntax_tree_funcdef_body_parse(const arC_state* s);

// Print out debug information to the console
ARLANG_API void arC_syntax_tree_stdout(const arC_syntax_tree* st);

#endif