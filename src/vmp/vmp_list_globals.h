#ifndef _VMP_LIST_GLOBALS_H_
#define _VMP_LIST_GLOBALS_H_

#include "../vm_string.h"

// List
struct vmp_list_globals
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_global** memory;
};
typedef struct vmp_list_globals vmp_list_globals;

// Initialize the list
extern BOOL vmp_list_globals_init(vmp_list_globals* l);

// Release the internal memory of list
extern void vmp_list_globals_release(vmp_list_globals* l);

// Add a new global and return the index where the global exists
extern vm_int32 vmp_list_globals_add(vmp_list_globals* l, struct vmp_global* local);

// Get an global using the supplied index
extern struct vmp_global* vmp_list_globals_get(const vmp_list_globals* l, vm_int32 index);

// Search for a global variable with the supplied name
extern struct vmp_global* vmp_list_globals_find(const vmp_list_globals* l, const vm_string* name);

#endif
