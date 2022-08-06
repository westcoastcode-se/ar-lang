#ifndef _VMCD_TYPES_H_
#define _VMCD_TYPES_H_

#include "../vm_string.h"
#include "vmcd_lexer.h"
#include "../vmp/vmp.h"

// Types
typedef enum vmcd_keyword_type
{
	// Package
	VMCD_KEYWORD_PACKAGE,

	// A function that might be associated with a struct
	VMCD_KEYWORD_FUNC,

	// A structure, type or something like that
	VMCD_KEYWORD_TYPE,

} vmcd_keyword_type;

// Header for all keywords
typedef struct vmcd_keyword
{
	vmcd_keyword_type type;
} vmcd_keyword;

// Operations
typedef enum vmcd_operation_type
{
	VMCD_OPERATION_VALUE,
	VMCD_OPERATION_OPERATOR,
} vmcd_operation_type;

// A generic operation, such as a function call or a constant value
typedef struct vmcd_operation
{
	vmcd_operation_type op_type;
	struct vmcd_operation* prev;
	struct vmcd_operation* next;
} vmcd_operation;

// A package
struct vmcd_package
{
	vmcd_keyword header;
	
	// Name of the package
	vm_string name;

	// The actual package
	vmp_package* package;
};
typedef struct vmcd_package vmcd_package;

typedef struct vmcd_type
{
	// Name of the type
	vm_string name;

	// Name of the package
	const vmcd_type* of_type;
} vmcd_type;

// Argument
struct vmcd_arg
{
	// Name of the argument
	vm_string name;

	// The type of the argument
	vmcd_type* type;

	// Intrusive list
	struct vmcd_func_arg* next;
	struct vmcd_func_arg* prev;

	// The argument
	vmp_arg* arg;
};

// A function
struct vmcd_keyword_func
{
	vmcd_keyword header;
	
	// Name of the function
	vm_string name;

	// The actual function
	vmp_func* func;
};
typedef struct vmcd_keyword_func vmcd_keyword_func;

// An operator, such as "+"
struct vmcd_statement_operator
{
	vmcd_statement header;

	// The specific operator this is
	vmcd_token_type op;

	// The type this operator results into. For example, doing 10 + 20 results into an 4 byte integer
	const vmp_type* type;
};
typedef struct vmcd_statement_operator vmcd_statement_operator;

// A constant value
struct vmcd_statement_value
{
	vmcd_statement header;
	
	// The constant value
	vmp_constant value;

	// The type this constant is
	const vmp_type* type;
};
typedef struct vmcd_statement_value vmcd_statement_value;

// Convert a value from one type into another, such as (float)1
struct vmcd_statement_convert_value
{
	vmcd_statement header;

	// From what type are we converting
	const vmp_type* from_type;

	// To what type are we converting
	const vmp_type* to_type;
};
typedef struct vmcd_statement_convert_value vmcd_statement_convert_value;

#endif
