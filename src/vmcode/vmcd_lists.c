#include "vmcd_lists.h"
#include "vmcd_types.h"

DEFINE_LIST_BASE_DEFAULT(vmcd_list_packages, vmcd_package, 4, 4);
DEFINE_LIST_FIND(vmcd_list_packages, vmcd_package, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(vmcd_list_imports, vmcd_package, 4, 4);
DEFINE_LIST_FIND(vmcd_list_imports, vmcd_package, header.name);

DEFINE_LIST_BASE_DEFAULT(vmcd_list_funcs, vmcd_func, 4, 4);
DEFINE_LIST_FIND(vmcd_list_funcs, vmcd_func, header.name);

DEFINE_LIST_BASE_DEFAULT(vmcd_list_types, vmcd_type, 4, 4);
DEFINE_LIST_FIND(vmcd_list_types, vmcd_type, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(vmcd_list_inherits_from, vmcd_type, 2, 2);
DEFINE_LIST_FIND(vmcd_list_inherits_from, vmcd_type, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(vmcd_list_inherited_by, vmcd_type, 2, 2);
DEFINE_LIST_FIND(vmcd_list_inherited_by, vmcd_type, header.name);
