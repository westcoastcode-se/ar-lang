#ifndef _ARC_SOURCE_CODE_H_
#define _ARC_SOURCE_CODE_H_

#include "../arString.h"

// Source Code to be compiled by the compiler
typedef struct arC_source_code
{
	// The source code itself
	arString source;
	// The filename
	arString filename;
	// The compiler this source code is part of
	struct arCompiler* compiler;
	// Intrusive linked list
	struct arC_source_code* head;
	struct arC_source_code* tail;
} arC_source_code;

// New source code. It is expected that the source code is NULL terminated
extern arC_source_code* arC_source_code_new(const arString* src, const arString* filename);

// Destroy the supplied source code
extern void arC_source_code_destroy(arC_source_code* ptr);

#endif
