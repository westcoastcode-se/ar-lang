#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_conv_i2_i4(arThread* t, arIP ip)
{
	const arInt16 rhs = *(const arInt16*)arStack_pop(&t->stack, sizeof(arInt16));
	arInt32* result = (arInt32*)arStack_push(&t->stack, sizeof(arInt32));
	*result = (arInt32)rhs;
	return ip + sizeof(vmi_instr_conv);
}

arIP _arThread_conv_i4_i2(arThread* t, arIP ip)
{
	const arInt32 rhs = *(const arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt16* result = (arInt16*)arStack_push(&t->stack, sizeof(arInt16));
	*result = (arInt16)rhs;
	return ip + sizeof(vmi_instr_conv);
}
