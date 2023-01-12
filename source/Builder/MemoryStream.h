#pragma once

#include "../Common.h"
#include "LinkError.h"

namespace WestCoastCode::Builder
{
	// Memory stream for writing bytecode
	class MemoryStream
	{
	public:
		MemoryStream(I32 totalSize);
		
		~MemoryStream();

		// Write data
		void Write(const void* ptr, I32 size);

		// Write data
		template<class T>
		void Write(const T* ptr) {
			Write(ptr, sizeof(T));
		}

		void Write(ReadOnlyString s) {
			Write(s.data(), (I32)s.length());
		}

		void Write(const String& s) {
			Write(s.data(), (I32)s.length());
		}

		// Get the bytes and return it. This will give away ownership of the memory to
		// the caller.
		Bytes Done();

		// Get the memory at the supplied location
		Bytes GetAt(I32 offset) {
			return _bytes + offset;
		}

		// Get the current offset
		I32 GetOffset() const {
			return (I32)(_ptr - _bytes);
		}

		// Reserve memory
		template<class T>
		T* Reserve() {
			if (_ptr + sizeof(T) > _end)
				throw LinkErrorInvalidBytecodeSize();
			Bytes temp = _ptr;
			_ptr += sizeof(T);
			return (T*)temp;
		}

		Bytes Reserve(I32 bytes) {
			if (_ptr + bytes > _end)
				throw LinkErrorInvalidBytecodeSize();
			Bytes temp = _ptr;
			_ptr += bytes;
			return temp;
		}

	private:
		Bytes _bytes;
		Bytes _ptr;
		const Bytes _end;
	};
}
