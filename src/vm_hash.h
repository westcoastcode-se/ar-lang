#ifndef _VM_HASH_H_
#define _VM_HASH_H_

#include "vm_config.h"

// Give an initialized hash value
extern vm_hash32 vm_hash32_init();

// Increment the hash value using a character
extern vm_hash32 vm_hash32_next(vm_hash32 hash, char c);

// Calculate a hash based on the supplied string
extern vm_hash32 vm_hash32_from_string(const char* s, const char* end);

#endif
