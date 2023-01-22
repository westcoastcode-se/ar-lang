#pragma once

#include "Common.h"
#include <memory>

namespace WestCoastCode
{
	// Information about the tracked memory
	struct MemoryTrackedInfo
	{
		const char* filename;
		I32 line;
		I32 size;
		MemoryTrackedInfo* head;
		MemoryTrackedInfo* tail;
	};

	// Memory tracking utility
	class ARLANG_API MemoryTracker
	{
	public:
		// Start memory tracking of all types inheriting from IMemoryTracked
		static void Begin();

		// Enable verbose memory logging
		static void Verbose();

		// End memory tracking. This will return false if memory leaks are found
		static bool End();

		// Initialize the tracked memory
		static void Add(MemoryTrackedInfo* ptr);

		// Release the tracked memory
		static void Remove(MemoryTrackedInfo* ptr);

		// Allocate memory using malloc instead of new
		static void* Malloc(std::size_t size, const char* filename, int line);

		// Free up memory
		static void Free(void* ptr);

		// Reallocate memory
		static void* Realloc(void* ptr, std::size_t size, const char* filename, int line);
	};

	// Inherit from this class if you want to have memory tracking for the sub-type. Using this will also require
	// that the developer sets the ARLANG_MEMORY_TRACKING define
	class ARLANG_API IMemoryTracked
	{
	public:
		virtual ~IMemoryTracked() = default;

#if defined(ARLANG_MEMORY_TRACKING)
		friend class MemoryTracker;

		static void* operator new(std::size_t size, const char* filename, int line);
		static void operator delete(void* p, const char* filename, int line);
		static void operator delete(void* p);

	private:
		MemoryTrackedInfo _info;
#endif
	};

}

#if defined(ARLANG_MEMORY_TRACKING)
#define ARLANG_NEW new (__FILE__, __LINE__)
#define ARLANG_MALLOC(size) WestCoastCode::MemoryTracker::Malloc(size, __FILE__, __LINE__)
#define ARLANG_REALLOC(ptr, size) WestCoastCode::MemoryTracker::Realloc(ptr, size, __FILE__, __LINE__)
#define ARLANG_FREE(ptr) WestCoastCode::MemoryTracker::Free(ptr)
#else
#define ARLANG_NEW new
#define ARLANG_MALLOC malloc
#define ARLANG_REALLOC realloc
#define ARLANG_FREE free
#endif
