#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

// Negate 1 byte of memory on the stack
arIP _arThread_bit_noti1(arThread* t, arIP ip)
{
	arInt8* value = (arInt8*)(t->stack.top - sizeof(arInt8));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <i8>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 1 byte of memory on the stack
arIP _arThread_bit_notui1(arThread* t, arIP ip)
{
	arUint8* value = (arUint8*)(t->stack.top - sizeof(arUint8));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <ui8>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 2 byte of memory on the stack
arIP _arThread_bit_noti2(arThread* t, arIP ip)
{
	arInt16* value = (arInt16*)(t->stack.top - sizeof(arInt16));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <i16>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 2 byte of memory on the stack
arIP _arThread_bit_notui2(arThread* t, arIP ip)
{
	arUint16* value = (arUint16*)(t->stack.top - sizeof(arUint16));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <ui16>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
arIP _arThread_bit_noti4(arThread* t, arIP ip)
{
	arInt32* value = (arInt32*)(t->stack.top - sizeof(arInt32));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <i32>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
arIP _arThread_bit_notui4(arThread* t, arIP ip)
{
	arUint32* value = (arUint32*)(t->stack.top - sizeof(arUint32));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <ui32>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
arIP _arThread_bit_noti8(arThread* t, arIP ip)
{
	arInt64* value = (arInt64*)(t->stack.top - sizeof(arInt64));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <i64>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
arIP _arThread_bit_notui8(arThread* t, arIP ip)
{
	arUint64* value = (arUint64*)(t->stack.top - sizeof(arUint64));
	*value = ~(*value);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BIT <not> <ui64>");
#endif
	return ip + sizeof(vmi_instr_single_instruction);
}
