#ifndef _VMI_LISTS_H_
#define _VMI_LISTS_H_

#include "../arList.h"

DECLARE_LIST_TYPE(vmi_list_packages, arPackage);
DECLARE_LIST_TYPE_FIND(vmi_list_packages, arPackage);

DECLARE_LIST_TYPE(vmi_list_functions, arFunction);
DECLARE_LIST_TYPE_FIND(vmi_list_functions, arFunction);

#endif
