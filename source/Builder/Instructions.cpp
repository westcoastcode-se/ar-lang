#include "Instructions.h"
#include "Symbols.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Builder;

void InstrMarker::SetInstr(Instr* i)
{
	_instr = i;
}

I32 InstrLdc::GetSize() const
{
	if (stackType->GetStackSize() < 4)
		return sizeof(Interpreter::InstrLdc_s);
	else if (stackType->GetStackSize() < 8)
		return sizeof(Interpreter::InstrLdc);
	else
		return sizeof(Interpreter::InstrLdc_l);
}

void InstrLdc::Write(MemoryStream& s)
{
	switch (GetSize())
	{
		case sizeof(Interpreter::InstrLdc_s) : {
			Interpreter::InstrLdc_s instr;
			instr.opcode = 0;
			instr.icode = Interpreter::Incode::Ldc_s;
			instr.props1 = (I8)stackType->GetPrimitiveType();
			instr.i16 = value.i16;
			s.Write(&instr);
			break;
		}
		case sizeof(Interpreter::InstrLdc) : {
			Interpreter::InstrLdc instr;
			instr.opcode = 0;
			instr.icode = Interpreter::Incode::Ldc;
			instr.props1 = (I8)stackType->GetPrimitiveType();
			instr.i32 = value.i32;
			s.Write(&instr);
			break;
		}
		default: {
			Interpreter::InstrLdc_l instr;
			instr.opcode = 0;
			instr.icode = Interpreter::Incode::Ldc_l;
			instr.props1 = (I8)stackType->GetPrimitiveType();
			instr.i64 = value.i64;
			s.Write(&instr);
			break;
		}
	}
}

void InstrAdd::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Add;
	data.props1 = (I8)type->GetPrimitiveType();
	s.Write(&data);
}

void InstrSub::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Sub;
	data.props1 = (I8)type->GetPrimitiveType();
	s.Write(&data);
}

void InstrMult::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Mult;
	data.props1 = (I8)type->GetPrimitiveType();
	s.Write(&data);
}

void InstrDiv::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Div;
	data.props1 = (I8)type->GetPrimitiveType();
	s.Write(&data);
}

void InstrRet::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Ret;
#if defined(VM_STACK_DEBUG)
	// The values pushed on the stack when the function starts are
	// 1. arguments
	// 2. return values
	data.expected_ebp_offset = function->GetArgumentsSize() + function->GetReturnsSize();
#endif
	s.Write(&data);
}

void InstrCall::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Call;
	data.expectedStackSize = function->GetArgumentsSize();
	s.Write(&data);
}

void InstrNeg::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Neg;
	data.props1 = (I8)type->GetPrimitiveType();
	s.Write(&data);
}

void InstrBitNot::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::BitNot;
	data.props1 = (I8)type->GetPrimitiveType();
	s.Write(&data);
}

void InstrConv::Write(MemoryStream& s)
{
	data.icode = Interpreter::Incode::Conv;
	data.props1 = (I8)type->GetPrimitiveType();
	data.props2 = (I8)to->GetPrimitiveType();
	s.Write(&data);
}
