#pragma once

#include <type_traits>

namespace WestCoastCode
{
	// Class which holds an unknown number of items in an array-like structure.
	// This type is compatible with the ReadOnlyArray interface
	template<class T, int Resize = 4>
	class Vector
	{
	public:
		Vector()
			: _capacity(Resize), _memory((T*)malloc(sizeof(T)* _capacity)), _size(0) {
			assert(_memory != nullptr);
		}

		// Move Constructor
		Vector(Vector<T, Resize>&& source)
			: _capacity{source._capacity}, _memory{source._memory}, _size{source._size}
		{
			source._capacity = 0;
			source._memory = nullptr;
			source._size = 0;
		}

		Vector(int initialSize)
			: _capacity(initialSize), _memory((T*)malloc(sizeof(T)* _capacity)), _size(initialSize) {
			assert(_memory != nullptr);
		}

		template<typename ...Args>
		Vector(Args... args)
			: _capacity(sizeof...(Args)), _memory((T*)malloc(sizeof(T)* _capacity)), _size(_capacity) {
			_VarSet(0, args...);
		}

		~Vector()
		{
			if (_memory)
				free(_memory);
		}

		// Get the size of the array
		I32 Size() const { return _size; }

		// Is this container empty
		bool IsEmpty() const { return _size == 0; }

		// Add a new item to the array
		void Add(T value)
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

	public:
		typedef T* iterator;
		typedef const T* const_iterator;
		iterator begin() { return _memory; }
		const_iterator begin() const { return _memory; }
		iterator end() { return &_memory[_size]; }
		const_iterator end() const { return &_memory[_size]; }

	private:
		void _VarSet(I32 index, T first) {
			_memory[index] = first;
		}

		template<typename ...Args>
		void _VarSet(I32 index, T first, Args... args) {
			_memory[index] = first;
			_VarSet(index + 1, args...);
		}

	private:
		int _capacity;
		T* _memory;
		int _size;
	};

	// Class which holds an array of a known size. Is compatible with the ReadOnlyArray interface
	template<class T, int Count>
	class Array
	{
	public:
		Array() {}

		template<typename ...Args>
		Array(Args... args) {
			_VarSet(0, args...);
		}

		// Get the size of the array
		I32 Size() const { return Count; }

		// Set the supplied value at the supplied index
		void Set(T value, I32 idx)
		{
			assert(idx < Size && idx > 0);
			_memory[idx] = value;
		}

		T* Ptr() { return _memory; }
		const T* Ptr() const { return _memory; }

	public:
		T& operator[](I32 idx) { return _memory[idx]; }
		const T& operator[](I32 idx) const { return _memory[idx]; }

	public:
		typedef T* iterator;
		typedef const T* const_iterator;
		iterator begin() { return _memory; }
		const_iterator begin() const { return _memory; }
		iterator end() { return &_memory[Count]; }
		const_iterator end() const { return &_memory[Count]; }

	private:
		void _VarSet(I32 index, T first) {
			_memory[index] = first;
		}

		template<typename ...Args>
		void _VarSet(I32 index, T first, Args... args) {
			_memory[index] = first;
			_VarSet(index + 1, args...);
		}

	private:
		T _memory[Count];
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
		ReadOnlyArray(const Vector<Class>& v)
			: _memory(GetMemory(v)), _size(GetSize(v))
		{
		}

		template<class Class, int Size>
		ReadOnlyArray(const Array<Class, Size>& v)
			: _memory(GetMemory(v)), _size(GetSize(v))
		{
		}

		ReadOnlyArray(const Vector<T>& rhs)
			: _memory(GetMemory(rhs)), _size(GetSize(rhs)) {}

		// Get the size of the vector
		I32 Size() const { return _size; }

	private:
		template<class Class>
		static I32 GetSize(const Vector<Class>& v)
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
		static T* GetMemory(const Vector<Class>& v)
		{
			using namespace std;
			if constexpr (is_pointer<Class>()) {
				if constexpr (is_base_of<remove_pointer<T>::type, remove_pointer<Class>::type>()) {
					Class* memory = const_cast<Vector<Class>&>(v).Ptr();
					return reinterpret_cast<T*>(memory);
				}
			}
			else {
				return 0;
			}
		}

		template<class Class, int Size>
		static I32 GetSize(const Array<Class, Size>& v)
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

		template<class Class, int Size>
		static T* GetMemory(const Array<Class, Size>& v)
		{
			using namespace std;
			if constexpr (is_pointer<Class>()) {
				if constexpr (is_base_of<remove_pointer<T>::type, remove_pointer<Class>::type>()) {
					Class* memory = const_cast<Array<Class, Size>&>(v).Ptr();
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

	public:
		typedef T* iterator;
		typedef const T* const_iterator;
		iterator begin() { return _memory; }
		const_iterator begin() const { return _memory; }
		iterator end() { return &_memory[_size]; }
		const_iterator end() const { return &_memory[_size]; }
	};


}