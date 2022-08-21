#include "lists.h"
#include "process.h"

DEFINE_LIST_BASE_DEFAULT(vmi_list_packages, vmi_package, 8, 8);
DEFINE_LIST_FIND(vmi_list_packages, vmi_package, name);

DEFINE_LIST_BASE_DEFAULT(vmi_list_functions, vmi_function, 8, 8);
DEFINE_LIST_FIND(vmi_list_functions, vmi_function, name);
