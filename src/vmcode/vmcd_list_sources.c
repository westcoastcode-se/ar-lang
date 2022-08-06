#include "vmcd_list_sources.h"
#include "vmcd_source_code.h"

#define CAPACITY (4)
#define RESIZE (4)

DEFINE_LIST_BASE_INIT(vmcd_list_sources, vmcd_source_code, CAPACITY);
DEFINE_LIST_BASE_RELEASE_DESTROY(vmcd_list_sources, vmcd_source_code);
DEFINE_LIST_BASE_ADD(vmcd_list_sources, vmcd_source_code, RESIZE);
DEFINE_LIST_BASE_GET(vmcd_list_sources, vmcd_source_code);
DEFINE_LIST_BASE_FIND(vmcd_list_sources, vmcd_source_code, filename);
