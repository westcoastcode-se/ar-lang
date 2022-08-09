#ifndef _VMCODE_LISTS_H_
#define _VMCODE_LISTS_H_

#include "../vm_list.h"

DECLARE_LIST_TYPE(vmcd_list_packages, vmcd_package);
DECLARE_LIST_TYPE_FIND(vmcd_list_packages, vmcd_package);

DECLARE_LIST_TYPE(vmcd_list_imports, vmcd_package);
DECLARE_LIST_TYPE_FIND(vmcd_list_imports, vmcd_package);

DECLARE_LIST_TYPE(vmcd_list_funcs, vmcd_func);
DECLARE_LIST_TYPE_FIND(vmcd_list_funcs, vmcd_func);

DECLARE_LIST_TYPE(vmcd_list_types, vmcd_type);
DECLARE_LIST_TYPE_FIND(vmcd_list_types, vmcd_type);

DECLARE_LIST_TYPE(vmcd_list_inherits_from, vmcd_type);
DECLARE_LIST_TYPE_FIND(vmcd_list_inherits_from, vmcd_type);

DECLARE_LIST_TYPE(vmcd_list_inherited_by, vmcd_type);
DECLARE_LIST_TYPE_FIND(vmcd_list_inherited_by, vmcd_type);

#endif
