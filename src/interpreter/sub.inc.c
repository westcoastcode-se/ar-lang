#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

// Add the two top-most values from the stack. Assume that they are 1 byte integers
arIP _arThread_subi1(arThread* t, arIP ip)
{
	const arInt8 rhs = *(const arInt8*)arStack_pop(&t->stack, sizeof(arInt8));
	arInt8* lhs = (arInt8*)(t->stack.top - sizeof(arInt8));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 1 byte integers
arIP _arThread_subui1(arThread* t, arIP ip)
{
	const arUint8 rhs = *(const arUint8*)arStack_pop(&t->stack, sizeof(arUint8));
	arUint8* lhs = (arUint8*)(t->stack.top - sizeof(arUint8));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 2 byte integers
arIP _arThread_subi2(arThread* t, arIP ip)
{
	const arInt16 rhs = *(const arInt16*)arStack_pop(&t->stack, sizeof(arInt16));
	arInt16* lhs = (arInt16*)(t->stack.top - sizeof(arInt16));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 2 byte integers
arIP _arThread_subui2(arThread* t, arIP ip)
{
	const arUint16 rhs = *(const arUint16*)arStack_pop(&t->stack, sizeof(arUint16));
	arUint16* lhs = (arUint16*)(t->stack.top - sizeof(arUint16));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 4 byte integers
arIP _arThread_subi4(arThread* t, arIP ip)
{
	const arInt32 rhs = *(const arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt32* lhs = (arInt32*)(t->stack.top - sizeof(arInt32));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 4 byte integers
arIP _arThread_subui4(arThread* t, arIP ip)
{
	const arUint32 rhs = *(const arUint32*)arStack_pop(&t->stack, sizeof(arUint32));
	arUint32* lhs = (arUint32*)(t->stack.top - sizeof(arUint32));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 8 byte integers
arIP _arThread_subi8(arThread* t, arIP ip)
{
	const arInt64 rhs = *(const arInt64*)arStack_pop(&t->stack, sizeof(arInt64));
	arInt64* lhs = (arInt64*)(t->stack.top - sizeof(arInt64));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 8 byte integers
arIP _arThread_subui8(arThread* t, arIP ip)
{
	const arUint64 rhs = *(const arUint64*)arStack_pop(&t->stack, sizeof(arUint64));
	arUint64* lhs = (arUint64*)(t->stack.top - sizeof(arUint64));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 4 byte floats
arIP _arThread_subf4(arThread* t, arIP ip)
{
	const arFloat32 rhs = *(const arFloat32*)arStack_pop(&t->stack, sizeof(arFloat32));
	arFloat32* lhs = (arFloat32*)(t->stack.top - sizeof(arFloat32));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 8 byte floats
arIP _arThread_subf8(arThread* t, arIP ip)
{
	const arFloat64 rhs = *(const arFloat64*)arStack_pop(&t->stack, sizeof(arFloat64));
	arFloat64* lhs = (arFloat64*)(t->stack.top - sizeof(arFloat64));
	*lhs = *lhs - rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}
