#include "Memory.h"

using namespace WestCoastCode;

namespace
{
	static const char* DELETED = "DELETED";

	struct Singleton
	{
		MemoryTrackedInfo* first;
		MemoryTrackedInfo* last;
		size_t totalBytes;
		bool enabled;
		bool verbose;
	};
	Singleton _singleton = { nullptr, nullptr, 0, false, false };
}

#include <iostream>

void MemoryTracker::Add(MemoryTrackedInfo* info)
{
	if (!_singleton.enabled)
		return;

	if (_singleton.verbose) {
		std::cout << "[" << (void*)info << "] size=+" << info->size << std::endl;
	}

	if (_singleton.first == nullptr) {
		_singleton.first = _singleton.last = info;
	}
	else {
		info->head = _singleton.last;
		_singleton.last->tail = info;
		_singleton.last = info;
	}
	_singleton.totalBytes += info->size;
}

void MemoryTracker::Remove(MemoryTrackedInfo* info)
{
	if (!_singleton.enabled)
		return;

	if (_singleton.verbose) {
		std::cout << "[" << (void*)info << "] size=-" << info->size << std::endl;
	}

	if (_singleton.first == info) {
		_singleton.first = info->tail;
	}
	if (_singleton.last == info) {
		_singleton.last = info->head;
	}
	if (info->head != nullptr) {
		info->head->tail = info->tail;
	}
	if (info->tail != nullptr) {
		info->tail->head = info->head;
	}
	_singleton.totalBytes -= info->size;
	info->filename = DELETED;
}

void* MemoryTracker::Malloc(std::size_t size, const char* filename, int line)
{
	Byte* const ptr = (Byte*)malloc(size + sizeof(MemoryTrackedInfo));
	if (ptr == nullptr)
		return nullptr;
	MemoryTrackedInfo* const info = (MemoryTrackedInfo*)ptr;
	info->head = info->tail = nullptr;
	info->filename = filename;
	info->line = line;
	info->size = (I32)size;
	MemoryTracker::Add(info);
	return ptr + sizeof(MemoryTrackedInfo);
}

void MemoryTracker::Free(void* ptr)
{
	ptr = (Byte*)ptr - sizeof(MemoryTrackedInfo);
	MemoryTracker::Remove((MemoryTrackedInfo*)ptr);
	free(ptr);
	return;
}

void* MemoryTracker::Realloc(void* ptr, std::size_t size, const char* filename, int line)
{
	ptr = (Byte*)ptr - sizeof(MemoryTrackedInfo);

	// Remove the old pointer
	MemoryTracker::Remove((MemoryTrackedInfo*)ptr);

	// Try to allocate new memory
	MemoryTrackedInfo* newMemory = (MemoryTrackedInfo*)realloc(ptr, size + sizeof(MemoryTrackedInfo));
	if (newMemory == nullptr) {
		// Re-add the old memory and return null to indicate that the realloc failed
		MemoryTracker::Add((MemoryTrackedInfo*)ptr);
		return nullptr;
	}

	// Add the new memory
	newMemory->head = newMemory->tail = nullptr;
	newMemory->filename = filename;
	newMemory->line = line;
	newMemory->size = (I32)size;
	MemoryTracker::Add(newMemory);
	return (Byte*)newMemory + sizeof(MemoryTrackedInfo);
}

#if defined(ARLANG_MEMORY_TRACKING)
void* IMemoryTracked::operator new(std::size_t size, const char* filename, int line)
{
	IMemoryTracked* t = (IMemoryTracked*)malloc(size);
	if (t == nullptr)
		return nullptr;
	MemoryTrackedInfo* const info = &t->_info;
	info->head = info->tail = nullptr;
	info->filename = filename;
	info->line = line;
	info->size = (I32)size;
	MemoryTracker::Add(info);
	return t;
}

void IMemoryTracked::operator delete(void* p, const char* filename, int line)
{
	IMemoryTracked* const t = (IMemoryTracked*)p;
	MemoryTrackedInfo* info = &t->_info;
	if (info->size > 0)
		MemoryTracker::Remove(info);
	free(p);
}

void IMemoryTracked::operator delete(void* p)
{
	IMemoryTracked* const t = (IMemoryTracked*)p;
	MemoryTracker::Remove(&t->_info);
	free(p);
}
#endif

void MemoryTracker::Begin()
{
	_singleton.first = nullptr;
	_singleton.last = nullptr;
	_singleton.totalBytes = 0;
	_singleton.enabled = true;
	_singleton.verbose = false;
}

void MemoryTracker::Verbose()
{
	_singleton.verbose = true;
}

bool MemoryTracker::End()
{
#if defined(ARLANG_MEMORY_TRACKING)
	if (_singleton.totalBytes > 0) {
		MemoryTrackedInfo* h = _singleton.first;
		printf("\n===================\n");
		printf("Total allocated memory not released is: %d\n", (I32)_singleton.totalBytes);
		while (h != NULL) {
			printf("%s@%d - [%p] %d bytes\n", h->filename, h->line, (void*)h, (I32)h->size);
			h = h->tail;
		}
		printf("===================\n");
		_singleton.first = nullptr;
		_singleton.last = nullptr;
		_singleton.totalBytes = 0;
		_singleton.enabled = true;
		_singleton.verbose = false;
		return false;
	}
	return true;
#else
	return true;
#endif
}
