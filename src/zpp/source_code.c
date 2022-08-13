#include "source_code.h"
#include "../vm_debug.h"

zpp_source_code* zpp_source_code_new(const vm_string* src, const vm_string* filename)
{
	assert(*src->end == 0 && "The last character is expected to be a NULL character");
	zpp_source_code* p = vm_safe_malloc(zpp_source_code);
	p->source = *src;
	p->filename = *filename;
	p->head = p->tail = NULL;
	return p;
}

void zpp_source_code_destroy(zpp_source_code* ptr)
{
	vm_free(ptr);
}
