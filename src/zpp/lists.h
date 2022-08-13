#ifndef _ZPP_LISTS_H_
#define _ZPP_LISTS_H_

#include "../vm_list.h"

DECLARE_LIST_TYPE(zpp_inherits_from, zpp_type);
DECLARE_LIST_TYPE_FIND(zpp_inherits_from, zpp_type);

DECLARE_LIST_TYPE(zpp_inherited_by, zpp_type);
DECLARE_LIST_TYPE_FIND(zpp_inherited_by, zpp_type);

DECLARE_LIST_TYPE(zpp_imports, zpp_package);
DECLARE_LIST_TYPE_FIND(zpp_imports, zpp_package);

#endif
