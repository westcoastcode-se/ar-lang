#include "vmcd_source_code.h"

vmcd_source_code* vmcd_source_code_create(const vm_byte* bytes, vm_int32 len)
{
	vmcd_source_code* s = (vmcd_source_code*)malloc(sizeof(vmcd_source_code));
	if (s == NULL)
		return NULL;
	vm_mutable_string_malloc(&s->source_code, bytes, len);
	return s;
}

void vmcd_source_code_destroy(vmcd_source_code* s)
{
	vm_mutable_string_free(&s->source_code);
	vm_mutable_string_free(&s->filename);
}

void vmcd_source_code_set_filename(vmcd_source_code* s, const vm_byte* filename, vm_int32 len)
{
	vm_mutable_string_malloc(&s->filename, filename, len);
}
