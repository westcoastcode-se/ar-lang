#ifndef _VMCD_TYPES_H_
#define _VMCD_TYPES_H_

#include "../vm_string.h"
#include "vmcd_lexer.h"
#include "../vmp/vmp.h"

// Types
enum vmcd_keyword_type
{
	VMCD_TYPE_PACKAGE,
	VMCD_TYPE_FUNC,
	VMCD_TYPE_STATEMENT,
};
typedef enum vmcd_keyword_type vmcd_keyword_type;

// Header for all compiler keywords
struct vmcd_keyword_header
{
	vmcd_keyword_type type;
};
typedef struct vmcd_keyword_header vmcd_keyword_header;

// Types
enum vmcd_statement_type
{
	VMCD_STATEMENT_OPERATOR,
	VMCD_STATEMENT_VALUE,
	VMCD_STATEMENT_CONVERT_VALUE,
};
typedef enum vmcd_statement_type vmcd_statement_type;

// A generic statement, such as a function call or a constant value
struct vmcd_statement
{
	vmcd_keyword_header header;
	vmcd_statement_type statement_type;
	struct vmcd_keyword_statement* prev;
	struct vmcd_keyword_statement* next;
};
typedef struct vmcd_statement vmcd_statement;

// A package
struct vmcd_keyword_package
{
	vmcd_keyword_header header;
	
	// Name of the package
	vm_string name;

	// The actual package
	vmp_package* package;
};
typedef struct vmcd_keyword_package vmcd_keyword_package;

// A function
struct vmcd_keyword_func
{
	vmcd_keyword_header header;
	
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
