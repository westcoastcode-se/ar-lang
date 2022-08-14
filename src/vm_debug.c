#include "vm_debug.h"

// Header for a memory block
struct vm_memory_tracker_header
{
	size_t size;
	const char* filename;
	vm_int32 line;
	struct vm_memory_tracker_header* prev;
	struct vm_memory_tracker_header* next;
};
typedef struct vm_memory_tracker_header vm_memory_tracker_header;

struct vm_memory_tracker
{
	vm_memory_tracker_header* first;
	vm_memory_tracker_header* last;
	size_t total_bytes;
};
typedef struct vm_memory_tracker vm_memory_tracker;

vm_memory_tracker _vm_memory_tracker = { NULL, NULL, 0 };

void vm_memory_tracker_header_init(vm_memory_tracker_header* h, size_t size, const char* filename, vm_int32 line)
{
	h->prev = h->next = NULL;
	h->size = size;
	h->filename = filename;
	h->line = line;
	if (_vm_memory_tracker.last == NULL)
		_vm_memory_tracker.last = _vm_memory_tracker.first = h;
	else {
		h->prev = _vm_memory_tracker.last;
		_vm_memory_tracker.last->next = h;
		_vm_memory_tracker.last = h;
	}
	_vm_memory_tracker.total_bytes += size;
}

void vm_memory_tracker_header_release(vm_memory_tracker_header* h)
{
	if (_vm_memory_tracker.first == h) {
		_vm_memory_tracker.first = h->next;
	}
	if (_vm_memory_tracker.last == h) {
		_vm_memory_tracker.last = h->prev;
	}
	if (h->prev != NULL) {
		h->prev->next = h->next;
	}
	if (h->next != NULL) {
		h->next->prev = h->prev;
	}
	_vm_memory_tracker.total_bytes -= h->size;
}

void vm_memory_tracker_header_resize(vm_memory_tracker_header* h, size_t new_size, const char* filename, vm_int32 line)
{
	new_size = new_size - h->size;
	h->size += new_size;
	h->filename = filename;
	h->line = line;
	_vm_memory_tracker.total_bytes += new_size;
}

// Allocate memory
void* vm_malloc1(size_t size, const char* filename, vm_int32 line)
{
	vm_byte* const ptr = malloc(size + sizeof(vm_memory_tracker_header));
	if (ptr == NULL)
		return NULL;
	vm_memory_tracker_header_init((vm_memory_tracker_header*)ptr, size, filename, line);
	return ptr + sizeof(vm_memory_tracker_header);
}

// Reallocate memory
void* vm_realloc1(void* ptr, size_t new_size, const char* filename, vm_int32 line)
{
	ptr = (vm_byte*)ptr - sizeof(vm_memory_tracker_header);
	vm_memory_tracker_header* old_header = (vm_memory_tracker_header*)ptr;
	vm_memory_tracker_header* old_prev = old_header->prev;
	vm_memory_tracker_header* old_next = old_header->next;
	vm_memory_tracker_header* new_header;
	vm_memory_tracker_header_resize(old_header, new_size, filename, line);
	new_header = (vm_memory_tracker_header*)realloc(ptr, new_size + sizeof(vm_memory_tracker_header));
	if (new_header == NULL)
		return NULL;
	if (old_prev != NULL)
		old_prev->next = new_header;
	if (old_next != NULL)
		old_next->prev = new_header;
	if (_vm_memory_tracker.first == old_header)
		_vm_memory_tracker.first = new_header;
	if (_vm_memory_tracker.last == old_header)
		_vm_memory_tracker.last = new_header;
	return (vm_byte*)new_header + sizeof(vm_memory_tracker_header);
}

// Free memory
void vm_free1(void* mem)
{
	mem = (vm_byte*)mem - sizeof(vm_memory_tracker_header);
	vm_memory_tracker_header_release((vm_memory_tracker_header*)mem);
	free(mem);
	return;
}

BOOL vm_memory_test_bytes_left()
{
	if (_vm_memory_tracker.total_bytes > 0) {
		vm_memory_tracker_header* h = _vm_memory_tracker.first;
		printf("\n===================\n");
		printf("Total allocated memory not released is: %d\n", (vm_int32)_vm_memory_tracker.total_bytes);
		while (h != NULL) {
			printf("%s@%d - %d bytes\n", h->filename, h->line, (vm_int32)h->size);
			h = h->next;
		}
		printf("===================\n");
		vm_memory_test_clear();
		return FALSE;
	}
	return TRUE;
}

void vm_memory_test_clear()
{
	_vm_memory_tracker.first = NULL;
	_vm_memory_tracker.last = NULL;
	_vm_memory_tracker.total_bytes = 0;
}
