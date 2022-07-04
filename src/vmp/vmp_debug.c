#include "vmp_debug.h"

// Header for a memory block
struct vmp_memory_header
{
	size_t size;
	const char* filename;
	vm_int32 line;
	struct vmp_memory_header* prev;
	struct vmp_memory_header* next;
};
typedef struct vmp_memory_header vmp_memory_header;

struct vmp_memory_manager
{
	vmp_memory_header* first;
	vmp_memory_header* last;
	size_t total_bytes;
};
typedef struct vmp_memory_manager vmp_memory_manager;

vmp_memory_manager _vmp_memory_manager = {NULL, NULL, 0};

void _vmp_memory_header_init(vmp_memory_header* h, size_t size, const char* filename, vm_int32 line)
{
	h->prev = h->next = NULL;
	h->size = size;
	h->filename = filename;
	h->line = line;
	if (_vmp_memory_manager.last == NULL)
		_vmp_memory_manager.last = _vmp_memory_manager.first = h;
	else {
		h->prev = _vmp_memory_manager.last;
		_vmp_memory_manager.last->next = h;
		_vmp_memory_manager.last = h;
	}
	_vmp_memory_manager.total_bytes += size;
}

void _vmp_memory_header_release(vmp_memory_header* h)
{
	if (_vmp_memory_manager.first == h) {
		_vmp_memory_manager.first = h->next;
	}
	if (_vmp_memory_manager.last == h) {
		_vmp_memory_manager.last = h->prev;
	}
	if (h->prev != NULL) {
		h->prev->next = h->next;
	}
	if (h->next != NULL) {
		h->next->prev = h->prev;
	}
	_vmp_memory_manager.total_bytes -= h->size;
}

void _vmp_memory_header_resize(vmp_memory_header* h, size_t new_size, const char* filename, vm_int32 line)
{
	new_size = new_size - h->size;
	h->size += new_size;
	h->filename = filename;
	h->line = line;
	_vmp_memory_manager.total_bytes += new_size;
}

// Allocate memory
void* vmp_malloc1(size_t size, const char* filename, vm_int32 line)
{
	vm_byte* const ptr = malloc(size + sizeof(vmp_memory_header));
	if (ptr == NULL)
		return NULL;
	_vmp_memory_header_init((vmp_memory_header*)ptr, size, filename, line);
	return ptr + sizeof(vmp_memory_header);
}

// Reallocate memory
void* vmp_realloc1(void* ptr, size_t new_size, const char* filename, vm_int32 line)
{
	ptr = (vm_byte*)ptr - sizeof(vmp_memory_header);
	vmp_memory_header* old_header = (vmp_memory_header*)ptr;
	vmp_memory_header* old_prev = old_header->prev;
	vmp_memory_header* old_next = old_header->next;
	vmp_memory_header* new_header;
	_vmp_memory_header_resize(old_header, new_size, filename, line);
	new_header = (vmp_memory_header*)realloc(ptr, new_size + sizeof(vmp_memory_header));
	if (new_header == NULL)
		return NULL;
	if (old_prev != NULL)
		old_prev->next = new_header;
	if (old_next != NULL)
		old_next->prev = new_header;
	if (_vmp_memory_manager.first == old_header)
		_vmp_memory_manager.first = new_header;
	if (_vmp_memory_manager.last == old_header)
		_vmp_memory_manager.last = new_header;
	return (vm_byte*)new_header + sizeof(vmp_memory_header);
}

// Free memory
void vmp_free1(void* mem)
{
	mem = (vm_byte*)mem - sizeof(vmp_memory_header);
	_vmp_memory_header_release((vmp_memory_header*)mem);
	free(mem);
	return;
}

BOOL vmp_memory_test_bytes_left()
{
	if (_vmp_memory_manager.total_bytes > 0) {
		vmp_memory_header* h = _vmp_memory_manager.first;
		printf("\n===================\n");
		printf("Total allocated memory not released is: %d\n", (vm_int32)_vmp_memory_manager.total_bytes);
		while (h != NULL) {
			printf("%s@%d - %d bytes\n", h->filename, h->line, (vm_int32)h->size);
			h = h->next;
		}
		printf("===================\n");
		_vmp_memory_manager.first = NULL;
		_vmp_memory_manager.last = NULL;
		_vmp_memory_manager.total_bytes = 0;
		return FALSE;
	}
	return TRUE;
}
