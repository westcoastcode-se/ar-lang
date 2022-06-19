#ifndef _VMC_COMPILER_H_
#include "../vmc_compiler.h"
#endif

#ifndef _VMI_OPS_H_
#include "../../interpreter/vmi_ops.h"
#endif

BOOL c_i16(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	vmi_instr_const_int32 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_CONST;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT16;
	instr.i16 = (vm_int16)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_const_int32));
	return TRUE;
}

BOOL c_i32(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	vmi_instr_const_int32 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_CONST;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT32;
	instr.value = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_const_int32));
	return TRUE;
}
