#ifndef _VMC_EOE_H_
#define _VMC_EOE_H_

#include "vm_compiler.h"
#include "../interpreter/vmi_ops.h"

// Write end-of-execution
void vmc_write_eoe(vmc_compiler* c)
{
	vmi_instr_eoe instr;
	instr.header.opcode = 0;
	instr.header.icode = VMI_EOE;
	vmc_write(c, &instr, sizeof(vmi_instr_eoe));
}

#endif
