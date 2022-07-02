#ifndef _VMP_LIST_MARKERS_H_
#define _VMP_LIST_MARKERS_H_

#include "../vm_config.h"

// List
struct vmp_list_markers
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_marker** memory;
};
typedef struct vmp_list_markers vmp_list_markers;

// Initialize the list
extern BOOL vmp_list_markers_init(vmp_list_markers* l);

// Release the internal memory of list
extern void vmp_list_markers_release(vmp_list_markers* l);

// Add a new marker and return the index where the local exists
extern vm_int32 vmp_list_markers_add(vmp_list_markers* l, struct vmp_marker* m);

// Get a marker using the supplied index
extern struct vmp_marker* vmp_list_markers_get(const vmp_list_markers* l, vm_int32 index);

#endif
