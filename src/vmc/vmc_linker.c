#include "vmc_linker.h"
#include "../interpreter/vmi_config.h"

vmc_linker_memory_marker* vmc_linker_memory_marker_alloc()
{
	vmc_linker_memory_marker* const marker = (vmc_linker_memory_marker*)malloc(sizeof(vmc_linker_memory_marker));
	if (marker == NULL) {
		return FALSE;
	}
	vm_string_zero(&marker->name);
	marker->offset = 0;
	marker->next = NULL;
	return marker;
}

void vmc_linker_memory_marker_destroy(vmc_linker_memory_marker* m)
{
	if (m == NULL)
		return;
	while (m != NULL) {
		vmc_linker_memory_marker* const next = m->next;
		free(m);
		m = next;
	}
}

vmc_linker_memory_marker* vmc_linker_memory_marker_find_sibling(vmc_linker_memory_marker* m, const vm_string* name)
{
	while (m != NULL) {
		if (vm_string_cmp(&m->name, name)) {
			return m;
		}
		m = m->next;
	}
	return NULL;
}

void vmc_linker_init(vmc_linker* l, vm_bytestream* stream)
{
	l->bytestream = stream;
	l->targets_first = l->targets_last = NULL;
}

void vmc_linker_process(vmc_linker* l)
{
	vmc_linker_memory_marker_target* t = l->targets_first;
	while (t != NULL) {
		// The memory location where the bytecode offset should be injected into
		vm_byte* const mem = l->bytestream->memory + t->bytestream_offset;
		// TODO: Assume that the target location is a vmi_ip (const vm_byte*) type
		vm_byte** destination = (vm_byte**)mem;
		*destination = l->bytestream->memory + t->marker->offset;
		t = t->next;
	}
}

void _vmc_linker_memory_marker_target_free(vmc_linker_memory_marker_target* t)
{
	while (t != NULL) {
		vmc_linker_memory_marker_target* const next = t->next;
		free(t);
		t = next;
	}
}

void vmc_linker_release(vmc_linker* l)
{
	_vmc_linker_memory_marker_target_free(l->targets_first);
}

vmc_linker_memory_marker_target* vmc_linker_add_memory_marker(vmc_linker* l, vmc_linker_memory_marker* marker,
	vm_int32 offset)
{
	vmc_linker_memory_marker_target* const target = (vmc_linker_memory_marker_target*)malloc(sizeof(vmc_linker_memory_marker_target));
	if (target == NULL)
		return NULL;
	target->marker = marker;
	target->bytestream_offset = vm_bytestream_pos(l->bytestream) + offset;
	target->next = NULL;
	if (l->targets_last == NULL) {
		l->targets_first = l->targets_last = target;
	}
	else {
		l->targets_last->next = target;
		l->targets_last = target;
	}
	return target;
}
