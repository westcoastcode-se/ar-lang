#ifndef _VMC_ERROR_H_
#define _VMC_ERROR_H_

#include "../vm_message.h"

enum vmc_error_code
{
	VMC_ERR_NO_ERROR = 0,

	VMC_CERR_START,
	VMC_CERR_SYMBOL_ALREADY_EXIST,
	VMC_CERR_INVALID_ARG,
	VMC_CERR_EXPECTED_COMMA,
	VMC_CERR_OUT_OF_MEMORY,

	VMC_LERR_START,
	VMC_LERR_MISSING_LABEL,

	// Lexer errors

	// Lexer warnings
	VMC_WARN_CODE_ESCAPE_RUNE_UNKNOWN,
	VMC_WARN_CODE_UNCLOSED_STRING,
	VMC_WARN_CODE_UNCLOSED_COMMENT,

	// Compiler errors

	VMC_ERROR_CODE_UNKNOWN_TOKEN,
	VMC_ERROR_CODE_INVALID_SEGMENT,
	VMC_ERROR_CODE_EXPECTED_IDENTIFIER,
	VMC_ERROR_CODE_EXPECTED_TYPE,
	VMC_ERROR_CODE_TYPE_NOT_FOUND,
	VMC_ERROR_CODE_EXPECTED_PARANT,
	VMC_ERROR_CODE_EXPECTED_BRACKET,
	VMC_ERROR_CODE_EXPECTED_KEYWORD,
	VMC_ERROR_CODE_EXPECTED_INDEX,
	VMC_ERROR_CODE_EXPECTED_INTEGER,
	VMC_ERROR_CODE_INCOMPLETE_FN_BODY,
	VMC_ERROR_CODE_INVALID_ARG,
	VMC_ERROR_CODE_NOT_IMPLEMENTED_YET,
	VMC_ERROR_CODE_EXTERN_UNKNOWN,

};
typedef enum vmc_error_code vmc_error_code;

// Set error where comma is missing
static inline BOOL vmc_error_unknown_token(vm_message* err)
{
	return vm_message_set(err, VMC_ERROR_CODE_UNKNOWN_TOKEN, "use comma as a delimiter between mutliple args");
}

// Set error where comma is missing
static inline BOOL vmc_error_set_comma(vm_message* err)
{
	return vm_message_set(err, VMC_CERR_EXPECTED_COMMA, "use comma as a delimiter between mutliple args");
}

#endif
