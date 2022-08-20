#ifndef _VMI_LIST_PACKAGES_H_
#define _VMI_LIST_PACKAGES_H_

#include "debug.h"
#include "../vm_string.h"

// List
struct vmi_list_packages
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmi_package** memory;
};
typedef struct vmi_list_packages vmi_list_packages;

// Initialize the list
extern BOOL vmi_list_packages_init(vmi_list_packages* l);

// Release the internal memory of list
extern void vmi_list_packages_release(vmi_list_packages* l);

// Add the supplied package
extern vm_int32 vmi_list_packages_add(vmi_list_packages* l, struct vmi_package* ptr);

// Search for a package with the supplied name
extern struct vmi_package* vmi_list_packages_find(const vmi_list_packages* l, const vm_string* name);

// Get a package of a specific index
extern struct vmi_package* vmi_list_packages_get(const vmi_list_packages* l, vm_int32 index);

// Get the first package of a specific index
extern struct vmi_package* vmi_list_packages_first(const vmi_list_packages* l);


#endif
