#ifndef _VMCD_TYPES_H_
#define _VMCD_TYPES_H_

#include "../vm_string.h"
#include "vmcd_lexer.h"
#include "vmcd_lists.h"
#include "../vmp/vmp.h"
#include "syntax_tree.h"

// Types
typedef enum vmcd_keyword_type
{
	// Package
	VMCD_KEYWORD_PACKAGE,

	// A function that might be associated with a struct
	VMCD_KEYWORD_FUNC,

	// A structure, type or something like that
	VMCD_KEYWORD_TYPE,

	// A package constant
	VMCD_KEYWORD_CONST,

	// A package global variable
	VMCD_KEYWORD_GLOBAL,

	// A function local variable
	VMCD_KEYWORD_LOCAL,

	// An argument
	VMCD_KEYWORD_ARG,

	// An return value
	VMCD_KEYWORD_RETURN,

	// An unresolved keyword. This is normally used for when you are referring to
	// something before it's declared by the developer
	VMCD_KEYWORD_UNRESOLVED,
} vmcd_keyword_type;

// Header for all keywords
typedef struct vmcd_keyword
{
	vmcd_keyword_type type;
	vm_string name;
} vmcd_keyword;

// Get the keyword representation for the supplied pointer
#define VMCD_KEYWORD(p) (&p->header)

// A package
typedef struct vmcd_package
{
	vmcd_keyword header;
	
	// All types
	vmcd_list_types types;

	// Functions contained in this package
	vmcd_list_funcs funcs;

	// All imported packages
	vmcd_list_imports imports;

	// The actual package
	vmp_package* package;
} vmcd_package;

// Represents an unresolved keyword. When this keyword is resolved then the proxy pointer will be set
typedef struct vmcd_keyword_unresolved
{
	vmcd_keyword header;
	vmcd_keyword* proxy;
} vmcd_keyword_unresolved;

typedef enum vmcd_type_flag
{
	VMCD_TYPE_FLAG_PTR = 1 << 0,
	VMCD_TYPE_FLAG_ARRAY = 1 << 1,
} vmcd_type_flag;

// Type
typedef struct vmcd_type
{
	vmcd_keyword header;

	// The package this type is part of
	vmcd_package* package;

	// The size of this type
	vm_int32 size;

	// Flags
	vm_bits32 flags;

	// What type of underlying datatype is this considered to be
	vm_uint8 data_type;

	// If this type is of another type. Normally used for pointer- and array types
	struct vmcd_type* of_type;

	// What this type is inherited from
	vmcd_list_inherits_from inherits_from;

	// What types this type is inherited by
	vmcd_list_inherited_by inherited_by;

	// The type, cached, so that we can much faster reuse it later on. This is realized after the parsing is done
	// and the actual compilation has started
	vmp_type* type;
} vmcd_type;

// Structure used to help creating a type
struct vmcd_type_props
{
	vm_string name;
	vm_uint32 size;
	vm_uint32 flags;
	vm_uint8 data_type;
	vmcd_type* of_type;
};
typedef struct vmcd_type_props vmcd_type_props;

// A package-global variable
typedef struct vmcd_global
{
	vmcd_keyword header;

	// Package this global variable is part of
	vmcd_package* package;

	// The type this variable is
	vmcd_keyword* type;

	// The global, cached. This is realized after the parsing is done
	// and the actual compilation has started
	vmp_global* global;
} vmcd_global;

// Argument
typedef struct vmcd_arg
{
	vmcd_keyword header;

	// The type of the argument. It might be a type or an unresovled type
	vmcd_keyword* type;

	// Intrusive list
	struct vmcd_arg* head;
	struct vmcd_arg* tail;

	// The argument
	vmp_arg* arg;
} vmcd_arg;

// Return values
typedef struct vmcd_return
{
	vmcd_keyword header;

	// The type of the argument
	vmcd_keyword* type;

	// Intrusive list
	struct vmcd_return* head;
	struct vmcd_return* tail;

	// The return value
	vmp_return* ret;
} vmcd_return;

// Flags that define what type of function this is
typedef enum vmcd_func_flag
{
	VMCD_FUNC_FLAG_PUBLIC = (1 << 0),
	VMCD_FUNC_FLAG_EXTERN = (1 << 1)
} vmcd_func_flag;

// A function
typedef struct vmcd_func
{
	vmcd_keyword header;

	// Function flags
	vm_bits32 flags;
	
	// The package this function is part of
	vmcd_package* package;

	// Arguments
	vmcd_arg* args;
	vmcd_arg* args_end;

	// Returns values
	vmcd_return* returns;
	vmcd_return* returns_end;

	// Operations done by this function
	vmcd_syntax_tree_node syntax_tree;
	vmcd_syntax_tree_node syntax_tree_end;

	// The actual function
	vmp_func* func;
} vmcd_func;

// Create a new type
extern vmcd_type* vmcd_type_new(const vm_string* name);

// Destroy the supplied package
extern void vmcd_type_destroy(vmcd_type* ptr);

// Create a new type based on a properties structure
extern vmcd_type* vmcd_type_new_from_props(const vmcd_type_props* props);

// Fetch a properties object that can be used to create a new type
extern const vmcd_type_props* vmcd_type_props_get(const vm_string* name, vm_uint32 size, vm_uint32 flags, vm_uint8 data_type, vmcd_type* of_type);

// Mark the supplied type as an of type
extern BOOL vmcd_type_of_type(vmcd_type* t, vmcd_type* of_type);

// Resolve the supplied type
extern vmp_type* vmcd_type_resolve_type(vmcd_type* t, struct vmcode* s);

// Create a new package
extern vmcd_package* vmcd_package_new(const vm_string* name);

// Destroy the supplied package
extern void vmcd_package_destroy(vmcd_package* ptr);

// Search for a function
extern vmcd_func* vmcd_package_find_func(vmcd_package* p, const vm_string* name);

// Search for a type
extern vmcd_type* vmcd_package_find_type(vmcd_package* p, const vm_string* name);

// Search for a type including packages
extern vmcd_type* vmcd_package_find_type_include_imports(vmcd_package* p, const vm_string* name);

// Search for an imported package
extern vmcd_package* vmcd_package_find_import(vmcd_package* p, const vm_string* name);

// Result of when
typedef enum vmcd_package_add_result
{
	// Add failed because out of memory
	VMCD_PACKAGE_ADD_OUT_OF_MEMORY = 0,
	// The item already exist
	VMCD_PACKAGE_ADD_DUPLICATE = 1,
	// Add successfully
	VMCD_PACKAGE_ADD_ADDED = 2
} vmcd_package_add_result;

// Add a type in the supplied package. If the return value is non-zero then the type exists in the package. If the value
// is VMCD_PACKAGE_ADD_DUPLICATE then the item is already added
extern vmcd_package_add_result vmcd_package_add_type(vmcd_package* p, vmcd_type* type);

// Add a function in the supplied package. If the return value is non-zero then the function exists in the package. If the value
// is VMCD_PACKAGE_ADD_DUPLICATE then the item is already added
extern vmcd_package_add_result vmcd_package_add_func(vmcd_package* p, vmcd_func* f);

// Add a new import in the supplied package. If the return value is non-zero then the import exists inside the package in the end. 
// If the value is VMCD_PACKAGE_ADD_DUPLICATE then the item is already added
extern vmcd_package_add_result vmcd_package_add_import(vmcd_package* p, vmcd_package* i);

// Compile package
extern BOOL vmcd_package_compile(vmcd_package* p, struct vmcode* s);

// Resolve the package
extern vmp_package* vmcd_package_resolve_package(vmcd_package* p, struct vmcode* s);

// Create a new function
extern vmcd_func* vmcd_func_new(const vm_string* name);

// Destroy the supplied function
extern void vmcd_func_destroy(vmcd_func* ptr);

// Add a syntax tree node to the supplied function
extern void vmcd_func_add_syntax_tree(vmcd_func* f, vmcd_syntax_tree_node node);

// Add the supplied argument
extern void vmcd_func_add_arg(vmcd_func* f, vmcd_arg* a);

// Add the supplied return value
extern void vmcd_func_add_return(vmcd_func* f, vmcd_return* r);

// Compile the supplied function. If the return value is FALSE then the compilation failed for this function
extern BOOL vmcd_func_compile(vmcd_func* f, struct vmcode* s);

// Resolve the function
extern vmp_func* vmcd_func_resolve_func(vmcd_func* f, struct vmcode* s);

// Create a new arg
extern vmcd_arg* vmcd_arg_new(const vm_string* name);

// Destroy the supplied arg
extern void vmcd_arg_destroy(vmcd_arg* ptr);

// Create a new ret
extern vmcd_return* vmcd_return_new();

// Destroy the supplied ret
extern void vmcd_return_destroy(vmcd_return* ptr);

// resolve a type based on a specific keyword
extern vmp_type* vmcd_keyword_resolve_type(vmcd_keyword* keyword);

#endif
