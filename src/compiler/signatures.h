#ifndef _ARC_SIGNATURES_H_
#define _ARC_SIGNATURES_H_

#include "../arString.h"

// A package's signature
typedef struct arC_signature_package
{
	// The full signature
	arString signature;
	// The signature in local format
	arString local_signature;
	// The name of the function
	arString name;
	// The parent package
	struct arC_package* parent;
} arC_signature_package;

// A type's signature
typedef struct arC_signature_type
{
	// The full signature
	arString signature;
	// The signature in local format
	arString local_signature;
	// The name of the function
	arString name;
	// The parent symbol
	struct arC_symbol* parent;
} arC_signature_type;

// The signature for a function
typedef struct arC_signature_func
{
	// The full signature
	arString signature;
	// The signature in local format
	arString local_signature;
	// The name of the function
	arString name;
	// The parent symbol
	struct arC_symbol* parent;
	// Arguments to the function
	struct arC_arg* arguments;
	struct arC_arg* arguments_end;
	arInt32 arguments_count;
	// All return values returned by this function
	struct arC_return* returns;
	struct arC_return* returns_end;
	arInt32 returns_count;
	// TODO: Add scope
} arC_signature_func;

// Initialize memory for package signature
ARLANG_API void arC_signature_package_init(arC_signature_package* p);

// Build a package's signature
ARLANG_API BOOL arC_signature_package_build(arC_signature_package* p, const struct arC_state* s);

// Initialize the memory for the supplied signature
ARLANG_API void arC_signature_type_init(arC_signature_type* p);

// Build the signature
ARLANG_API BOOL arC_signature_type_init_and_build(arC_signature_type* p, const arString* name, struct arC_symbol* parent, const struct arC_state* s);

// Build a type's signature
ARLANG_API BOOL arC_signature_type_build(arC_signature_type* p, const struct arC_state* s);

// Initialize the internal structure for the signature
ARLANG_API void arC_signature_func_init(arC_signature_func* p);

// Parse the current source code and put the result into the signature
ARLANG_API BOOL arC_signature_func_parse(arC_signature_func* p, const struct arC_state* s);

// Build the signature
ARLANG_API BOOL arC_signature_func_build(arC_signature_func* p, const struct arC_state* s);


#endif
