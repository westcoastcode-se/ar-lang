#include "vmp_list_markers.h"
#include "vmp_types.h"
#include "vmp_debug.h"

#define CAPACITY (4)
#define RESIZE (4)

BOOL vmp_list_markers_init(vmp_list_markers* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (struct vmp_marker**)vmp_malloc(sizeof(vmp_marker*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_markers_release(vmp_list_markers* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_marker* const a = l->memory[i];
		vmp_marker_free(a);
	}

	vmp_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_markers_add(vmp_list_markers* l, vmp_marker* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_marker**)vmp_realloc(l->memory, sizeof(vmp_marker*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_marker* vmp_list_markers_get(const vmp_list_markers* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}
