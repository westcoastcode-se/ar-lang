#pragma once

#include "Common.h"
#include <memory>

namespace WestCoastCode
{
	class ARLANG_API IMemoryTracked;

	// Memory tracking utility
	class ARLANG_API MemoryTracker
	{
	public:
		// Start memory tracking of all types inheriting from IMemoryTracked
		static void Begin();

		// End memory tracking. This will return false if memory leaks are found
		static bool End();

		// Initialize the tracked memory
		static void Add(IMemoryTracked* ptr);

		// Release the tracked memory
		static void Remove(IMemoryTracked* ptr);
	};

	// Inherit from this class if you want to have memory tracking for the sub-type. Using this will also require
	// that the developer sets the ARLANG_MEMORY_TRACKING define
	class ARLANG_API IMemoryTracked
	{
	public:
		virtual ~IMemoryTracked() {}

#if defined(ARLANG_MEMORY_TRACKING)
		friend class MemoryTracker;

		static void* operator new(std::size_t size, const char* filename, int line);
		static void operator delete(void* p, const char* filename, int line);
		static void operator delete(void* p);
#endif

	private:
		const char* _filename;
		I32 _line;
		I32 _size;
		IMemoryTracked* _head;
		IMemoryTracked* _tail;
	};

}

#if defined(ARLANG_MEMORY_TRACKING)
#define ARLANG_NEW new (__FILE__, __LINE__)
#else
#define ARLANG_NEW new
#endif
