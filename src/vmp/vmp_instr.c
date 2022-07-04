#include "vmp_instr.h"
#include "vmp.h"
#include "vmp_list_args.h"
#include "vmp_messages.h"
#include "../vmc/vmc_debug.h"

vmp_constant vmp_const_i1(vm_int8 value)
{
	vmp_constant c;
	c.i1 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT8;
	return c;
}

vmp_constant vmp_const_ui1(vm_int8 value)
{
	vmp_constant c;
	c.ui1 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT8;
	return c;
}

vmp_constant vmp_const_i2(vm_int16 value)
{
	vmp_constant c;
	c.i2 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT16;
	return c;
}

vmp_constant vmp_const_ui2(vm_uint16 value)
{
	vmp_constant c;
	c.ui2 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT16;
	return c;
}

vmp_constant vmp_const_i4(vm_int32 value)
{
	vmp_constant c;
	c.i4 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT32;
	return c;
}

vmp_constant vmp_const_ui4(vm_uint32 value)
{
	vmp_constant c;
	c.ui4 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT32;
	return c;
}

vmp_constant vmp_const_i8(vm_int64 value)
{
	vmp_constant c;
	c.i8 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT64;
	return c;
}

vmp_constant vmp_const_ui8(vm_uint64 value)
{
	vmp_constant c;
	c.ui8 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT64;
	return c;
}

vmp_constant vmp_const_f4(vm_float32 value)
{
	vmp_constant c;
	c.f4 = value;
	c.type = VMI_INSTR_CONST_PROP1_FLOAT32;
	return c;
}

vmp_constant vmp_const_f8(vm_float64 value)
{
	vmp_constant c;
	c.f8 = value;
	c.type = VMI_INSTR_CONST_PROP1_FLOAT64;
	return c;
}

vmp_constant vmp_const_ptr(vm_byte* value)
{
	vmp_constant c;
	c.ptr = value;
#ifdef VM_32BIT
	c.type = VMI_INSTR_CONST_PROP1_INT32;
#else
	c.type = VMI_INSTR_CONST_PROP1_INT64;
#endif
	return c;
}

vmp_instr* vmp_instr_lda(vm_uint32 index)
{
	vmp_instr_def_lda* instr = (vmp_instr_def_lda*)vmc_malloc(sizeof(vmp_instr_def_lda));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDA, sizeof(vmi_instr_lda));
	instr->index = index;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_lda_a(vm_uint32 index)
{
	vmp_instr_def_lda_a* instr = (vmp_instr_def_lda_a*)vmc_malloc(sizeof(vmp_instr_def_lda_a));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDA_A, sizeof(vmi_instr_lda_a));
	instr->index = index;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_str(vm_uint32 index)
{
	vmp_instr_def_str* instr = (vmp_instr_def_str*)vmc_malloc(sizeof(vmp_instr_def_str));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STR, sizeof(vmi_instr_lda));
	instr->index = index;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldl(vm_uint32 index)
{
	vmp_instr_def_ldl* instr = (vmp_instr_def_ldl*)vmc_malloc(sizeof(vmp_instr_def_ldl));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDL, sizeof(vmi_instr_ldl));
	instr->index = index;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldl_a(vm_uint32 index)
{
	vmp_instr_def_ldl_a* instr = (vmp_instr_def_ldl_a*)vmc_malloc(sizeof(vmp_instr_def_ldl_a));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDL_A, sizeof(vmi_instr_ldl_a));
	instr->index = index;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_stl(vm_uint32 index)
{
	vmp_instr_def_stl* instr = (vmp_instr_def_stl*)vmc_malloc(sizeof(vmp_instr_def_stl));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STL, sizeof(vmi_instr_stl));
	instr->index = index;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_locals(const vmp_func* func)
{
	vmp_instr_def_locals* instr = (vmp_instr_def_locals*)vmc_malloc(sizeof(vmp_instr_def_locals));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LOCALS, sizeof(vmi_instr_locals));
	instr->reserve_stack_size = (vm_int16)func->locals_stack_size;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldc(const vmp_type* type, vmp_constant constant)
{
	switch (constant.type)
	{
	case VMI_INSTR_PROP_INT64:
	case VMI_INSTR_PROP_UINT64:
	case VMI_INSTR_PROP_FLOAT64:
	{

		vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)vmc_malloc(sizeof(vmp_instr_def_ldc));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC_I8, sizeof(vmi_instr_ldc_i64));
		instr->type = type;
		instr->constant = constant;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
	default:
	{

		vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)vmc_malloc(sizeof(vmp_instr_def_ldc));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC, sizeof(vmi_instr_ldc_i32));
		instr->type = type;
		instr->constant = constant;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
	}
}

vmp_instr* vmp_instr_ldc_s(const vmp_type* type, vmp_constant constant)
{
	vmp_instr_def_ldc_s* instr = (vmp_instr_def_ldc_s*)vmc_malloc(sizeof(vmp_instr_def_ldc_s));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC_S, sizeof(vmi_instr_ldc_s));
	instr->type = type;
	instr->constant = constant;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_allocs(const vmp_type* type)
{
	vmp_instr_def_allocs* instr = (vmp_instr_def_allocs*)vmc_malloc(sizeof(vmp_instr_def_allocs));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ALLOCS, sizeof(vmi_instr_allocs));
	instr->type = type;
	instr->amount = 0;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_allocs_const(vm_int16 amount)
{
	vmp_instr_def_allocs* instr = (vmp_instr_def_allocs*)vmc_malloc(sizeof(vmp_instr_def_allocs));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ALLOCS, sizeof(vmi_instr_allocs));
	instr->type = NULL;
	instr->amount = amount;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_sturef(const vmp_type* type)
{
	vmp_instr_def_sturef* instr = (vmp_instr_def_sturef*)vmc_malloc(sizeof(vmp_instr_def_sturef));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STUREF, sizeof(vmi_instr_sturef));
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldc_i8(const vmp_type* type, vmp_constant constant)
{
	vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)vmc_malloc(sizeof(vmp_instr_def_ldc));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC_I8, sizeof(vmi_instr_ldc_i64));
	instr->type = type;
	instr->constant = constant;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_call(const vmp_func* func)
{
	vmp_instr_def_call* instr = (vmp_instr_def_call*)vmc_malloc(sizeof(vmp_instr_def_call));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CALL, sizeof(vmi_instr_call));
	instr->target_func = func;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_add(vm_int8 type)
{
	vmp_instr_def_add* instr = (vmp_instr_def_add*)vmc_malloc(sizeof(vmp_instr_def_add));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ADD, sizeof(vmi_instr_add));
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_clt(const vmp_type* type)
{
	vmp_instr_def_cmp* instr = (vmp_instr_def_cmp*)vmc_malloc(sizeof(vmp_instr_def_cmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CMP, sizeof(vmi_instr_cmp));
	instr->comparator = VMI_INSTR_CMP_PROP1_LT;
	instr->compare_type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_cgt(const vmp_type* type)
{
	vmp_instr_def_cmp* instr = (vmp_instr_def_cmp*)vmc_malloc(sizeof(vmp_instr_def_cmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CMP, sizeof(vmi_instr_cmp));
	instr->comparator = VMI_INSTR_CMP_PROP1_GT;
	instr->compare_type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_jmpt(const vmp_marker* marker)
{
	vmp_instr_def_jmp* instr = (vmp_instr_def_jmp*)vmc_malloc(sizeof(vmp_instr_def_jmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_JMP, sizeof(vmi_instr_jmp));
	instr->op = VMI_INSTR_JMP_PROP1_TRUE;
	instr->marker = marker;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_jmpf(const vmp_marker* marker)
{
	vmp_instr_def_jmp* instr = (vmp_instr_def_jmp*)vmc_malloc(sizeof(vmp_instr_def_jmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_JMP, sizeof(vmi_instr_jmp));
	instr->op = VMI_INSTR_JMP_PROP1_FALSE;
	instr->marker = marker;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_conv(const vmp_type* from, const vmp_type* to)
{
	vmp_instr_def_conv* instr = (vmp_instr_def_conv*)vmc_malloc(sizeof(vmp_instr_def_conv));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CONV, sizeof(vmi_instr_conv));
	instr->from_type = from;
	instr->to_type = to;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_basic(vmp_instr_type type, vm_int32 size)
{
	vmp_instr_def_basic* instr = (vmp_instr_def_basic*)vmc_malloc(sizeof(vmp_instr_def_basic));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, type, size);
	return VMC_PIPELINE_INSTR_BASE(instr);
}

const vmp_instr* vmp_instr_build(const vmp_instr* h, struct vmp_builder* builder)
{
	const vmp_func* const func = h->func;

	switch (h->instr_type)
	{
	case VMP_INSTR_LDA:
	{
		const vmp_instr_def_lda* const cmd = (vmp_instr_def_lda*)h;
		const vmp_arg* const arg = vmp_list_args_get(&func->args, cmd->index);
		if (arg == NULL) {
			vmp_builder_message_arg_index_missing(builder, cmd->index);
			return NULL;
		}

		vmi_instr_lda instr;
		instr.opcode = 0;
		instr.icode = VMI_LDA;
		instr.size = arg->type->size;
		instr.offset = arg->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_lda))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDA_A:
	{
		const vmp_instr_def_lda_a* const cmd = (vmp_instr_def_lda_a*)h;
		const vmp_arg* const arg = vmp_list_args_get(&func->args, cmd->index);
		if (arg == NULL) {
			vmp_builder_message_arg_index_missing(builder, cmd->index);
			return NULL;
		}

		vmi_instr_lda_a instr;
		instr.opcode = 0;
		instr.icode = VMI_LDA_A;
		instr.size = arg->type->size;
		instr.offset = arg->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_lda_a))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDC:
	{
		const vmp_instr_def_ldc* const cmd = (vmp_instr_def_ldc*)h;

		vmi_instr_ldc_i32 instr;
		instr.opcode = 0;
		instr.icode = VMI_LDC;
		instr.props1 = cmd->constant.type;
		instr.i32 = cmd->constant.i4;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldc_i32))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDC_S:
	{
		const vmp_instr_def_ldc_s* const cmd = (vmp_instr_def_ldc_s*)h;

		vmi_instr_ldc_s instr;
		instr.opcode = 0;
		instr.icode = VMI_LDC_S;
		instr.props1 = cmd->constant.type;
		instr.i16 = cmd->constant.i2;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldc_s))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDC_I8:
	{
		const vmp_instr_def_ldc* const cmd = (vmp_instr_def_ldc*)h;

		vmi_instr_ldc_i64 instr;
		instr.opcode = 0;
		instr.icode = VMI_LDC_I8;
		instr.props1 = cmd->constant.type;
		instr.i64 = cmd->constant.i8;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldc_i64))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_STL:
	{
		const vmp_instr_def_stl* const cmd = (vmp_instr_def_stl*)h;
		const vmp_local* const local = vmp_list_locals_get(&func->locals, cmd->index);
		if (local == NULL) {
			vmp_builder_message_local_index_missing(builder, cmd->index);
			return NULL;
		}

		vmi_instr_stl instr;
		instr.opcode = 0;
		instr.icode = VMI_STL;
		instr.size = local->type->size;
		instr.offset = local->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_stl))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDL:
	{
		const vmp_instr_def_ldl* const cmd = (vmp_instr_def_ldl*)h;
		const vmp_local* const local = vmp_list_locals_get(&func->locals, cmd->index);
		if (local == NULL) {
			vmp_builder_message_local_index_missing(builder, cmd->index);
			return NULL;
		}

		vmi_instr_ldl instr;
		instr.opcode = 0;
		instr.icode = VMI_LDL;
		instr.size = local->type->size;
		instr.offset = local->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldl))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDL_A:
	{
		const vmp_instr_def_ldl_a* const cmd = (vmp_instr_def_ldl_a*)h;
		const vmp_local* const local = vmp_list_locals_get(&func->locals, cmd->index);
		if (local == NULL) {
			vmp_builder_message_local_index_missing(builder, cmd->index);
			return NULL;
		}

		vmi_instr_ldl_a instr;
		instr.opcode = 0;
		instr.icode = VMI_LDL_A;
		instr.size = local->type->size;
		instr.offset = local->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldl_a))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LOCALS:
	{
		const vmp_instr_def_locals* const cmd = (vmp_instr_def_locals*)h;

		vmi_instr_locals instr;
		instr.opcode = 0;
		instr.icode = VMI_LOCALS;
		instr.size = cmd->reserve_stack_size;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_locals))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_ALLOCS:
	{
		const vmp_instr_def_allocs* const cmd = (vmp_instr_def_allocs*)h;

		vmi_instr_allocs instr;
		instr.opcode = 0;
		instr.icode = VMI_ALLOCS;
		if (cmd->type != NULL)
			instr.size = cmd->type->size;
		else
			instr.size = cmd->amount;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_allocs))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_STUREF:
	{
		const vmp_instr_def_sturef* const cmd = (vmp_instr_def_sturef*)h;

		vmi_instr_sturef instr;
		instr.opcode = 0;
		instr.icode = VMI_STUREF;
		instr.size = cmd->type->size;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_sturef))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_CALL:
	{
		const vmp_instr_def_call* const cmd = (vmp_instr_def_call*)h;
		const vmp_func* const func = cmd->target_func;

		vmi_instr_call instr;
		instr.opcode = 0;
		instr.icode = VMI_CALL;
		instr.expected_stack_size = func->args_stack_size + func->returns_stack_size;
		instr.addr = builder->bytestream.memory + func->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_call))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_JMP:
	{
		const vmp_instr_def_jmp* const cmd = (vmp_instr_def_jmp*)h;
		const vmp_marker* const marker = cmd->marker;

		vmi_instr_jmp instr;
		instr.opcode = 0;
		instr.icode = VMI_JMP;
		instr.props1 = cmd->op;
		instr.destination = builder->bytestream.memory + marker->instr_offset + marker->func->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_jmp))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_ADD:
	{
		const vmp_instr_def_add* const cmd = (vmp_instr_def_add*)h;

		vmi_instr_add instr;
		instr.opcode = 0;
		instr.icode = VMI_ADD;
		instr.props1 = cmd->type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_add))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_CMP:
	{
		const vmp_instr_def_cmp* const cmd = (vmp_instr_def_cmp*)h;

		vmi_instr_cmp instr;
		instr.opcode = 0;
		instr.icode = VMI_CMP;
		instr.props1 = cmd->comparator;
		instr.props2 = VMI_INSTR_CMP_PROP2_SIGNED;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_cmp))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_RET:
	{
		const vmp_instr_def_ret* const cmd = (vmp_instr_def_ret*)h;

		vmi_instr_ret instr;
		instr.opcode = 0;
		instr.icode = VMI_RET;
		instr.pop_stack_size = func->args_stack_size;
		instr.pop_locals_size = func->locals_stack_size;
#if defined(VM_STACK_DEBUG)
		// The values pushed on the stack when the function starts are
		// 1. Pointer to the return address location
		// 2. The previous ebp
		// 3. return values
		// 4. arguments
		instr.expected_ebp_offset = sizeof(vmi_ip) + sizeof(vm_byte*) + func->returns_stack_size + func->args_stack_size;
#endif
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ret))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_EOE:
	{
		vmi_instr_eoe instr;
		instr.opcode = 0;
		instr.icode = VMI_EOE;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_eoe))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_STR:
	{
		const vmp_instr_def_str* const cmd = (vmp_instr_def_str*)h;
		const vmp_return* const ret = vmp_list_returns_get(&func->returns, cmd->index);
		if (ret == NULL) {
			vmp_builder_message_return_index_missing(builder, cmd->index);
			return NULL;
		}

		vmi_instr_str instr;
		instr.opcode = 0;
		instr.icode = VMI_STR;
		instr.size = ret->type->size;
		instr.offset = ret->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_str))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_CONV:
	{
		const vmp_instr_def_conv* const cmd = (vmp_instr_def_conv*)h;
		if (vmp_type_can_convert(cmd->from_type, cmd->to_type) == FALSE) {
			vmp_builder_message_types_not_compatible(builder, &cmd->from_type->name, &cmd->to_type->name);
			return NULL;
		}

		vmi_instr_conv instr;
		instr.opcode = 0;
		instr.icode = VMI_CONV;
		instr.props1 = cmd->from_type->data_type;
		instr.props2 = cmd->to_type->data_type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_conv))) {
			return NULL;
		}
		break;
	}
	default:
		vmp_builder_message_unknown_instr(builder, h->instr_type);
		return NULL;
	}
	return h->next;
}
