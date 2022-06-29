#include "vmc_linker.h"
#include "../interpreter/vmi_config.h"
#include "vmc_linker_messages.h"

void _vmc_linker_marker_free(vmc_linker_marker_inject_addr* h)
{
	if (h == NULL)
		return;

	while (h != NULL) {
		vmc_linker_marker_inject_addr* const next = h->next;
		free(h);
		h = next;
	}
}

BOOL _vmc_linker_marker_process_inject_addr(vmc_linker* l, vm_bytestream* stream)
{
	const vmc_linker_marker_addr* addr;
	if (l->inject_addr_first == NULL) {
		return TRUE;
	}

	// Verify that if the address is resolved or not
	addr = l->inject_addr_first->addr;
	if (addr->offset == 0) {
		return vmc_linker_message_unresolved_addr(&l->messages, &addr->signature);
	}

	// Write the actual memory address
	vmc_linker_marker_inject_addr* m = l->inject_addr_first;
	while (m != NULL) {
		// Where to inject the address into
		vm_byte** const target = (vm_byte**)(stream->memory + m->offset);
		// Inject the address
		*target = stream->memory + addr->offset;

		m = m->next;
	}
	return TRUE;
}

void vmc_linker_init(vmc_linker* l)
{
	vm_messages_init(&l->messages);
	l->inject_addr_first = l->inject_addr_last = NULL;
}

BOOL vmc_linker_process(vmc_linker* l, vm_bytestream* stream)
{
	if (!_vmc_linker_marker_process_inject_addr(l, stream))
		return FALSE;

	return TRUE;
}

void vmc_linker_release(vmc_linker* l)
{
	_vmc_linker_marker_free(l->inject_addr_first);
	vm_messages_destroy(&l->messages);
}

vmc_linker_marker_inject_addr* vmc_linker_marker_add_inject_addr(vmc_linker* l, const vmc_linker_marker_addr* addr, vm_uint32 offset)
{
	vmc_linker_marker_inject_addr* const target = (vmc_linker_marker_inject_addr*)malloc(sizeof(vmc_linker_marker_inject_addr));
	if (target == NULL)
		return NULL;
	target->next = NULL;
	target->offset = offset;
	target->addr = addr;
	if (l->inject_addr_last == NULL) {
		l->inject_addr_first = l->inject_addr_last = target;
	}
	else {
		l->inject_addr_last->next = target;
		l->inject_addr_last = target;
	}
	return target;
}
