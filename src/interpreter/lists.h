#ifndef _VMI_LISTS_H_
#define _VMI_LISTS_H_

#include "../vm_list.h"

DECLARE_LIST_TYPE(vmi_list_packages, vmi_package);
DECLARE_LIST_TYPE_FIND(vmi_list_packages, vmi_package);

DECLARE_LIST_TYPE(vmi_list_functions, vmi_function);
DECLARE_LIST_TYPE_FIND(vmi_list_functions, vmi_function);

#endif
