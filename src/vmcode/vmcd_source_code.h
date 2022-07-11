#ifndef _VMCD_SOURCE_CODE_H_
#define _VMCD_SOURCE_CODE_H_

#include "../vm_string.h"

struct vmcd_source_code
{
	// The source code in a vm_string format
	vm_mutable_string source_code;

	// Filename (used for debugging etc.). This is an optinal field
	vm_mutable_string filename;
};
typedef struct vmcd_source_code vmcd_source_code;

// Create a new source code blob
extern vmcd_source_code* vmcd_source_code_create(const vm_byte* bytes, vm_int32 len);

// Destroy the source code
extern void vmcd_source_code_destroy(vmcd_source_code* s);

// Set the, optional, filename for this source code
extern void vmcd_source_code_set_filename(vmcd_source_code* s, const vm_byte* filename, vm_int32 len);

#endif
