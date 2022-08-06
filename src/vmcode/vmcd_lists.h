#ifndef _VMCODE_LISTS_H_
#define _VMCODE_LISTS_H_

#include "../vm_list.h"

DECLARE_LIST_TYPE(vmcd_list_packages, vmcd_package);
DECLARE_LIST_TYPE_FIND(vmcd_list_packages, vmcd_package);

DECLARE_LIST_TYPE(vmcd_list_funcs, vmcd_func);
DECLARE_LIST_TYPE_FIND(vmcd_list_funcs, vmcd_func);

DECLARE_LIST_TYPE(vmcd_list_args, vmcd_arg);
DECLARE_LIST_TYPE_FIND(vmcd_list_args, vmcd_arg);

DECLARE_LIST_TYPE(vmcd_list_returns, vmcd_return);
DECLARE_LIST_TYPE_FIND(vmcd_list_returns, vmcd_returns);

DECLARE_LIST_TYPE(vmcd_list_types, vmcd_types);
DECLARE_LIST_TYPE_FIND(vmcd_list_types, vmcd_types);

#endif
