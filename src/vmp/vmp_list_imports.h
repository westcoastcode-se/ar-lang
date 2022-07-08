#ifndef _VMP_LIST_IMPORTS_H_
#define _VMP_LIST_IMPORTS_H_

#include "../vm_string.h"

// List
struct vmp_list_imports
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_package** memory;
};
typedef struct vmp_list_imports vmp_list_imports;

// Initialize the list
extern BOOL vmp_list_imports_init(vmp_list_imports* l);

// Release the internal memory of list
extern void vmp_list_imports_release(vmp_list_imports* l);

// Add the supplied package
extern vm_int32 vmp_list_imports_add(vmp_list_imports* l, struct vmp_package* ptr);

// Search for a package with the supplied name
extern struct vmp_package* vmp_list_imports_find(const vmp_list_imports* l, const vm_string* name);

// Get a package of a specific index
extern struct vmp_package* vmp_list_imports_get(const vmp_list_imports* l, vm_int32 index);

// Get the first package of a specific index
extern struct vmp_package* vmp_list_imports_first(const vmp_list_imports* l);

#endif
