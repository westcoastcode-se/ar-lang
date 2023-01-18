#pragma once

#include "../Common.h"
#include "../Primitive.h"

#define VMI_PROPS1_OPCODE(P) ((P) << 8)
#define VMI_PROPS2_OPCODE(P) ((P) << 16)
#define VMI_PROPS3_OPCODE(P) ((P) << 24)

namespace WestCoastCode::Interpreter
{
	typedef I32 Opcode;
	typedef I8 IncodeProps;

	static constexpr I32 Props1(I32 val) { return val << 8; }
	static constexpr I32 Props2(I32 val) { return val << 16; }
	static constexpr I32 Props3(I32 val) { return val << 24; }

	enum class Incode : I8
	{
		Ldc,
		Ldc_s,
		Ldc_l,

		Conv,

		Add,
		Sub,
		Mult,
		Div,

		Neg,

		BitNot,

		ScopeIn,
		ScopeOut,
		Ret,

		Eoe
	};

	template<typename P1, typename P2, typename P3>
	static constexpr I32 Props(Incode incode, P1 p1, P2 p2, P3 p3) {
		return ((I32)incode | Props1((I32)p1) | Props2((I32)p2) | Props3((I32)p3));
	}

	// Header for the operational code
	struct OpcodeHeader
	{
		union
		{
			Opcode opcode;
			struct
			{
				Incode incode;
				IncodeProps props1;
				IncodeProps props2;
				IncodeProps props3;
			};
		};
	};
	static_assert(sizeof(OpcodeHeader) == 4, "expected the size of the OpcodeHeader to be 4");
	
	// Header for all instructions (must be identical with OpcodeHeader)
#define OPCODE_HEADER \
union { \
	OpcodeHeader header; \
	Opcode opcode; \
	struct { \
		Incode icode; \
		IncodeProps props1; \
		IncodeProps props2; \
		IncodeProps props3; \
	};  \
}

	// A single operation. It represents an instruction with no auditional data
	struct InstrSingle
	{
		OPCODE_HEADER;
	};
	typedef InstrSingle InstrAdd;
	typedef InstrSingle InstrSub;
	typedef InstrSingle InstrMult;
	typedef InstrSingle InstrDiv;
	typedef InstrSingle InstrConv;
	typedef InstrSingle InstrNeg;
	typedef InstrSingle InstrBitNot;
	typedef InstrSingle InstrEoe;
	static_assert(sizeof(InstrSingle) == sizeof(OpcodeHeader));

	// Return statement
	struct InstrRet
	{
		OPCODE_HEADER;
#if defined(VM_STACK_DEBUG)
		I32 expected_ebp_offset;
#endif
	};
#if defined(VM_STACK_DEBUG)
	static_assert(sizeof(InstrRet) == sizeof(OpcodeHeader) + sizeof(U32));
#else
	static_assert(sizeof(InstrRet) == sizeof(OpcodeHeader), "expected the size to be the same as OpcodeHeader");
#endif

	// Shorthand instruction pushing a constant onto the stack. This instruction only allows for values of 16 bit and smaller
	struct InstrLdc_s
	{
		union
		{
			OpcodeHeader header;
			Opcode opcode;
			struct
			{
				Incode icode;
				IncodeProps props1;
				I16 i16;
			};
			struct
			{
				Incode icode;
				IncodeProps props1;
				I8 i8;
				I8 i8_0;
			};
		};
	};
	static_assert(sizeof(InstrLdc_s) == sizeof(OpcodeHeader));

	struct InstrLdc
	{
		OPCODE_HEADER;
		union
		{
			I32 i32;
			struct
			{
				I16 i16;
				I16 i16_1;
			};
			struct
			{
				I8 i8;
				I8 i8_1;
				I8 i8_2;
				I8 i8_3;
			};
			F32 f32;
#ifdef ARLANG_32BIT
			void* ptr;
#endif
		};
	};
	static_assert(sizeof(InstrLdc) == sizeof(OpcodeHeader) + sizeof(I32));

	// Instruction pushing a long constant onto the stack. This instruction only allows for values of 64 bit and smaller
#pragma pack(push, 1)
	struct InstrLdc_l
	{
		OPCODE_HEADER;
		union
		{
			I64 i64;
			F64 f64;
#ifdef ARLANG_64BIT
			void* ptr;
#endif
		};
	};
#pragma pack(pop)
	static_assert(sizeof(InstrLdc_l) == sizeof(OpcodeHeader) + sizeof(I64));

	enum class Opcodes : I32
	{
		Ldc_s_I8_0 = Props(Incode::Ldc_s, PrimitiveType::I8, 0, 0),
		Ldc_s_I8_1 = Props(Incode::Ldc_s, PrimitiveType::I8, 1, 0),
		Ldc_s_U8_0 = Props(Incode::Ldc_s, PrimitiveType::U8, 0, 0),
		Ldc_s_U8_1 = Props(Incode::Ldc_s, PrimitiveType::U8, 1, 0),
		Ldc_s_I16_0 = Props(Incode::Ldc_s, PrimitiveType::I16, 0, 0),
		Ldc_s_I16_1 = Props(Incode::Ldc_s, PrimitiveType::I16, 1, 0),
		Ldc_s_U16_0 = Props(Incode::Ldc_s, PrimitiveType::U16, 0, 0),
		Ldc_s_U16_1 = Props(Incode::Ldc_s, PrimitiveType::U16, 1, 0),

		Add_I8 = Props(Incode::Add, PrimitiveType::I8, 0, 0),
		Add_I16 = Props(Incode::Add, PrimitiveType::I16, 0, 0),
		Add_I32 = Props(Incode::Add, PrimitiveType::I32, 0, 0),
		Add_I64 = Props(Incode::Add, PrimitiveType::I64, 0, 0),
		Add_F32 = Props(Incode::Add, PrimitiveType::F32, 0, 0),
		Add_F64 = Props(Incode::Add, PrimitiveType::F64, 0, 0),

		Neg_I8 = Props(Incode::Neg, PrimitiveType::I8, 0, 0),
		Neg_I16 = Props(Incode::Neg, PrimitiveType::I16, 0, 0),
		Neg_I32 = Props(Incode::Neg, PrimitiveType::I32, 0, 0),
		Neg_I64 = Props(Incode::Neg, PrimitiveType::I64, 0, 0),
		Neg_F32 = Props(Incode::Neg, PrimitiveType::F32, 0, 0),
		Neg_F64 = Props(Incode::Neg, PrimitiveType::F64, 0, 0),

		BitNot_I8 = Props(Incode::BitNot, PrimitiveType::I8, 0, 0),
		BitNot_U8 = Props(Incode::BitNot, PrimitiveType::U8, 0, 0),
		BitNot_I16 = Props(Incode::BitNot, PrimitiveType::I16, 0, 0),
		BitNot_U16 = Props(Incode::BitNot, PrimitiveType::U16, 0, 0),
		BitNot_I32 = Props(Incode::BitNot, PrimitiveType::I32, 0, 0),
		BitNot_U32 = Props(Incode::BitNot, PrimitiveType::U32, 0, 0),
		BitNot_I64 = Props(Incode::BitNot, PrimitiveType::I64, 0, 0),
		BitNot_U64 = Props(Incode::BitNot, PrimitiveType::U64, 0, 0),
	};
}
