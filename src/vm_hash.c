#include "vm_hash.h"

vm_hash32 vm_hash32_init() {
	return 5381;
}

vm_hash32 vm_hash32_next(vm_hash32 hash, char c) {
	// Based on http://www.cse.yorku.ca/~oz/hash.html
	return ((hash << 5) + hash) + c; /* hash * 33 + c */
}

vm_hash32 vm_hash32_from_string(const char* s, const char* end) {
	vm_hash32 hash = vm_hash32_init();
	for (; s != end; ++s)
		hash = vm_hash32_next(hash, *s);
	return hash;
}
