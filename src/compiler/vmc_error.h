#ifndef _VMC_ERROR_H_
#define _VMC_ERROR_H_

#include "../vm_config.h"

enum vmc_error_code
{
	VMC_ERR_NO_ERROR = 0,

	VMC_CERR_START,
	VMC_CERR_LABEL_ALREADY_EXIST,
	VMC_CERR_INVALID_ARG,
	VMC_CERR_EXPECTED_COMMA,

	VMC_LERR_START,
	VMC_LERR_MISSING_LABEL,
};
typedef enum vmc_error_code vmc_error_code;

// 
struct vmc_error
{
	int code;
	char message[4096];
};
typedef struct vmc_error vmc_error;

// Set error message and code to body. Will always return FALSE so that it can be part of the
// call structure.
BOOL vmc_error_set(vmc_error* err, int error_code, const char* format, ...);

// Set error where comma is missing
static inline BOOL vmc_error_set_comma(vmc_error* err)
{
	return vmc_error_set(err, VMERR_EXPECTED_COMMA, "use comma as a delimiter between mutliple args");
}

// Set error where end memory resolve is missing
static inline BOOL vmc_error_set_ref_not_closed(vmc_error* err)
{
	return vmc_error_set(err, VMERR_REF_NOT_CLOSED, "did not close memory reaf with ']'");
}


#endif
