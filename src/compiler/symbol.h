#ifndef _ARC_SYMBOLS_H_
#define _ARC_SYMBOLS_H_

#include "syntax_tree.h"
#include "../arBuilder.h"

// Describes what type of symbol something is
typedef enum arC_symbol_type
{
	arC_SYMBOL_PACKAGE,
	arC_SYMBOL_FUNC,
	arC_SYMBOL_CONST,
	arC_SYMBOL_TYPE,
	arC_SYMBOL_GLOBAL,
	arC_SYMBOL_LOCAL,
	arC_SYMBOL_ARGUMENT,
	arC_SYMBOL_RETURN
} arC_symbol_type;

// Represents a symbol, such as a package or a function
typedef struct arC_symbol
{
	// Type of symbol
	arC_symbol_type type;
	// The name of this symbol
	arString name;
	// The unique "full name"/signature of this symbol. A full signature is application unique - no
	// two symbols are allowed to have the same signature
	arString signature;
} arC_symbol;

// Get the base class for a specific symbol
#define asC_symbol(s) (&s->header)
// Get the name of the supplied symbol
#define asC_symbol_name(s) (&(s->header.name))
// Get the signature of the supplied symbol
#define asC_symbol_signature(s) (&(s->header.signature))

// Check to see if the supplied symbol has the name
ARLANG_API BOOL arC_symbol_has_name(arC_symbol* s, const arString* name);

// Check to see if the supplied symbol has the signature
ARLANG_API BOOL arC_symbol_has_signature(arC_symbol* s, const arString* signature);

// The package
typedef struct arC_package
{
	arC_symbol header;

	// Parent package
	struct arC_package* parent;

	// Child packages
	struct arC_package* children;
	struct arC_package* children_end;
	struct arC_package* children_head;
	struct arC_package* children_tail;

	// Types part of this package
	struct arC_type* types;
	struct arC_type* types_end;

	// Functions part of this package
	struct arC_func* funcs;
	struct arC_func* funcs_end;

	arB_package* package;
} arC_package;

// Flags that helps describe a specific type
typedef enum arC_type_flag
{
	arC_TYPE_FLAG_PTR = 1 << 0,
	arC_TYPE_FLAG_ARRAY = 1 << 1,
} arC_type_flag;

DECLARE_LIST_TYPE(arC_inherits_from, arC_type);
DECLARE_LIST_TYPE_FIND(arC_inherits_from, arC_type);

DECLARE_LIST_TYPE(arC_inherited_by, arC_type);
DECLARE_LIST_TYPE_FIND(arC_inherited_by, arC_type);

// Represents a type
typedef struct arC_type
{
	arC_symbol header;
	
	// The package this type is part of
	arC_package* package;

	// The size of this type
	arInt32 size;

	// Flags
	arBits32 flags;

	// What type of underlying datatype is this considered to be
	arUint8 data_type;

	// If this type is of another type. Normally used for pointer- and array types
	struct arC_type* of_type;

	// What this type is inherited from
	arC_inherits_from inherits_from;

	// What types this type is inherited by
	arC_inherited_by inherited_by;

	// Intrusive linked list
	struct arC_type* head;
	struct arC_type* tail;

	// The type, cached, so that we can much faster reuse it later on. This is realized after the parsing is done
	// and the actual compilation has started
	arB_type* type;
} arC_type;

// Structure used to help creating a type
typedef struct arC_type_props
{
	arString name;
	arUint32 size;
	arUint32 flags;
	arUint8 data_type;
	arC_type* of_type;
} arC_type_props;

// An argument
typedef struct arC_arg
{
	arC_symbol header;

	// The expected argument type
	arC_type* type;

	// Intrusive linked list
	struct arC_arg* head;
	struct arC_arg* tail;

	// The argument. Freeing memory of this is done by the pipeline and not this type
	arB_arg* arg;
} arC_arg;

// A local variable
typedef struct arC_local
{
	arC_symbol header;

	// The expected argument type
	arC_type* type;

	// Intrusive linked list
	struct arC_local* head;
	struct arC_local* tail;

	// The local. Freeing memory of this is done by the pipeline and not this type
	arB_local* local;
} arC_local;

// A return value
typedef struct arC_return
{
	arC_symbol header;
	
	// The return type returned
	arC_type* type;

	// Intrusive linked list
	struct arC_return* head;
	struct arC_return* tail;

	// The return value. Freeing memory of this is done by the pipeline and not this type
	arB_return* ret;
} arC_return;

typedef enum arC_func_flag
{
	// This function is public and can be accessed outside the package
	arC_FUNC_FLAG_PUBLIC = (1 << 0),

	// This function is marked as an external function. This means that the function
	// has to be linked from a c source code
	arC_FUNC_FLAG_EXTERN = (1 << 1),

	// This function is a lambda function
	arC_FUNC_FLAG_LAMBDA = (1 << 2),

	// The function is generated by the compiler. This type of function is
	// not visible outside the compiler
	arC_FUNC_FLAG_GENERATED = (1 << 3),
} arC_func_flag;

// A functions signature that represents a local signature
typedef struct arC_func_sign
{
	// The complete signature in string format. The signature is application unique
	arString signature;

	// The signature in a short version. This signature does not contain the
	// package name
	arString short_signature;

	// The name of the function
	arString name;

	// Package this function is part of
	arC_package* package;

	// All arguments required by this function
	arC_arg* arguments;
	arC_arg* arguments_end;
	arInt32 arguments_count;

	// All return values returned by this function
	arC_return* returns;
	arC_return* returns_end;
	arInt32 returns_count;
} arC_func_sign;

// The function
typedef struct arC_func
{
	arC_symbol header;

	// Signature information
	arC_func_sign signature;

	// All local variables
	arC_local* locals;
	arC_local* locals_end;

	// Intrusive linked list
	struct arC_func* head;
	struct arC_func* tail;

	// Function. Freeing memory of this is done by the pipeline and not this type
	arB_func* func;
} arC_func;

// Fetch a properties object that can be used to create a new type
ARLANG_API const arC_type_props* arC_type_props_get(const arString* name, arUint32 size, arUint32 flags, arUint8 data_type, arC_type* of_type);

// Create a new type.
// TODO Make it possible to create a new type based on a vmp type
ARLANG_API arC_type* arC_type_new(const arString* name);

// Create a new type from a properties structure
ARLANG_API arC_type* arC_type_from_props(const arC_type_props* props);

// Destroy the supplied type
ARLANG_API void arC_type_destroy(arC_type* p);

// Create a signature for the supplied type. Returns TRUE if the signature was successfully created
ARLANG_API BOOL arC_type_build_signature(arC_type* ptr, const struct arC_state* s);

// Manually set the signature
ARLANG_API void arC_type_set_signature(arC_type* ptr, const arString* sig);

// Resolve the supplied type and get the type
ARLANG_API BOOL arC_type_resolve(arC_type* t, const struct arC_state* s);

// Create a new package
ARLANG_API arC_package* arC_package_new(const arString* name);

// Destroy the supplied package
ARLANG_API void arC_package_destroy(arC_package* ptr);

// Create a signature for the supplied package. Returns TRUE if the signature was successfully created
ARLANG_API BOOL arC_package_build_signature(arC_package* ptr, const struct arC_state* s);

// Resolve the supplied type and get the type
ARLANG_API BOOL arC_package_resolve(arC_package* p, const struct arC_state* s);

// Search for a function
ARLANG_API arC_func* arC_package_find_func(arC_package* p, const arString* name);

// Search for a type
ARLANG_API arC_type* arC_package_find_type(arC_package* p, const arString* name);

// Search for a generic symbol. It might be a function, type, constant or global variable
ARLANG_API arC_symbol* arC_package_find_symbol(arC_package* p, const arString* name);

// Add a type in the supplied package
ARLANG_API void arC_package_add_type(arC_package* p, arC_type* t);

// Add a function in the supplied package
ARLANG_API void arC_package_add_func(arC_package* p, arC_func* f);

// Add a sub-package in the supplied package
ARLANG_API void arC_package_add_package(arC_package* p, arC_package* sub_package);

// Initialize the memory of the supplied signature
ARLANG_API void arC_func_sign_init(arC_func_sign* signature);

// Create a signature for the supplied function. Returns TRUE if the signature was successfully created
ARLANG_API BOOL arC_func_sign_build(arC_func_sign* sign, const struct arC_state* s);

// Parse the supplied compiler state and build a function signature from it
ARLANG_API BOOL arC_func_sign_parse(arC_func_sign* sign, const struct arC_state* s);

// Create a new function using the supplied signature
ARLANG_API arC_func* arC_func_new(const arC_func_sign* signature);

// Destroy the supplied function
ARLANG_API void arC_func_destroy(arC_func* f);

// Add the supplied argument
ARLANG_API void arC_func_add_arg(arC_func* f, arC_arg* a);

// Add the supplied return value
ARLANG_API void arC_func_add_return(arC_func* f, arC_return* r);

// Add the supplied local value
ARLANG_API void arC_func_add_local(arC_func* f, arC_local* l);

// Resolve the supplied type and get the type
ARLANG_API BOOL arC_func_resolve(arC_func* f, const struct arC_state* s);

// Search for a local variable in this function
ARLANG_API arC_local* arC_func_find_local(arC_func* f, const arString* name);

// Search for an argument of a specific name
ARLANG_API arC_arg* arC_func_find_arg(arC_func* f, const arString* name);

// Search for a symbol variable in this function
ARLANG_API arC_symbol* arC_func_find_symbol(arC_func* f, const arString* name);

// Return how many arguments are part of the function
ARLANG_API arInt32 arC_func_count_args(arC_func* f);

// Return how many arguments are part of the function
ARLANG_API arInt32 arC_func_count_returns(arC_func* f);

// Create a new arg
ARLANG_API arC_arg* arC_arg_new(const arString* name);

// Destroy the supplied arg
ARLANG_API void arC_arg_destroy(arC_arg* ptr);

// Create a new ret
ARLANG_API arC_return* arC_return_new();

// Destroy the supplied ret
ARLANG_API void arC_return_destroy(arC_return* ptr);

// Create a new local variable
ARLANG_API arC_local* arC_local_new(const arString* name);

// Destroy the supplied local
ARLANG_API void arC_local_destroy(arC_local* ptr);

#endif
