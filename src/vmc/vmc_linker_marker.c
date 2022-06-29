#include "vmc_linker_marker.h"
#include "vmc_debug.h"

vmc_linker_marker_addr* vmc_linker_marker_addr_alloc(const vm_string* signature)
{
	vmc_linker_marker_addr* const m = (vmc_linker_marker_addr*)vmc_malloc(sizeof(vmc_linker_marker_addr));
	if (m == NULL) {
		return FALSE;
	}
	m->signature = *signature;
	m->offset = 0;
	m->next = NULL;
	return m;
}

void vmc_linker_marker_addr_destroy(vmc_linker_marker_addr* m)
{
	if (m == NULL)
		return;
	while (m != NULL) {
		vmc_linker_marker_addr* const next = m->next;
		vmc_free(m);
		m = next;
	}
}

vmc_linker_marker_addr* vmc_linker_marker_addr_search(vmc_linker_marker_addr* m, const vm_string* name)
{
	while (m != NULL) {
		if (vm_string_cmp(&m->signature, name)) {
			return m;
		}
		m = m->next;
	}
	return NULL;
}
