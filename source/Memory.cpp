#include "Memory.h"

using namespace WestCoastCode;

namespace
{
	struct Singleton
	{
		IMemoryTracked* first;
		IMemoryTracked* last;
		size_t totalBytes;
		bool enabled;
	};
	Singleton _singleton = { nullptr, nullptr, 0, false };
}

void MemoryTracker::Add(IMemoryTracked* h)
{
	if (_singleton.first == nullptr) {
		_singleton.first = _singleton.last = h;
	}
	else {
		h->_head = _singleton.last;
		_singleton.last->_tail = h;
		_singleton.last = h;
	}
	_singleton.totalBytes += h->_size;
}

void MemoryTracker::Remove(IMemoryTracked* h)
{
	if (_singleton.first == h) {
		_singleton.first = h->_tail;
	}
	if (_singleton.last == h) {
		_singleton.last = h->_head;
	}
	if (h->_head != nullptr) {
		h->_head->_tail = h->_tail;
	}
	if (h->_tail != NULL) {
		h->_tail->_head = h->_head;
	}
	_singleton.totalBytes -= h->_size;
}

#if defined(ARLANG_MEMORY_TRACKING)
void* IMemoryTracked::operator new(std::size_t size, const char* filename, int line)
{
	IMemoryTracked* t = (IMemoryTracked*)malloc(size);
	if (t == nullptr)
		return nullptr;
	t->_head = t->_tail = nullptr;
	t->_filename = filename;
	t->_line = line;
	t->_size = (I32)size;
	if (_singleton.enabled)
		MemoryTracker::Add(t);
	return t;
}

void IMemoryTracked::operator delete(void* p, const char* filename, int line)
{
	IMemoryTracked* t = (IMemoryTracked*)p;
	if (t->_size > 0 && _singleton.enabled)
		MemoryTracker::Remove(t);
	free(p);
}

void IMemoryTracked::operator delete(void* p)
{
	IMemoryTracked* t = (IMemoryTracked*)p;
	if (_singleton.enabled)
		MemoryTracker::Remove(t);
	free(p);
}
#endif

void MemoryTracker::Begin()
{
#if defined(ARLANG_MEMORY_TRACKING)
	_singleton.first = nullptr;
	_singleton.last = nullptr;
	_singleton.totalBytes = 0;
	_singleton.enabled = true;
#endif
}

bool MemoryTracker::End()
{
#if defined(ARLANG_MEMORY_TRACKING)
	if (_singleton.totalBytes > 0) {
		IMemoryTracked* h = _singleton.first;
		printf("\n===================\n");
		printf("Total allocated memory not released is: %d\n", (I32)_singleton.totalBytes);
		while (h != NULL) {
			printf("%s@%d - %d bytes\n", h->_filename, h->_line, (I32)h->_size);
			h = h->_tail;
		}
		printf("===================\n");
		_singleton.first = nullptr;
		_singleton.last = nullptr;
		_singleton.totalBytes = 0;
		_singleton.enabled = true;
		return false;
	}
	return true;
#else
	return true;
#endif
}
