#include "lists.h"
#include "symbols.h"

DEFINE_LIST_BASE_RELEASE_ONLY(zpp_inherits_from, zpp_type, 2, 2);
DEFINE_LIST_FIND(zpp_inherits_from, zpp_type, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(zpp_inherited_by, zpp_type, 2, 2);
DEFINE_LIST_FIND(zpp_inherited_by, zpp_type, header.name);

DEFINE_LIST_BASE_RELEASE_ONLY(zpp_imports, zpp_package, 4, 4);
DEFINE_LIST_FIND(zpp_imports, zpp_package, header.name);
