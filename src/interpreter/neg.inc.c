#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

// Negate 1 byte of memory on the stack
arIP _arThread_negi1(arThread* t, arIP ip)
{
	arInt8* value = (arInt8*)(t->stack.top - sizeof(arInt8));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <i8>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 1 byte of memory on the stack
arIP _arThread_negui1(arThread* t, arIP ip)
{
	arUint8* value = (arUint8*)(t->stack.top - sizeof(arUint8));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <ui8>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 2 byte of memory on the stack
arIP _arThread_negi2(arThread* t, arIP ip)
{
	arInt16* value = (arInt16*)(t->stack.top - sizeof(arInt16));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <i16>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 2 byte of memory on the stack
arIP _arThread_negui2(arThread* t, arIP ip)
{
	arUint16* value = (arUint16*)(t->stack.top - sizeof(arUint16));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <ui16>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
arIP _arThread_negi4(arThread* t, arIP ip)
{
	arInt32* value = (arInt32*)(t->stack.top - sizeof(arInt32));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <i32>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
arIP _arThread_negui4(arThread* t, arIP ip)
{
	arUint32* value = (arUint32*)(t->stack.top - sizeof(arUint32));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <ui32>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
arIP _arThread_negi8(arThread* t, arIP ip)
{
	arInt64* value = (arInt64*)(t->stack.top - sizeof(arInt64));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <i64>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
arIP _arThread_negui8(arThread* t, arIP ip)
{
	arUint64* value = (arUint64*)(t->stack.top - sizeof(arUint64));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <ui64>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
arIP _arThread_negf4(arThread* t, arIP ip)
{
	arFloat32* value = (arFloat32*)(t->stack.top - sizeof(arFloat32));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <f32>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
arIP _arThread_negf8(arThread* t, arIP ip)
{
	arFloat64* value = (arFloat64*)(t->stack.top - sizeof(arFloat64));
	*value = -(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <f64>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}