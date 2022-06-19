#ifndef _VMC_COMPILER_H_
#include "../vmc_compiler.h"
#endif

#ifndef _VMI_OPS_H_
#include "../../interpreter/vmi_ops.h"
#endif

BOOL load_a(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	vmi_instr_load_a instr;
	vm_int32 index;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT) {
		return vmc_compiler_message_expected_index(&c->messages, l, t);
	}

	index = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	if (func->args_count == 0 || func->args_count <= index) {
		return vmc_compiler_message_invalid_index(&c->messages, l, index, 0, func->args_count - 1);
	}
	instr.opcode = 0;
	instr.icode = VMI_LOAD_A;
	instr.size = func->args[index].type.size;
	instr.offset = func->args[index].type.offset;
	vmc_write(c, &instr, sizeof(vmi_instr_load_a));
	return TRUE;
}
