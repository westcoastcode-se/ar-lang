#ifndef _ARB_TYPES_H_
#define _ARB_TYPES_H_

#include "../arString.h"
#include "../arList.h"

// Return value indicating that the item is already added once
#define VMP_LIST_ADDED (0)

// Return value indicating that the item is already added once
#define VMP_LIST_OUT_OF_MEMORY (1)

// Return value indicating that the item is already added once
#define VMP_LIST_ALREADY_ADDED (2)

// Return value indicating that the another item with the same identifier already exist
#define VMP_LIST_ALREADY_EXISTS (3)

DECLARE_LIST_TYPE(arB_types, arB_type);
DECLARE_LIST_TYPE_FIND(arB_types, arB_type);

DECLARE_LIST_TYPE(arB_funcs, arB_func);
DECLARE_LIST_TYPE_FIND(arB_funcs, arB_func);

DECLARE_LIST_TYPE(vmp_list_globals, arB_global);
DECLARE_LIST_TYPE_FIND(vmp_list_globals, arB_global);

typedef struct arB_package
{
	// The name of the package
	arString name;

	// THe signature of the package
	arString signature;

	// Pipeline this package is added to
	struct arBuilder* pipeline;

	// All types in this package
	arB_types types;

	// All functions in this package
	arB_funcs funcs;

	// Global variables
	vmp_list_globals globals;
} arB_package;

DECLARE_LIST_TYPE(arB_inherited_by, arB_type);
DECLARE_LIST_TYPE_FIND(arB_inherited_by, arB_type);

DECLARE_LIST_TYPE(arB_inherits_from, arB_type);
DECLARE_LIST_TYPE_FIND(arB_inherits_from, arB_type);

// A type such as an int32. It can also be a more complex struct type
typedef struct arB_type
{
	// The name of the type
	arString name;

	// The signature of the type
	arString signature;

	// The package this type is part of
	arB_package* package;

	// Size, in bytes, that this type takes. If the size is 0 then this type has an unknown size
	arUint32 size;

	// Flags
	arUint32 flags;

	// Information on the interpreters data type
	arUint8 data_type;

	// If this type is of another type. Normally used for pointer- and array types
	const struct arB_type* of_type;

	// What this type is inherited from
	arB_inherits_from inherits_from;

	// What types this type is inherited by
	arB_inherited_by inherited_by;
} arB_type;

// Define which marks a type as a pointer
#define arB_TYPE_FLAGS_PTR (1 << 0)

// Define which marks a type as an array
#define arB_TYPE_FLAGS_ARRAY (1 << 1)

// Structure used to help creating a type
typedef struct arB_type_props
{
	arString name;
	arUint32 size;
	arUint32 flags;
	arUint8 data_type;
	arB_type* of_type;
} arB_type_props;

DECLARE_LIST_TYPE(arB_locals, arB_local);
DECLARE_LIST_TYPE_FIND(arB_locals, arB_local);

DECLARE_LIST_TYPE(arB_returns, arB_return);

DECLARE_LIST_TYPE(arB_args, arB_arg);
DECLARE_LIST_TYPE_FIND(arB_args, arB_arg);

DECLARE_LIST_TYPE(arB_markers, arB_marker);

typedef struct arB_func
{
	// The name of the function
	arString name;

	// The signature of the function
	arString signature;

	// The package this function is part of
	arB_package* package;

	// Offset where the bytecode for this function is found. Will be -1 if the offset
	// is now known yet. The compiler will try to resolve the offset during the
	// link step
	arUint32 offset;

	// The size, in bytes, for this function
	arUint32 body_size;

	// Arguments
	arB_args args;

	// Return values
	arB_returns returns;

	// Locals
	arB_locals locals;

	// Memory markers
	arB_markers markers;

	// A reference to a native function if one exist
	arB_native_func native_func;

	// Flags used to define the function
	arInt32 flags;

	// The stack size required for this function to work
	arUint32 args_stack_size;
	arUint32 returns_stack_size;
	arUint32 locals_stack_size;

	// The first instruction in this function
	struct arB_definition* first_instr;
	struct arB_definition* last_instr;
} arB_func;

// A function is public
#define arB_FUNC_FLAGS_PUBLIC (1 << 0)
// A function is marked as an external function
#define arB_FUNC_FLAGS_EXTERN (1 << 1)

typedef struct arB_arg
{
	// The name of the argument
	arString name;

	// Function this argument is part of
	const arB_func* func;

	// The type this argument is
	const arB_type* type;

	// Offset, in bytes, where this argument is located on the stack (from EPB's point of view)
	arUint32 offset;

	// The index of the argument
	arUint32 index;
} arB_arg;

// Represents a return value from a function
typedef struct arB_return
{
	// The (optional) name of the return value
	arString name;

	// Function this return property is part of
	const arB_func* func;

	// The type this return is
	const arB_type* type;
} arB_return;

typedef struct arB_local
{
	// The name of the local variable
	arString name;

	// Function this argument is part of
	const arB_func* func;

	// The type this argument is
	const arB_type* type;

	// Offset, in bytes, where this argument is located on the stack (from EPB's point of view)
	arUint32 offset;

	// The index where the local variable exist
	arUint32 index;
} arB_local;

typedef struct arB_global
{
	// The name of the global variable
	arString name;

	// Package this global variable is part of
	const arB_package* package;

	// The type this variable is
	const arB_type* type;

	// Offset, in bytes, where the global varaible can be found
	arUint32 offset;
} arB_global;

// Markers are used by the builder to remember a location in memory. Normally used 
// when dealing with branches, such as if-statements and loops
typedef struct arB_marker
{
	const arB_func* func;
	arInt32 instr_offset;
} arB_marker;

// New package
ARLANG_API arB_package* arB_package_new(const arString* name);

// New package
ARLANG_API arB_package* arB_package_newsz(const char* name, arInt32 len);

// Set the signature of the package
ARLANG_API void arB_package_set_signature(arB_package* p, const arString* name);

// Set the signature of the package
ARLANG_API void arB_package_set_signaturesz(arB_package* p, const char* name, arInt32 len);

// Destroy package
ARLANG_API void arB_package_destroy(arB_package* p);

// Add the supplied function to this package. This function will return 0 we added the type successfully
ARLANG_API int arB_package_add_func(arB_package* p, arB_func* func);

// Add the supplied type to this package. This function will return 0 we added the type successfully
ARLANG_API int arB_package_add_type(arB_package* p, arB_type* type);

// Add a global variable to a package
ARLANG_API int arB_package_add_global(arB_package* p, arB_global* g);

// New type
ARLANG_API arB_type* arB_type_new(const arString* name);

// New type based on predefined properties
ARLANG_API arB_type* arB_type_from_props(const arB_type_props* props);

// Get properties that can be used when creating a new type.
// Important! This function is not thread-safe
ARLANG_API const arB_type_props* arB_type_props_get(const arString* name, arUint32 size, arUint32 flags, arUint8 data_type, arB_type* of_type);

// Mark the supplied type as an of type
ARLANG_API BOOL arB_type_set_of_type(arB_type* t, const arB_type* of_type);

// Destroy type
ARLANG_API void arB_type_destroy(arB_type* p);

// Set the signature of the type
ARLANG_API void arB_type_set_signature(arB_type* p, const arString* name);

// Check to see if the supplied type inherits from the test
ARLANG_API BOOL arB_type_test_inherits_from(const arB_type* type, const arB_type* inherits_from);

// Tell the pipeline that the supplied inherits from another type. Returns FALSE if the inheritence is not allowed
ARLANG_API BOOL arB_type_inherit_from(arB_type* type, arB_type* inherits_from);

// Check if you can convert one type into another using convertion
ARLANG_API BOOL arB_type_can_convert(const arB_type* from, const arB_type* to);

// New arg
ARLANG_API arB_arg* arB_arg_new();

// Destroy arg
ARLANG_API void arB_arg_destroy(arB_arg* a);

// Set the name of the argument
ARLANG_API void arB_arg_set_name(arB_arg* a, const arString* name);

// Set the name of the argument
ARLANG_API void arB_arg_set_namesz(arB_arg* a, const char* name, arInt32 len);

// New return value
ARLANG_API arB_return* arB_return_new();

// Destroy return
ARLANG_API void arB_return_destroy(arB_return* a);

// New local
ARLANG_API arB_local* arB_local_new();

// Destroy arg
ARLANG_API void arB_local_destroy(arB_local* l);

// Set the name of the argument
ARLANG_API void arB_local_set_name(arB_local* l, const arString* name);

// Set the name of the argument
ARLANG_API void arB_local_set_namesz(arB_local* l, const char* name, arInt32 len);

// New global variable
ARLANG_API arB_global* arB_global_new();

// Destroy the global variable
ARLANG_API void arB_global_destroy(arB_global* g);

// Set the name of the variable
ARLANG_API void arB_global_set_name(arB_global* g, const arString* name);

// Set the name of the variable
ARLANG_API void arB_global_set_namesz(arB_global* g, const char* name, arInt32 len);

// Set the name of the variable
ARLANG_API void arB_global_set_type(arB_global* g, arB_type* type);

// New type
ARLANG_API arB_func* arB_func_new(const arString* name);

// New type
ARLANG_API arB_func* arB_func_newsz(const char* name, arInt32 name_len);

// Destroy type
ARLANG_API void arB_func_destroy(arB_func* p);

// Set the signature of the function
ARLANG_API void arB_func_set_signature(arB_func* func, const arString* name);

// Add a specific flag
ARLANG_API void arB_func_add_flag(arB_func* p, arInt32 flag);

// Set the function
ARLANG_API void arB_func_set_native(arB_func* p, arB_native_func func);

// Add an argument to this function
ARLANG_API BOOL arB_func_add_arg(arB_func* f, arB_arg* arg);

// Add an argument to this function
ARLANG_API arB_arg* arB_func_new_arg(arB_func* f, arB_type* type);

// Add an return value to this function
ARLANG_API BOOL arB_func_add_return(arB_func* f, arB_return* ret);

// Add a return value from this function
ARLANG_API arB_return* arB_func_new_return(arB_func* f, arB_type* type);

// Add a local variable to this function
ARLANG_API BOOL arB_func_add_local(arB_func* f, arB_local* local);

// Add a local variable for this function of a specific type
ARLANG_API arB_local* arB_func_new_local(arB_func* f, arB_type* type);

// Add a memory marker
ARLANG_API arB_marker* arB_func_new_marker(arB_func* f);

// Set the first instruction to be called when jumping to the supplied marker
ARLANG_API void arB_marker_set_instr(arB_marker* m, struct arB_definition* instr);

// Remove memory marker
ARLANG_API void arB_marker_destroy(arB_marker* m);

// Add a new instruction. If the there's followup instructions attached to the instruction then those will be added
// as well. Returns the last added instruction
ARLANG_API struct arB_definition* arB_func_add_instr(arB_func* f, struct arB_definition* instr);

// Inject a new instruction after another instruction. If NULL then it will be put at the beginning
ARLANG_API struct arB_definition* arB_func_inject_after(arB_func* f, struct arB_definition* after, struct arB_definition* instr);

// Remove the instruction and return it for deletion
ARLANG_API struct arB_definition* arB_func_remove_instr(arB_func* f, struct arB_definition* instr);

// Begin writing the body instructions
ARLANG_API void arB_func_body_begin(arB_func* f);

// Instructions written to the body is now complete
ARLANG_API void arB_func_body_end(arB_func* f);

#endif
