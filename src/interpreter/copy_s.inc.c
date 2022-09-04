#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_copy_s_int32(arThread* t, arIP ip)
{
	const arInt32* const value = (const arInt32*)arStack_peek(&t->stack, sizeof(arInt32));
	arInt32* target = (arInt32*)arStack_push(&t->stack, sizeof(arInt32));
	*target = *value;
	return ip + sizeof(vmi_instr_single_instruction);
}
