#ifndef _ZPP_SOURCE_CODE_H_
#define _ZPP_SOURCE_CODE_H_

#include "../vm_string.h"

typedef struct zpp_source_code
{
	// The source code itself
	vm_string source;
	// The filename
	vm_string filename;
	// The compiler this source code is part of
	struct zpp_compiler* compiler;
	// Intrusive linked list
	struct zpp_source_code* head;
	struct zpp_source_code* tail;
} zpp_source_code;

// New source code. It is expected that the source code is NULL terminated
extern zpp_source_code* zpp_source_code_new(const vm_string* src, const vm_string* filename);

// Destroy
extern void zpp_source_code_destroy(zpp_source_code* ptr);

#endif
