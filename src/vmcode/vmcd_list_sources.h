#ifndef _VMCD_LIST_SOURCES_H_
#define _VMCD_LIST_SOURCES_H_

#include "../vm_string.h"

// List
struct vmcd_list_sources
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmcd_source_code** memory;
};
typedef struct vmcd_list_sources vmcd_list_sources;

// Initialize the list
extern BOOL vmcd_list_sources_init(vmcd_list_sources* l);

// Release the internal memory of list
extern void vmcd_list_sources_release(vmcd_list_sources* l);

// Add new source code and return the index where it exists
extern vm_int32 vmcd_list_sources_add(vmcd_list_sources* l, struct vmcd_source_code* ptr);

// Get an sorce code using the supplied index
extern struct vmcd_source_code* vmcd_list_sources_get(const vmcd_list_sources* l, vm_int32 index);

// Search for a source code using it's filename
extern struct vmcd_source_code* vmcd_list_sources_find(const vmcd_list_sources* l, const vm_string* name);


#endif
