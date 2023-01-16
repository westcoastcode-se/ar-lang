#pragma once

#include "MemoryStream.h"
#include "Instructions.h"
#include "../Common.h"
#include "../Array.h"
#include "../Primitive.h"

namespace WestCoastCode::Builder
{
	class IGlobal;
	class IPackage;
	class IFunction;
	class IType;

	// Interface for symbols
	class ARLANG_API ISymbol
	{
	public:
		virtual ~ISymbol() {}

		// Get the parent symbol
		virtual ISymbol* GetParent() = 0;

		// Serialize this symbol into the memory stream
		virtual void SerializeHeader(MemoryStream& stream) = 0;

		// Serialize this symbol into the memory stream
		virtual void Serialize(MemoryStream& stream) = 0;

		// Serialize read-only memory
		virtual void SerializeReadOnly(MemoryStream& stream) = 0;

		// Get the signature of this symbol
		virtual const ReadOnlyString GetSignature() const = 0;
	};

	// Interface for packages
	class ARLANG_API IPackage : public ISymbol
	{
	public:
		// Get the signature of this symbol
		virtual const ReadOnlyString GetSignature() const = 0;

		// Get all functions part of this package
		virtual ReadOnlyArray<IFunction*> GetFunctions() const = 0;

		// Get all globals part of this package
		virtual ReadOnlyArray<IGlobal*> GetGlobals() const = 0;

		// Get all types part of this package
		virtual ReadOnlyArray<IType*> GetTypes() const = 0;
	};

	// Interface for a function
	class ARLANG_API IFunction : public ISymbol
	{
	public:
		// Get the amount of bytes the argument values require
		virtual I32 GetArgumentsSize() const = 0;

		// Get the amount of bytes the return values require
		virtual I32 GetReturnsSize() const = 0;
	};

	enum class ARLANG_API TypeFlag
	{
		Void = 1 << 0,
		Primitive = 1 << 1,
		Ptr = 1 << 2,
		Array = 1 << 3,
		Complex = 1 << 4
	};
	typedef I32 TypeFlags;

	// Interface for type
	class ARLANG_API IType : public ISymbol
	{
	public:
		// Get the offset where the type information is located
		virtual I32 GetOffset() const = 0;

		// Get the size of this type, in bytes, that this type takes on the stack
		virtual I32 GetStackSize() const = 0;

		// Get flags associated with this type
		virtual TypeFlags GetFlags() const = 0;
		
		// Get the primitive type, if one is applicable
		virtual PrimitiveType GetPrimitiveType() const = 0;
	};

	class ARLANG_API IGlobal : public ISymbol
	{
	public:
		// Get the offset where this global is located
		virtual I32 GetOffset() const = 0;

		// Get the size of this global variable
		virtual I32 GetStackSize() const = 0;

		// Get the underlying type that this global is
		virtual IType* GetType() = 0;
	};

	class Type;
	class Global;
	class Function;

	// Represents a package
	class ARLANG_API Package : public IPackage
	{
	public:
		// The name of the package
		Package(ReadOnlyString name)
			: _parent(nullptr), _name(name), _signature(), _offset(-1) {}

		~Package();

		// Set the parent symbol
		void SetParent(ISymbol* parent) { _parent = parent; }

		// Add the supplied function
		Function* Add(Function* function);

		// Add the supplied type
		Type* Add(Type* type);

		// Add the supplied global
		Global* Add(Global* global);

		// Resolve header memory size
		I32 ResolveHeaderMemory(I32 offset);

		// Resolve memory which is not read-only. This includes, for example,
		// global variables. Returns the current offset
		I32 ResolveGlobalVariables(I32 offset);

		// Resolve memory which is part of the read-only block. This includes, for example,
		// the function byte-code instructions. Returns the current offset
		I32 ResolveFunctionBody(I32 offset);

	public:
		// Inherited via ISymbolPackage
		ISymbol* GetParent() final { return _parent; }
		void SerializeHeader(MemoryStream& stream) final;
		void Serialize(MemoryStream& stream) final;
		void SerializeReadOnly(MemoryStream& stream) final;
		const ReadOnlyString GetSignature() const final { return _signature; }
		ReadOnlyArray<IFunction*> GetFunctions() const final { return _functions; }
		ReadOnlyArray<IType*> GetTypes() const final { return _types; }
		ReadOnlyArray<IGlobal*> GetGlobals() const final { return _globals; }

	private:
		ISymbol* _parent;
		ReadOnlyString _name;
		String _signature;
		Vector<IFunction*> _functions;
		Vector<IType*> _types;
		Vector<IGlobal*> _globals;
		I32 _offset;
	};

	class ARLANG_API Global : public IGlobal
	{
	public:
		Global(IType* type, ReadOnlyString name)
			: _parent(nullptr), _name(name), _type(type), _offset(-1) {}
		~Global();

		// Set the parent symbol
		void SetParent(ISymbol* parent) { _parent = parent; }

		// Resolve header memory size
		I32 ResolveHeaderMemory(I32 offset);

		// Resolve memory which is not read-only. This includes, for example,
		// global variables. Returns the current offset
		I32 ResolveGlobalVariables(I32 offset);

	public:
		// Inherited via ISymbolGlobal
		ISymbol* GetParent() final { return _parent; }
		virtual void SerializeHeader(MemoryStream& stream) override;
		virtual void Serialize(MemoryStream& stream) override;
		void SerializeReadOnly(MemoryStream& stream) final {}
		I32 GetStackSize() const final { return _type->GetStackSize(); }
		const ReadOnlyString GetSignature() const final { return _signature; }
		I32 GetOffset() const final { return _offset; }
		IType* GetType() final { return _type; }

	private:
		ISymbol* _parent;
		ReadOnlyString _name;
		String _signature;
		IType* _type;
		I32 _offset;
	};

	class ARLANG_API Type : public IType
	{
	public:
		Type(ReadOnlyString name, I32 size, TypeFlags flags, 
			PrimitiveType primitiveType)
			: _parent(nullptr), _name(name), _stackSize(size), _flags(flags), _primitiveType(primitiveType), _offset(-1) {}
		~Type() {}

		// Set the parent symbol
		void SetParent(ISymbol* parent) { _parent = parent; }

		// Resolve header memory size
		I32 ResolveHeaderMemory(I32 offset);

	public:
		// Inherited via ISymbolType
		ISymbol* GetParent() final { return _parent; }
		void SerializeHeader(MemoryStream& stream) final;
		void Serialize(MemoryStream& stream) final {}
		void SerializeReadOnly(MemoryStream& stream) final {}
		const ReadOnlyString GetSignature() const final { return _signature; }
		I32 GetOffset() const final { return _offset; }
		I32 GetStackSize() const final { return _stackSize; }
		TypeFlags GetFlags() const final { return _flags; }
		PrimitiveType GetPrimitiveType() const final { return _primitiveType; }

	private:
		ISymbol* _parent;
		ReadOnlyString _name;
		String _signature;
		I32 _stackSize;
		TypeFlags _flags;
		PrimitiveType _primitiveType;
		I32 _offset;
	};

	// All instructions in a specific function
	class ARLANG_API Instructions
	{
	public:
		Instructions(Function* f) : _function(f), _stackSize(0) {}

		~Instructions();

		// Add the supplied instruction
		Instr* Add(Instr* instr);

		// Register a marker
		InstrMarker* Marker();

		// Get the size of all instructions
		I32 GetSize() const { return _stackSize; }

		// We are now done writing the instructions
		void End();

		// Serialize all instructions
		void Serialize(MemoryStream& stream);

	public:
		InstrAdd* Add(Type* type);
		InstrSub* Sub(Type* type);
		InstrDiv* Div(Type* type);
		InstrMult* Mult(Type* type);

		InstrLdc* Ldc(Type* stackType, PrimitiveValue value);

		InstrRet* Ret();

	private:
		Function* const _function;
		Vector<Instr*> _instructions;
		Vector<InstrMarker*> _markers;
		I32 _stackSize;
	};

	class ARLANG_API Function : public IFunction
	{
	public:
		Function(ReadOnlyString name)
			: _parent(nullptr), _name(name), _stackSize(-1), _entrypointOffset(-1), _instructions(this) {}

		// Set an argument
		void AddArgument(Type* t);

		// Set a return
		void AddReturn(Type* t);

		// Set the parent symbol
		void SetParent(ISymbol* parent) { _parent = parent; }

		// Begin creating instructions
		Instructions& Begin() { return _instructions; }

		// Resolve header memory size
		I32 ResolveHeaderMemory(I32 offset);

		// Resolve memory which is part of the read-only block. This includes, for example,
		// the function byte-code instructions. Returns the current offset
		I32 ResolveFunctionBody(I32 offset);

	public:
		// Inherited via ISymbolFunction
		ISymbol* GetParent() final { return _parent; }
		void SerializeHeader(MemoryStream& stream) final;
		void Serialize(MemoryStream& stream) final;
		void SerializeReadOnly(MemoryStream& stream) final;
		const ReadOnlyString GetSignature() const final { return _signature; }
		I32 GetArgumentsSize() const final;
		I32 GetReturnsSize() const final;

	private:
		ISymbol* _parent;
		ReadOnlyString _name;
		String _signature;
		I32 _stackSize;
		I32 _entrypointOffset;
		Instructions _instructions;
		Vector<Type*> _arguments;
		Vector<Type*> _returns;
	};
}
