#ifndef _VMP_LIST_PACKAGES_H_
#define _VMP_LIST_PACKAGES_H_

#include "../vm_string.h"

// List
struct vmp_list_packages
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_package** memory;
};
typedef struct vmp_list_packages vmp_list_packages;

// Initialize the list
extern BOOL vmp_list_packages_init(vmp_list_packages* l);

// Release the internal memory of list
extern void vmp_list_packages_release(vmp_list_packages* l);

// Add the supplied package
extern vm_int32 vmp_list_packages_add(vmp_list_packages* l, struct vmp_package* ptr);

// Search for a package with the supplied name
extern struct vmp_package* vmp_list_packages_find(const vmp_list_packages* l, const vm_string* name);

// Get a package of a specific index
extern struct vmp_package* vmp_list_packages_get(const vmp_list_packages* l, vm_int32 index);

// Get the first package of a specific index
extern struct vmp_package* vmp_list_packages_first(const vmp_list_packages* l);

#endif
