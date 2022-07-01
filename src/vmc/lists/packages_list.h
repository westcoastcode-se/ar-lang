#ifndef _PACKAGES_LIST_H_
#define _PACKAGES_LIST_H_

#include "../../vm_string.h"

#define VMC_PACKAGES_LIST_CAPACITY (8)
#define VMC_PACKAGES_LIST_RESIZE (8)

// List containing import statements
struct vmc_packages_list
{
	struct vmc_package** memory;
	vm_int32 count;
	vm_int32 capacity;
};
typedef struct vmc_packages_list vmc_packages_list;

// Initialize list
extern BOOL vmc_packages_list_init(vmc_packages_list* l);

// Destroy the internal properties of the supplied list
extern void vmc_packages_list_destroy(vmc_packages_list* l);

// Add the supplied package
extern vm_int32 vmc_packages_list_add(vmc_packages_list* l, struct vmc_package* ptr);

// Search for a package with the supplied name
extern struct vmc_package* vmc_packages_list_find(vmc_packages_list* l, const vm_string* name);

#endif
