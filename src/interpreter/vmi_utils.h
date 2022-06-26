#ifndef _VMI_UTILS_H_
#define _VMI_UTILS_H_

#include "../vm_config.h"

// Copy the bytes found in the source memory location and put the result into the destination location
static vm_byte* vmi_copy_bytes(vm_byte* dest, const vm_byte* src, vm_int32 size) {
	const vm_byte* const end = dest + size;
	for (; dest != end; ++dest, ++src) {
		*dest = *src;
	}
	return dest;
}

#endif
