#pragma once

#include "ProcessError.h"
#include "Primitive.h"
#include "Thread.h"
#include <unordered_map>

namespace WestCoastCode::Interpreter
{
	class Function;
	class Type;
	class Global;

	class Package
	{
	public:
		Package(ReadOnlyString name, ReadOnlyString signature, I32 id)
			: _name(name), _signature(signature), _id(id) {}

		~Package();

		// Get the name for this package
		ReadOnlyString GetName() const { return _name; }

		// Get the signature for this package
		ReadOnlyString GetSignature() const { return _signature; }

		// Search for a function given a name
		const Function* FindFunction(ReadOnlyString name) const;

		// Add the supplied function
		void AddFunction(Function* function) {
			_functions.push_back(function);
		}

	private:
		const ReadOnlyString _name;
		const ReadOnlyString _signature;
		const I32 _id;

		// All functions
		Vector<Function*> _functions;
		Vector<Type*> _types;
		Vector<Global*> _globals;
	};

	// Represents a function that we can call
	class Function
	{
	public:
		Function(ReadOnlyString name, ReadOnlyString signature, I32 id, const Byte* address,
			I32 stackSize)
			: _name(name), _signature(signature), _id(id), _address(address), _stackSize(stackSize) {}

		// Get the name for this function
		ReadOnlyString GetName() const { return _name; }

		// Get the signature for this function
		ReadOnlyString GetSignature() const { return _signature; }

		// Get the entrypoint address
		const Byte* GetAddress() const { return _address; }

		// How many bytes are expected to be pushed on the stack for this function to work
		I32 GetStackSize() const { return _stackSize; }

	private:
		const ReadOnlyString _name;
		const ReadOnlyString _signature;
		const I32 _id;
		const Byte* _address;
		const I32 _stackSize;
	};

	// Represents a type
	class Type
	{
	public:
		Type(ReadOnlyString name, ReadOnlyString signature, I32 id)
			: _name(name), _signature(signature), _id(id) {}

		// Get the name for this type
		ReadOnlyString GetName() const { return _name; }

		// Get the signature for this type
		ReadOnlyString GetSignature() const { return _signature; }

	private:
		const ReadOnlyString _name;
		const ReadOnlyString _signature;
		const I32 _id;
	};

	// Represents a global
	class Global
	{
	public:
		Global(ReadOnlyString name, ReadOnlyString signature, I32 id, void* memory)
			: _name(name), _signature(signature), _id(id), _memory(memory) {}

		// Get the name for this global
		ReadOnlyString GetName() const { return _name; }

		// Get the signature for this global
		ReadOnlyString GetSignature() const { return _signature; }

		// Get the memory of this global
		void* Memory() { return _memory; }

		// Get the memory of this global
		template<typename T>
		T* MemoryAs() { return (T*)_memory; }

	private:
		const ReadOnlyString _name;
		const ReadOnlyString _signature;
		const I32 _id;
		void* const _memory;
	};

	// Process running the virtual machine
	class Process
	{
	public:
		Process();

		~Process();

		// Load the supplied bytecode. An exception will be raised if the supplied bytecode is invalid. Set the "deleteOnExit"
		// to false if you do not want the process to delete the bytecode when done
		void Load(Bytes bytecode, bool deleteOnExit = true);

		// Create a new thread
		Thread* NewThread();

		// Execute the function using the new thread. It is expected that you've already pushed
		// the required arguments onto the stack before calling this
		void Exec(Thread* thread, const Function* entryPoint);

		// Find a package with the supplied signature
		const Package* FindPackage(ReadOnlyString signature) const;

		// Find a function with the supplied signature
		const Function* FindFunction(ReadOnlyString signature) const;

		// Find a global with the supplied signature
		const Global* FindGlobal(ReadOnlyString signature) const;

		// Get the execution range
		Range GetExecutionRange() const noexcept {
			return Range(_bytecodeExecutionStart, _bytecodeExecutionEnd);
		}

	private:
		friend class Thread;

		// Get the bytecode
		Byte* GetBytecode() { return _bytecode; }

	private:
		Byte* _bytecode;
		bool _bytecodeDelete;

		// Range where execution is allowed
		const Byte* _bytecodeExecutionStart;
		const Byte* _bytecodeExecutionEnd;

		Thread* _currentThread;
		Stack<Thread*> _pausedThreads;

		// All packages
		Vector<Package*> _packages;
		std::unordered_map<ReadOnlyString, const Package*> _namedPackages;

		// All types
		Vector<Type*> _types;
		std::unordered_map<ReadOnlyString, const Type*> _namedTypes;

		// All functions
		Vector<Function*> _functions;
		std::unordered_map<ReadOnlyString, const Function*> _namedFunctions;

		// All globals
		Vector<Global*> _globals;
		std::unordered_map<ReadOnlyString, const Global*> _namedGlobals;
	};
}
