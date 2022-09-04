#include "lists.h"
#include "../arProcess.h"

DEFINE_LIST_BASE_DEFAULT(vmi_list_packages, arPackage, 8, 8);
DEFINE_LIST_FIND(vmi_list_packages, arPackage, name);

DEFINE_LIST_BASE_DEFAULT(vmi_list_functions, arFunction, 8, 8);
DEFINE_LIST_FIND(vmi_list_functions, arFunction, name);
