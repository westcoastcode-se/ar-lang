#ifndef _VMP_LISTS_H_
#define _VMP_LISTS_H_

#include "../vm_list.h"

DECLARE_LIST_TYPE(vmp_list_inherited_by, vmp_type);
DECLARE_LIST_TYPE_FIND(vmp_list_inherited_by, vmp_type);

DECLARE_LIST_TYPE(vmp_list_inherits_from, vmp_type);
DECLARE_LIST_TYPE_FIND(vmp_list_inherits_from, vmp_type);

DECLARE_LIST_TYPE(vmp_list_funcs, vmp_func);
DECLARE_LIST_TYPE_FIND(vmp_list_funcs, vmp_func);



#endif
