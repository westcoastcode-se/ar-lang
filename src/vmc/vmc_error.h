#ifndef _VMC_ERROR_H_
#define _VMC_ERROR_H_

#include "../vm_message.h"

enum vmc_error_code
{
	VMC_ERR_NO_ERROR = 0,

	VMC_CERR_SYMBOL_ALREADY_EXIST,
	VMC_CERR_OUT_OF_MEMORY,

};
typedef enum vmc_error_code vmc_error_code;

#endif
