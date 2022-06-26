#ifndef _VMC_COMPILER_H_
#include "../vmc_compiler.h"
#endif

#ifndef _VMI_OPS_H_
#include "../../interpreter/vmi_ops.h"
#endif

#ifndef FUNC_BODY
#define FUNC_BODY(N) BOOL parse_##N(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
#endif

FUNC_BODY(ldc_s_i8)
{
	vmi_instr_ldc_s instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_S;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT8;
	instr.i8 = (vm_int8)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_s));
	return TRUE;
}

FUNC_BODY(ldc_s_i16)
{
	vmi_instr_ldc_s instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_S;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT16;
	instr.i8 = (vm_int8)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_s));
	return TRUE;
}

FUNC_BODY(ldc_s_i32)
{
	vmi_instr_ldc_s instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_S;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT32;
	instr.i8 = (vm_int8)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_s));
	return TRUE;
}

FUNC_BODY(ldc_s_i64)
{
	vmi_instr_ldc_s instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_S;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT64;
	instr.i8 = (vm_int8)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_s));
	return TRUE;
}

FUNC_BODY(ldc_s_f32)
{
	vmi_instr_ldc_s instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_S;
	instr.props1 = VMI_INSTR_CONST_PROP1_FLOAT32;
	instr.i8 = (vm_int8)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_s));
	return TRUE;
}

FUNC_BODY(ldc_s_f64)
{
	vmi_instr_ldc_s instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_S;
	instr.props1 = VMI_INSTR_CONST_PROP1_FLOAT64;
	instr.i8 = (vm_int8)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_s));
	return TRUE;
}

FUNC_BODY(ldc_i8)
{
	vmi_instr_ldc_i32 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT8;
	instr.i8 = (vm_int8)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_i32));
	return TRUE;
}

FUNC_BODY(ldc_i16)
{
	vmi_instr_ldc_i32 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT16;
	instr.i16 = (vm_int16)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_i32));
	return TRUE;
}

FUNC_BODY(ldc_i32)
{
	vmi_instr_ldc_i32 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT32;
	instr.i32 = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_i32));
	return TRUE;
}

FUNC_BODY(ldc_i64)
{
	vmi_instr_ldc_i64 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT)
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_I64;
	instr.props1 = VMI_INSTR_CONST_PROP1_INT64;
	instr.i64 = strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_i64));
	return TRUE;
}

FUNC_BODY(ldc_f32)
{
	vmi_instr_ldc_i32 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT && t->type != VMC_LEXER_TYPE_DECIMAL)
		return vmc_compiler_message_expected_decimal(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC;
	instr.props1 = VMI_INSTR_CONST_PROP1_FLOAT32;
	if (t->type == VMC_LEXER_TYPE_DECIMAL)
		instr.f32 = (vm_float32)strtod(t->string.start, NULL);
	else
		instr.f32 = (vm_float32)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_i32));
	return TRUE;
}

FUNC_BODY(ldc_f64)
{
	vmi_instr_ldc_i64 instr;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT && t->type != VMC_LEXER_TYPE_DECIMAL)
		return vmc_compiler_message_expected_decimal(&c->messages, l, t);
	instr.opcode = 0;
	instr.icode = VMI_LDC_I64;
	instr.props1 = VMI_INSTR_CONST_PROP1_FLOAT64;
	if (t->type == VMC_LEXER_TYPE_DECIMAL)
		instr.f64 = strtod(t->string.start, NULL);
	else
		instr.f64 = (vm_float64)strtoi64(t->string.start, vm_string_length(&t->string));
	vmc_write(c, &instr, sizeof(vmi_instr_ldc_i64));
	return TRUE;
}

FUNC_BODY(clt)
{
	vmi_instr_cmp instr;
	instr.opcode = 0;
	instr.icode = VMI_CMP;
	instr.props1 = VMI_INSTR_CMP_PROP1_LT;
	instr.props2 = VMI_INSTR_CMP_PROP2_SIGNED;
	vmc_write(c, &instr, sizeof(vmi_instr_cmp));
	return TRUE;
}

FUNC_BODY(cgt)
{
	vmi_instr_cmp instr;
	instr.opcode = 0;
	instr.icode = VMI_CMP;
	instr.props1 = VMI_INSTR_CMP_PROP1_GT;
	instr.props2 = VMI_INSTR_CMP_PROP2_SIGNED;
	vmc_write(c, &instr, sizeof(vmi_instr_cmp));
	return TRUE;
}

FUNC_BODY(load_a)
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
	instr.size = func->args[index].definition->size;
	instr.offset = func->args[index].offset;
	vmc_write(c, &instr, sizeof(vmi_instr_load_a));
	return TRUE;
}

FUNC_BODY(save_r)
{
	vmi_instr_save_r instr;
	vm_int32 index;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT) {
		return vmc_compiler_message_expected_index(&c->messages, l, t);
	}

	index = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	if (func->returns_count == 0 || func->returns_count <= index) {
		return vmc_compiler_message_invalid_index(&c->messages, l, index, 0, func->returns_count - 1);
	}
	instr.opcode = 0;
	instr.icode = VMI_SAVE_R;
	instr.size = func->returns[index].definition->size;
	instr.offset = func->returns[index].offset;
	vmc_write(c, &instr, sizeof(vmi_instr_save_r));
	return TRUE;
}

FUNC_BODY(locals)
{
	vmi_instr_locals instr;
	// Parse locals
	if (!_vmc_parse_keyword_fn_locals(c, l, p, t, func))
		return FALSE;

	instr.opcode = 0;
	instr.icode = VMI_LOCALS;
	instr.size = func->locals_total_size;
	vmc_write(c, &instr, sizeof(vmi_instr_locals));
	return TRUE;
}

FUNC_BODY(load_l)
{
	vmi_instr_load_l instr;
	vm_int32 index;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT) {
		return vmc_compiler_message_expected_index(&c->messages, l, t);
	}

	index = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	if (func->locals_count == 0 || func->locals_count <= index) {
		return vmc_compiler_message_invalid_index(&c->messages, l, index, 0, func->locals_count - 1);
	}
	instr.opcode = 0;
	instr.icode = VMI_LOAD_L;
	instr.size = func->locals[index].definition->size;
	instr.offset = func->locals[index].offset;
	vmc_write(c, &instr, sizeof(vmi_instr_load_l));
	return TRUE;
}

FUNC_BODY(save_l)
{
	vmi_instr_save_l instr;
	vm_int32 index;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT) {
		return vmc_compiler_message_expected_index(&c->messages, l, t);
	}

	index = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	if (func->locals_count == 0 || func->locals_count <= index) {
		return vmc_compiler_message_invalid_index(&c->messages, l, index, 0, func->locals_count - 1);
	}
	instr.opcode = 0;
	instr.icode = VMI_SAVE_L;
	instr.size = func->locals[index].definition->size;
	instr.offset = func->locals[index].offset;
	vmc_write(c, &instr, sizeof(vmi_instr_save_l));
	return TRUE;
}

FUNC_BODY(ldl_a)
{
	vmi_instr_ldl_a instr;
	vm_int32 index;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT) {
		return vmc_compiler_message_expected_index(&c->messages, l, t);
	}

	index = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	if (func->locals_count == 0 || func->locals_count <= index) {
		return vmc_compiler_message_invalid_index(&c->messages, l, index, 0, func->locals_count - 1);
	}
	instr.opcode = 0;
	instr.icode = VMI_LDL_A;
	instr.size = func->locals[index].definition->size;
	instr.offset = func->locals[index].offset;
	vmc_write(c, &instr, sizeof(vmi_instr_ldl_a));
	return TRUE;
}

FUNC_BODY(sunref_i32)
{
	_vmc_emit_opcode(c, VMI_OP_SUNREF_INT32);
	return TRUE;
}

FUNC_BODY(stelem)
{
	vmi_instr_stelem instr;
	vmc_var var;
	vmc_lexer_next(l, t);
	if (!_vmc_parse_type2(c, l, p, t, &var))
		return FALSE;
	instr.opcode = 0;
	instr.icode = VMI_STELEM;
	instr.size_per_element = var.definition->size;
	vmc_write(c, &instr, sizeof(vmi_instr_stelem));
	return TRUE;
}

FUNC_BODY(stelem_s)
{
	vmi_instr_stelem_s instr;
	vmc_var var;
	vmc_lexer_next(l, t);
	if (!_vmc_parse_type2(c, l, p, t, &var))
		return FALSE;
	instr.opcode = 0;
	instr.icode = VMI_STELEM_S;
	instr.size = var.definition->size;
	vmc_write(c, &instr, sizeof(vmi_instr_stelem_s));
	return TRUE;
}

FUNC_BODY(ldelem)
{
	vmi_instr_ldelem instr;
	vmc_var var;
	vmc_lexer_next(l, t);
	if (!_vmc_parse_type2(c, l, p, t, &var))
		return FALSE;
	instr.opcode = 0;
	instr.icode = VMI_LDELEM;
	instr.size_per_element = var.definition->size;
	vmc_write(c, &instr, sizeof(vmi_instr_ldelem));
	return TRUE;
}

FUNC_BODY(ldelem_s)
{
	vmi_instr_ldelem_s instr;
	vmc_var var;
	vmc_lexer_next(l, t);
	if (!_vmc_parse_type2(c, l, p, t, &var))
		return FALSE;
	instr.opcode = 0;
	instr.icode = VMI_LDELEM_S;
	instr.size = var.definition->size;
	vmc_write(c, &instr, sizeof(vmi_instr_ldelem_s));
	return TRUE;
}

FUNC_BODY(alloc_s)
{
	vmi_instr_alloc_s instr;
	vm_int32 num_bytes;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT) {
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	}

	num_bytes = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	if (num_bytes < 0) {
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	}
	else if (num_bytes > UINT16_MAX) {
		return vmc_compiler_message_not_implemented(&c->messages, l, t);
	}
	instr.opcode = 0;
	instr.icode = VMI_ALLOC_S;
	instr.size = num_bytes;
	vmc_write(c, &instr, sizeof(vmi_instr_alloc_s));
	return TRUE;
}

FUNC_BODY(free_s)
{
	vmi_instr_free_s instr;
	vm_int32 num_bytes;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_INT) {
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	}

	num_bytes = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	if (num_bytes < 0) {
		return vmc_compiler_message_expected_int(&c->messages, l, t);
	}
	else if (num_bytes > UINT16_MAX) {
		return vmc_compiler_message_not_implemented(&c->messages, l, t);
	}
	instr.opcode = 0;
	instr.icode = VMI_FREE_S;
	instr.size = num_bytes;
	vmc_write(c, &instr, sizeof(vmi_instr_free_s));
	return TRUE;
}

FUNC_BODY(copy_s)
{
	// TODO: It should be possible to know which type is pushed in the current scope by the compiler
	// copy_s <type>
	vmi_opcode opcode = VMI_COPY_S;
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_KEYWORD)
		return vmc_compiler_message_expected_type(&c->messages, l, t);
	if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(int32)))
		opcode |= VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT32);
	else
		return vmc_compiler_message_not_implemented(&c->messages, l, t);
	_vmc_emit_opcode(c, opcode);
	return TRUE;
}

FUNC_BODY(add)
{
	vmi_opcode opcode = VMI_ADD;

	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_KEYWORD)
		return vmc_compiler_message_expected_type(&c->messages, l, t);
	if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(int32)))
		opcode |= VMI_PROPS1_OPCODE(VMI_INSTR_ADD_PROP1_INT32);
	else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(int16)))
		opcode |= VMI_PROPS1_OPCODE(VMI_INSTR_ADD_PROP1_INT16);
	else
		return vmc_compiler_message_not_implemented(&c->messages, l, t);
	_vmc_emit_opcode(c, opcode);
	return TRUE;
}

FUNC_BODY(conv_i16_i32)
{
	vmi_instr_conv instr;
	instr.header.opcode = 0;
	instr.header.icode = VMI_CONV;
	instr.props1 = VMI_INSTR_PROP_INT16;
	instr.props2 = VMI_INSTR_PROP_INT32;
	vmc_write(c, &instr, sizeof(vmi_instr_conv));
	return TRUE;
}

FUNC_BODY(conv_i32_i16)
{
	vmi_instr_conv instr;
	instr.header.opcode = 0;
	instr.header.icode = VMI_CONV;
	instr.props1 = VMI_INSTR_PROP_INT32;
	instr.props2 = VMI_INSTR_PROP_INT16;
	vmc_write(c, &instr, sizeof(vmi_instr_conv));
	return TRUE;
}
