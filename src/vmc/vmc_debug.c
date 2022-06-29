#include "vmc_debug.h"

// Header for a memory block
struct vmc_memory_header
{
	size_t size;
	const char* filename;
	vm_int32 line;
	struct vmc_memory_header* prev;
	struct vmc_memory_header* next;
};
typedef struct vmc_memory_header vmc_memory_header;

struct vmc_memory_manager
{
	vmc_memory_header* first;
	vmc_memory_header* last;
	size_t total_bytes;
};
typedef struct vmc_memory_manager vmc_memory_manager;

vmc_memory_manager _vmc_memory_manager = {NULL, NULL, 0};

void _vmc_memory_header_init(vmc_memory_header* h, size_t size, const char* filename, vm_int32 line)
{
	h->prev = h->next = NULL;
	h->size = size;
	h->filename = filename;
	h->line = line;
	if (_vmc_memory_manager.last == NULL)
		_vmc_memory_manager.last = _vmc_memory_manager.first = h;
	else {
		h->prev = _vmc_memory_manager.last;
		_vmc_memory_manager.last->next = h;
		_vmc_memory_manager.last = h;
	}
	_vmc_memory_manager.total_bytes += size;
}

void _vmc_memory_header_release(vmc_memory_header* h)
{
	if (_vmc_memory_manager.first == h) {
		_vmc_memory_manager.first = h->next;
	}
	if (_vmc_memory_manager.last == h) {
		_vmc_memory_manager.last = h->prev;
	}
	if (h->prev != NULL) {
		h->prev->next = h->next;
	}
	if (h->next != NULL) {
		h->next->prev = h->prev;
	}
	_vmc_memory_manager.total_bytes -= h->size;
}

void _vmc_memory_header_resize(vmc_memory_header* h, size_t new_size, const char* filename, vm_int32 line)
{
	new_size = new_size - h->size;
	h->size += new_size;
	h->filename = filename;
	h->line = line;
	_vmc_memory_manager.total_bytes += new_size;
}

// Allocate memory
void* vmc_malloc1(size_t size, const char* filename, vm_int32 line)
{
	vm_byte* const ptr = malloc(size + sizeof(vmc_memory_header));
	if (ptr == NULL)
		return NULL;
	_vmc_memory_header_init((vmc_memory_header*)ptr, size, filename, line);
	return ptr + sizeof(vmc_memory_header);
}

// Reallocate memory
void* vmc_realloc1(void* ptr, size_t new_size, const char* filename, vm_int32 line)
{
	ptr = (vm_byte*)ptr - sizeof(vmc_memory_header);
	vmc_memory_header* old_header = (vmc_memory_header*)ptr;
	vmc_memory_header* old_prev = old_header->prev;
	vmc_memory_header* old_next = old_header->next;
	vmc_memory_header* new_header;
	_vmc_memory_header_resize(old_header, new_size, filename, line);
	new_header = (vmc_memory_header*)realloc(ptr, new_size + sizeof(vmc_memory_header));
	if (new_header == NULL)
		return NULL;
	if (old_prev != NULL)
		old_prev->next = new_header;
	if (old_next != NULL)
		old_next->prev = new_header;
	if (_vmc_memory_manager.first == old_header)
		_vmc_memory_manager.first = new_header;
	if (_vmc_memory_manager.last == old_header)
		_vmc_memory_manager.last = new_header;
	return (vm_byte*)new_header + sizeof(vmc_memory_header);
}

// Free memory
void vmc_free1(void* mem)
{
	mem = (vm_byte*)mem - sizeof(vmc_memory_header);
	_vmc_memory_header_release((vmc_memory_header*)mem);
	free(mem);
	return;
}

BOOL vmc_memory_test_bytes_left()
{
	if (_vmc_memory_manager.total_bytes > 0) {
		vmc_memory_header* h = _vmc_memory_manager.first;
		printf("\n===================\n");
		printf("Total allocated memory not released is: %d\n", (vm_int32)_vmc_memory_manager.total_bytes);
		while (h != NULL) {
			printf("%s@%d - %d bytes\n", h->filename, h->line, (vm_int32)h->size);
			h = h->next;
		}
		printf("===================\n");
		return FALSE;
	}
	return TRUE;
}
