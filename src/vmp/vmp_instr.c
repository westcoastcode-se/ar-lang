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

vmp_constant vmp_const_i2(vm_int16 value)
{
	vmp_constant c;
	c.i2 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT16;
	return c;
}

vmp_constant vmp_const_i4(vm_int32 value)
{
	vmp_constant c;
	c.i4 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT32;
	return c;
}

vmp_constant vmp_const_i8(vm_int64 value)
{
	vmp_constant c;
	c.i8 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT64;
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

vmp_instr* vmp_instr_str(vm_uint32 index)
{
	vmp_instr_def_str* instr = (vmp_instr_def_str*)vmc_malloc(sizeof(vmp_instr_def_str));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STR, sizeof(vmi_instr_lda));
	instr->index = index;
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

vmp_instr* vmp_instr_add(vm_int8 type)
{
	vmp_instr_def_add* instr = (vmp_instr_def_add*)vmc_malloc(sizeof(vmp_instr_def_add));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ADD, sizeof(vmi_instr_add));
	instr->type = type;
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
	default:
		vmp_builder_message_unknown_instr(builder, h->instr_type);
		return NULL;
	}
	return h->next;
}
