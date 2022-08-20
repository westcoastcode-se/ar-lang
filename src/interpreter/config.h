#ifndef _VMI_CONFIG_H_
#define _VMI_CONFIG_H_

#include "../vm_config.h"

// Represents a type for position, in memory, where the virtual machine is processing instructions
typedef const vm_byte* vmi_ip;

// Offset
#define OFFSET(n) (vmi_ip)((vm_byte*)NULL + n)

#endif
