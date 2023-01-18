#pragma once

#include "MemoryStream.h"
#include "../Common.h"
#include "../Interpreter/Instructions.h"
#include "../Primitive.h"

namespace WestCoastCode::Builder
{
	class IType;
	class IFunction;

	// Definition of an instruction
	class ARLANG_API Instr
	{
	public:
		Instr() : _offset(-1) {}

		virtual ~Instr() {}

		// Get the size of this instruction
		virtual I32 GetSize() const = 0;

		// Get the offset where this instruction is located
		I32 GetOffset() const { return _offset; }

		// Set the offset for this instruction
		void SetOffset(I32 offset) { _offset = offset; }

		// Write the instruction to the supplied memory stream
		virtual void Write(MemoryStream& s) = 0;

	private:
		I32 _offset;
	};

	template<class T>
	class ARLANG_API TInstr : public Instr
	{
	public:
		TInstr() { data.opcode = 0; }

		I32 GetSize() const final { return sizeof(T); }
		
		T data;
	};

	template<typename T>
	class ARLANG_API InstrWithType : public TInstr<T>
	{
	public:
		InstrWithType(const IType* type) 
			: type(type) {
		}
		
		const IType* const type;
	};

	class ARLANG_API InstrAdd : public InstrWithType<Interpreter::InstrAdd>
	{
	public:
		InstrAdd(const IType* type) : InstrWithType(type) {}

		void Write(MemoryStream& s) final;
	};

	class ARLANG_API InstrSub : public InstrWithType<Interpreter::InstrSub>
	{
	public:
		InstrSub(const IType* type) : InstrWithType(type) {}

		void Write(MemoryStream& s) final;
	};

	class ARLANG_API InstrMult : public InstrWithType<Interpreter::InstrMult>
	{
	public:
		InstrMult(const IType* type) : InstrWithType(type) {}

		void Write(MemoryStream& s) final;
	};

	class ARLANG_API InstrDiv : public InstrWithType<Interpreter::InstrDiv>
	{
	public:
		InstrDiv(const IType* type) : InstrWithType(type) {}

		void Write(MemoryStream& s) final;
	};

	class ARLANG_API InstrNeg : public InstrWithType<Interpreter::InstrNeg>
	{
	public:
		InstrNeg(const IType* type) : InstrWithType(type) {}

		void Write(MemoryStream& s) final;
	};

	class ARLANG_API InstrBitNot : public InstrWithType<Interpreter::InstrBitNot>
	{
	public:
		InstrBitNot(const IType* type) : InstrWithType(type) {}

		void Write(MemoryStream& s) final;
	};

	class ARLANG_API InstrRet : public TInstr<Interpreter::InstrRet>
	{
	public:
		InstrRet(const IFunction* f) : function(f) {}

		void Write(MemoryStream& s) final;

		const IFunction* const function;
	};

	class ARLANG_API InstrLdc : public Instr
	{
	public:
		InstrLdc(const IType* stackType, const PrimitiveValue& value)
			: stackType(stackType), value(value) {}

		I32 GetSize() const final;

		void Write(MemoryStream& s) final;

		const IType* stackType;
		PrimitiveValue value;
	};

	class ARLANG_API InstrMarker
	{
	public:
		InstrMarker()
			: _instr(nullptr) {}

		// Get the instruction offset where this marker is located in the bytecode
		// This is useful for when performing jumps
		I32 GetOffset() { return _instr->GetOffset(); }

		// Det the instruction which this marker points towards
		void SetInstr(Instr* instr);

	private:
		Instr* _instr;
	};
}
