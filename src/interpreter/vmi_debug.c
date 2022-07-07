#include "vmi_debug.h"
#include "vmi_ops.h"

#define VMI_DEBUG_INSTR(I) case I: printf(#I "\n"); break

void vmi_debug_instruction(vmi_ip ip)
{
	const vmi_opcode_header* const header = (const vmi_opcode_header*)ip;
	switch (header->icode) {
		VMI_DEBUG_INSTR(VMI_LDA);
		VMI_DEBUG_INSTR(VMI_LDA_A);
		VMI_DEBUG_INSTR(VMI_ALLOCS);
		VMI_DEBUG_INSTR(VMI_FREES);
		VMI_DEBUG_INSTR(VMI_ALLOCH);
		VMI_DEBUG_INSTR(VMI_ALLOCH_C);
		VMI_DEBUG_INSTR(VMI_FREEH);
		VMI_DEBUG_INSTR(VMI_CALL);
		VMI_DEBUG_INSTR(VMI_CALLNATIVE);
		VMI_DEBUG_INSTR(VMI_RET);
		VMI_DEBUG_INSTR(VMI_LDC_S);
		VMI_DEBUG_INSTR(VMI_LDC);
		VMI_DEBUG_INSTR(VMI_LDC_I8);
		VMI_DEBUG_INSTR(VMI_LOCALS);
		VMI_DEBUG_INSTR(VMI_LDL);
		VMI_DEBUG_INSTR(VMI_LDL_A);
		VMI_DEBUG_INSTR(VMI_STL);
		VMI_DEBUG_INSTR(VMI_STUREF);
		VMI_DEBUG_INSTR(VMI_STUREF_S);
		VMI_DEBUG_INSTR(VMI_STELEM);
		VMI_DEBUG_INSTR(VMI_STELEM_S);
		VMI_DEBUG_INSTR(VMI_LDELEM);
		VMI_DEBUG_INSTR(VMI_LDELEM_S);
		VMI_DEBUG_INSTR(VMI_ADD);
		VMI_DEBUG_INSTR(VMI_CONV);
		VMI_DEBUG_INSTR(VMI_EOE);
	default:
		printf("<unknown instruction>\n");
		break;
	}
}

extern void vmi_debug_stack(const vmi_stack* s)
{
	printf("StackSize: %d\n", vmi_stack_count(s));
}
