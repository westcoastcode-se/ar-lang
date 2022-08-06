#include "vmp_list_inherited_by.h"
#include "vmp_types.h"
#include "../vm_debug.h"

#define CAPACITY (2)
#define RESIZE (2)

DEFINE_LIST_BASE(vmp_list_inherited_by, vmp_type, CAPACITY, RESIZE);
DEFINE_LIST_BASE_FIND(vmp_list_inherited_by, vmp_type, name);

BOOL vmp_list_inherited_by_contains(const vmp_list_inherited_by* l, const struct vmp_type* ptr)
{
	struct vmp_type** item = &l->memory[0];
	struct vmp_type** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		if (*item == ptr) {
			return TRUE;
		}
	}
	return FALSE;
}
