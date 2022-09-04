#ifndef _ARLANG_BYTECODE_H_
#define _ARLANG_BYTECODE_H_

#include "arconf.h"

// Header that describes the bytecode. The header is always at the start of the binary blob
typedef struct ar_process_header
{
	// "VM0"
	char header[3];

	// Compatibility version
	char version;

	// Offset where the code starts
	arUint32 code_offset;

	// Offset where data starts, such as constant data, package- and function information
	arUint32 data_offset;

	// Number of packages compiled and linked with the bytecode
	arUint32 packages_count;

	// Offset where the first package is found
	arUint32 first_package_offset;
} ar_process_header;

// Header for a function in the bytecode
typedef struct ar_function_header
{
	// Length of the name
	arUint32 name_length;

	// Offset where the function starts (in relation to the start of the bytecode)
	arUint32 ptr_start;

	// How many bytes are expected to be pushed for this function to work
	arInt32 expected_stack_size;

	// char name[]
} ar_function_header;

// Header for a package in the bytecode
typedef struct ar_package_header
{
	arUint32 name_length;
	arUint32 functions_count;
	arUint32 types_count;
	// char name[]
	// functions[]
} ar_package_header;

#endif