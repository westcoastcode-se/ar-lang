#ifndef _VMI_THREAD_DEBUG_H_
#define _VMI_THREAD_DEBUG_H_

#include "vmi_config.h"
#include "vmi_stack.h"

// Print debug information about the current instruction
extern void vmi_debug_instruction(vmi_ip ip);

// Print debug information about the stack
extern void vmi_debug_stack(const vmi_stack* s);

#endif
