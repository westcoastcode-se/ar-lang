#include "vmcd_list_sources.h"
#include "vmcd_source_code.h"

#define CAPACITY (4)
#define RESIZE (4)

DEFINE_LIST_BASE_DEFAULT(vmcd_list_sources, vmcd_source_code, CAPACITY, RESIZE);
DEFINE_LIST_FIND(vmcd_list_sources, vmcd_source_code, filename);
