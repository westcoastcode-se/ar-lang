#include "arMemory.h"

// Header for a memory block
typedef struct arDebugMemoryHeader
{
	size_t size;
	const char* filename;
	arInt32 line;
	struct arDebugMemoryHeader* prev;
	struct arDebugMemoryHeader* next;
} arDebugMemoryHeader;

struct arDebugMemoryTracker
{
	arDebugMemoryHeader* first;
	arDebugMemoryHeader* last;
	size_t total_bytes;
};
typedef struct arDebugMemoryTracker arDebugMemoryTracker;

arDebugMemoryTracker _arDebugMemoryTracker = { NULL, NULL, 0 };

void arDebugMemoryHeader_init(arDebugMemoryHeader* h, size_t size, const char* filename, arInt32 line)
{
	h->prev = h->next = NULL;
	h->size = size;
	h->filename = filename;
	h->line = line;
	if (_arDebugMemoryTracker.last == NULL)
		_arDebugMemoryTracker.last = _arDebugMemoryTracker.first = h;
	else {
		h->prev = _arDebugMemoryTracker.last;
		_arDebugMemoryTracker.last->next = h;
		_arDebugMemoryTracker.last = h;
	}
	_arDebugMemoryTracker.total_bytes += size;
}

void arDebugMemoryHeader_release(arDebugMemoryHeader* h)
{
	if (_arDebugMemoryTracker.first == h) {
		_arDebugMemoryTracker.first = h->next;
	}
	if (_arDebugMemoryTracker.last == h) {
		_arDebugMemoryTracker.last = h->prev;
	}
	if (h->prev != NULL) {
		h->prev->next = h->next;
	}
	if (h->next != NULL) {
		h->next->prev = h->prev;
	}
	_arDebugMemoryTracker.total_bytes -= h->size;
}

void arDebugMemoryHeader_resize(arDebugMemoryHeader* h, size_t new_size, const char* filename, arInt32 line)
{
	new_size = new_size - h->size;
	h->size += new_size;
	h->filename = filename;
	h->line = line;
	_arDebugMemoryTracker.total_bytes += new_size;
}

void* arDebugMemory_malloc(size_t size, const char* filename, arInt32 line)
{
	arByte* const ptr = malloc(size + sizeof(arDebugMemoryHeader));
	if (ptr == NULL)
		return NULL;
	arDebugMemoryHeader_init((arDebugMemoryHeader*)ptr, size, filename, line);
	return ptr + sizeof(arDebugMemoryHeader);
}

void* arDebugMemory_realloc(void* ptr, size_t new_size, const char* filename, arInt32 line)
{
	ptr = (arByte*)ptr - sizeof(arDebugMemoryHeader);
	arDebugMemoryHeader* old_header = (arDebugMemoryHeader*)ptr;
	arDebugMemoryHeader* old_prev = old_header->prev;
	arDebugMemoryHeader* old_next = old_header->next;
	arDebugMemoryHeader* new_header;
	arDebugMemoryHeader_resize(old_header, new_size, filename, line);
	new_header = (arDebugMemoryHeader*)realloc(ptr, new_size + sizeof(arDebugMemoryHeader));
	if (new_header == NULL)
		return NULL;
	if (old_prev != NULL)
		old_prev->next = new_header;
	if (old_next != NULL)
		old_next->prev = new_header;
	if (_arDebugMemoryTracker.first == old_header)
		_arDebugMemoryTracker.first = new_header;
	if (_arDebugMemoryTracker.last == old_header)
		_arDebugMemoryTracker.last = new_header;
	return (arByte*)new_header + sizeof(arDebugMemoryHeader);
}

void arDebugMemory_free(void* mem)
{
	mem = (arByte*)mem - sizeof(arDebugMemoryHeader);
	arDebugMemoryHeader_release((arDebugMemoryHeader*)mem);
	free(mem);
	return;
}

BOOL arDebugMemory_verify()
{
	if (_arDebugMemoryTracker.total_bytes > 0) {
		arDebugMemoryHeader* h = _arDebugMemoryTracker.first;
		printf("\n===================\n");
		printf("Total allocated memory not released is: %d\n", (arInt32)_arDebugMemoryTracker.total_bytes);
		while (h != NULL) {
			printf("%s@%d - %d bytes\n", h->filename, h->line, (arInt32)h->size);
			h = h->next;
		}
		printf("===================\n");
		arDebugMemory_clear();
		return FALSE;
	}
	return TRUE;
}

void arDebugMemory_clear()
{
	_arDebugMemoryTracker.first = NULL;
	_arDebugMemoryTracker.last = NULL;
	_arDebugMemoryTracker.total_bytes = 0;
}
