#pragma once

#include <type_traits>

namespace WestCoastCode
{
	template<class T, int Resize> class Array;
	template<class T> class ReadOnlyArray;

	// Class which holds arrays of classes togehter in the same manner as the std::vector. 
	// It has, however, support for exposing itself as a ReadOnly view of their interface
	template<class T, int Resize = 4>
	class Array
	{
		friend class ReadOnlyArray<T>;

	public:
		Array()
			: _capacity(Resize), _memory((T*)malloc(sizeof(T) * _capacity)), _size(0) {
			assert(_memory != nullptr);
		}

		~Array()
		{
			free(_memory);
		}

		// Get the size of the array
		I32 Size() const { return _size; }

		// Add a new item to the array
		void Insert(T value)
		{
			// Resize if neccessary
			if (_size == _capacity) {
				_capacity += Resize;
				_memory = (T*)realloc(_memory, sizeof(T) * _capacity);
			}
			_memory[_size++] = value;
		}
		
		// Put the supplied pointer at the index
		void PutAt(T value, I32 idx)
		{
			assert(_capacity < idx);
			_memory[idx] = value;
		}

		// Remove the item at the supplied index
		void RemoveAt(I32 idx)
		{
			assert(_capacity > idx);
			assert(_size > idx);
			if (idx > _size)
				return;

			// Move all items backwards one step
			for (I32 i = idx + 1; i < _size; ++i) {
				_memory[i - 1] = _memory[i];
			}
			_size--;
		}

		// Remove the supplied item
		void Remove(T value)
		{
			for (I32 i = 0; i < _size; ++i)
			{
				if (_memory[i] == value) {
					RemoveAt(i);
					return;
				}
			}
		}

		T* Ptr() { return _memory; }

		const T* Ptr() const { return _memory; }

	public:
		T& operator[](I32 idx) { return _memory[idx]; }
		const T& operator[](I32 idx) const { return _memory[idx]; }

	private:
		int _capacity;
		T* _memory;
		int _size;
	};

	// A view of the Array object but exposes the implementations as interfaces
	template<class T>
	class ReadOnlyArray
	{
	private:
		T* _memory;
		const I32 _size;

	public:
		ReadOnlyArray()
			: _memory(nullptr), _size(0) {}

		template<class Class>
		ReadOnlyArray(const Array<Class>& v)
			: _memory(GetMemory<Class>(v)), _size(GetSize<Class>(v))
		{
		}

		ReadOnlyArray(const Array<T>& rhs)
			: _memory(rhs._memory), _size(rhs._size) {}

		// Get the size of the vector
		I32 Size() const { return _size; }

	private:
		template<class Class>
		static I32 GetSize(const Array<Class>& v)
		{
			using namespace std;
			if constexpr (is_pointer<Class>()) {
				if constexpr (is_base_of<remove_pointer<T>::type, remove_pointer<Class>::type>()) {
					return v.Size();
				}
			}
			else {
				return 0;
			}
		}

		template<class Class>
		static T* GetMemory(const Array<Class>& v)
		{
			using namespace std;
			if constexpr (is_pointer<Class>()) {
				if constexpr (is_base_of<remove_pointer<T>::type, remove_pointer<Class>::type>()) {
					Class* memory = const_cast<Array<Class>&>(v).Ptr();
					return reinterpret_cast<T*>(memory);
				}
			}
			else {
				return 0;
			}
		}

	public:
		T operator[](I32 idx) { return _memory[idx]; }
		const T operator[](I32 idx) const { return _memory[idx]; }
	};

}