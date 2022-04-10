#ifndef _VM_ERROR_H_
#define _VM_ERROR_H_

#include "../vm_message.h"

enum vmc_error_code
{
	VMC_ERR_NO_ERROR = 0,

	VMC_CERR_START,
	VMC_CERR_LABEL_ALREADY_EXIST,
	VMC_CERR_INVALID_ARG,
	VMC_CERR_EXPECTED_COMMA,

	VMC_LERR_START,
	VMC_LERR_MISSING_LABEL,

	VMC_ERROR_CODE_UNKNOWN_TOKEN
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

// Set error where we define the same label multiple times
static inline BOOL vmc_error_label_already_exists(vm_message* err, const char* s, int len)
{
	return vm_message_set(err, VMC_CERR_LABEL_ALREADY_EXIST, "label already exists");
}

#endif
