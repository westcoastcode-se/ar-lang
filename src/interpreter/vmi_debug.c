#include "vmi_debug.h"
#include "vmi_ops.h"

#define VMI_DEBUG_INSTR(I) case I: printf(#I "\n"); break

void vmi_debug_instruction(vmi_ip ip)
{
	const vmi_opcode_header* const header = (const vmi_opcode_header*)ip;
	switch (header->icode) {
		VMI_DEBUG_INSTR(VMI_BEGIN);
		VMI_DEBUG_INSTR(VMI_LOAD_A);
		VMI_DEBUG_INSTR(VMI_SAVE_R);
		VMI_DEBUG_INSTR(VMI_ALLOC_S);
		VMI_DEBUG_INSTR(VMI_FREE_S);
		VMI_DEBUG_INSTR(VMI_CALL);
		VMI_DEBUG_INSTR(VMI_RET);
		VMI_DEBUG_INSTR(VMI_LOCALS);
		VMI_DEBUG_INSTR(VMI_LDC_S);
		VMI_DEBUG_INSTR(VMI_LDC);
		VMI_DEBUG_INSTR(VMI_LDC_I64);
		VMI_DEBUG_INSTR(VMI_LOAD_L);
		VMI_DEBUG_INSTR(VMI_SAVE_L);
		VMI_DEBUG_INSTR(VMI_LDL_A);
		VMI_DEBUG_INSTR(VMI_SUNREF);
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
