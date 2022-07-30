#include "vmcd_source_code.h"
#include "../vm_debug.h"

vmcd_source_code* vmcd_source_code_create(const vm_byte* bytes)
{
	vmcd_source_code* s = (vmcd_source_code*)vm_malloc(sizeof(vmcd_source_code));
	if (s == NULL)
		return NULL;
	vm_mutable_string_malloc(&s->source_code, bytes, vm_str_len(bytes) + 1);
	return s;
}

void vmcd_source_code_destroy(vmcd_source_code* s)
{
	vm_mutable_string_free(&s->source_code);
	vm_mutable_string_free(&s->filename);
	vm_free(s);
}

void vmcd_source_code_set_filename(vmcd_source_code* s, const vm_byte* filename, vm_int32 len)
{
	vm_mutable_string_malloc(&s->filename, filename, len);
}
