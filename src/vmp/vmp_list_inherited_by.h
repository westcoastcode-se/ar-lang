#ifndef _VMP_LIST_INHERITED_BY_H_
#define _VMP_LIST_INHERITED_BY_H_

#include "../vm_string.h"
#include "../vm_list.h"

DECLARE_LIST_TYPE(vmp_list_inherited_by, vmp_type);
DECLARE_LIST_TYPE_FIND(vmp_list_inherited_by, vmp_type);

// check to see if the supplied list contains the supplied item
extern BOOL vmp_list_inherited_by_contains(const vmp_list_inherited_by* l, const struct vmp_type* ptr);

#endif
