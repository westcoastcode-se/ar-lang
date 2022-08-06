#include "vmp_lists.h"
#include "vmp_types.h"

DEFINE_LIST_BASE_RELEASE_ONLY(vmp_list_inherited_by, vmp_type, 2, 2);
DEFINE_LIST_FIND(vmp_list_inherited_by, vmp_type, name);

DEFINE_LIST_BASE_RELEASE_ONLY(vmp_list_inherits_from, vmp_type, 2, 2);
DEFINE_LIST_FIND(vmp_list_inherits_from, vmp_type, name);

DEFINE_LIST_BASE_DEFAULT(vmp_list_funcs, vmp_func, 8, 8);
DEFINE_LIST_FIND(vmp_list_funcs, vmp_func, name);
