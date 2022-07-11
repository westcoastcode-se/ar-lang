#include "vmcd_list_sources.h"
#include "vmcd_source_code.h"

#define CAPACITY (4)
#define RESIZE (4)

BOOL vmcd_list_sources_init(vmcd_list_sources* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (struct vmcd_source_code**)malloc(sizeof(vmcd_source_code*) * l->capacity);
	return l->memory != NULL;
}

void vmcd_list_sources_release(vmcd_list_sources* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmcd_source_code* const a = l->memory[i];
		vmcd_source_code_destroy(a);
	}

	free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmcd_list_sources_add(vmcd_list_sources* l, vmcd_source_code* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmcd_source_code**)realloc(l->memory, sizeof(vmcd_source_code*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmcd_source_code* vmcd_list_sources_get(const vmcd_list_sources* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}

vmcd_source_code* vmcd_list_sources_find(const vmcd_list_sources* l, const vm_string* filename)
{
	struct vmcd_source_code** item = &l->memory[0];
	struct vmcd_source_code** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmcd_source_code* p = *item;
		if (vm_string_cmp((vm_string*)&p->filename, filename)) {
			return p;
		}
	}
	return NULL;
}
