#include "source_code.h"
#include "../arMemory.h"

arC_source_code* arC_source_code_new(const arString* src, const arString* filename)
{
	assert(*src->end == 0 && "The last character is expected to be a NULL character");
	arC_source_code* p = arSafeMalloc(arC_source_code);
	p->source = *src;
	p->filename = *filename;
	p->head = p->tail = NULL;
	return p;
}

void arC_source_code_destroy(arC_source_code* ptr)
{
	arFree(ptr);
}
