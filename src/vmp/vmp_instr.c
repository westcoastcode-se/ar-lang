#include "vmp_instr.h"
#include "vmp.h"
#include "vmp_list_args.h"
#include "vmp_messages.h"
#include "../vm_debug.h"
#include <inttypes.h>

vmp_instr* vmp_instr_lda(const vmp_arg* arg)
{
	vmp_instr_def_lda* instr = (vmp_instr_def_lda*)vm_malloc(sizeof(vmp_instr_def_lda));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDA, sizeof(vmi_instr_lda));
	instr->arg = arg;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_lda_a(const vmp_arg* arg)
{
	vmp_instr_def_lda_a* instr = (vmp_instr_def_lda_a*)vm_malloc(sizeof(vmp_instr_def_lda_a));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDA_A, sizeof(vmi_instr_lda_a));
	instr->arg = arg;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldl(const vmp_local* local)
{
	vmp_instr_def_ldl* instr = (vmp_instr_def_ldl*)vm_malloc(sizeof(vmp_instr_def_ldl));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDL, sizeof(vmi_instr_ldl));
	instr->local = local;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldl_a(const vmp_local* local)
{
	vmp_instr_def_ldl_a* instr = (vmp_instr_def_ldl_a*)vm_malloc(sizeof(vmp_instr_def_ldl_a));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDL_A, sizeof(vmi_instr_ldl_a));
	instr->local = local;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldf(const vmp_func* func)
{
	vmp_instr_def_ldf* instr = (vmp_instr_def_ldf*)vm_malloc(sizeof(vmp_instr_def_ldf));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDF, sizeof(vmi_instr_ldf));
	instr->target_func = func;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_stl(const vmp_local* local)
{
	vmp_instr_def_stl* instr = (vmp_instr_def_stl*)vm_malloc(sizeof(vmp_instr_def_stl));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STL, sizeof(vmi_instr_stl));
	instr->local = local;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_locals(const vmp_func* func)
{
	vmp_instr_def_locals* instr = (vmp_instr_def_locals*)vm_malloc(sizeof(vmp_instr_def_locals));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LOCALS, sizeof(vmi_instr_locals));
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldc(const vmp_type* type, vmp_const constant)
{
	switch (constant.type)
	{
	case VMI_INSTR_PROP_I64:
	case VMI_INSTR_PROP_UI64:
	case VMI_INSTR_PROP_F64:
	{
		vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)vm_malloc(sizeof(vmp_instr_def_ldc));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC_I8, sizeof(vmi_instr_ldc_i64));
		instr->type = type;
		instr->constant = constant;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
	default:
	{

		vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)vm_malloc(sizeof(vmp_instr_def_ldc));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC, sizeof(vmi_instr_ldc_i32));
		instr->type = type;
		instr->constant = constant;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
	}
}

vmp_instr* vmp_instr_ldc_s(const vmp_type* type, vmp_const constant)
{
	vmp_instr_def_ldc_s* instr = (vmp_instr_def_ldc_s*)vm_malloc(sizeof(vmp_instr_def_ldc_s));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC_S, sizeof(vmi_instr_ldc_s));
	instr->type = type;
	instr->constant = constant;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldg(vmp_global* g)
{
	vmp_instr_def_ldg* instr = (vmp_instr_def_ldg*)vm_malloc(sizeof(vmp_instr_def_ldg));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDG, sizeof(vmi_instr_ldg));
	instr->global = g;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_ldg_a(vmp_global* g)
{
	vmp_instr_def_ldg_a* instr = (vmp_instr_def_ldg_a*)vm_malloc(sizeof(vmp_instr_def_ldg_a));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDG_A, sizeof(vmi_instr_ldg_a));
	instr->global = g;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_stg(vmp_global* g)
{
	vmp_instr_def_stg* instr = (vmp_instr_def_stg*)vm_malloc(sizeof(vmp_instr_def_stg));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STG, sizeof(vmi_instr_stg));
	instr->global = g;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_allocs(const vmp_type* type)
{
	vmp_instr_def_allocs* instr = (vmp_instr_def_allocs*)vm_malloc(sizeof(vmp_instr_def_allocs));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ALLOCS, sizeof(vmi_instr_allocs));
	instr->type = type;
	instr->amount = 0;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_allocs_const(vm_int16 amount)
{
	vmp_instr_def_allocs* instr = (vmp_instr_def_allocs*)vm_malloc(sizeof(vmp_instr_def_allocs));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ALLOCS, sizeof(vmi_instr_allocs));
	instr->type = NULL;
	instr->amount = amount;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vm_int32 vmp_instr_allocs_get_size(const vmp_instr_def_frees* cmd)
{
	if (cmd->type == NULL && cmd->amount <= 0) {
		return 0;
	}

	if (cmd->type != NULL)
		return cmd->type->size;
	else
		return cmd->amount;
}

vmp_instr* vmp_instr_frees(const vmp_type* type)
{
	vmp_instr_def_frees* instr = (vmp_instr_def_frees*)vm_malloc(sizeof(vmp_instr_def_frees));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_FREES, sizeof(vmi_instr_frees));
	instr->type = type;
	instr->amount = 0;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_frees_const(vm_int16 amount)
{
	vmp_instr_def_frees* instr = (vmp_instr_def_frees*)vm_malloc(sizeof(vmp_instr_def_frees));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_FREES, sizeof(vmi_instr_frees));
	instr->type = NULL;
	instr->amount = amount;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_alloch(const vmp_type* type)
{
	vmp_instr_def_alloch* instr = (vmp_instr_def_alloch*)vm_malloc(sizeof(vmp_instr_def_alloch));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ALLOCH, sizeof(vmi_instr_alloch));
	instr->type = NULL;
	instr->amount = 0;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_alloch_type(const vmp_type* type)
{
	vmp_instr_def_alloch* instr = (vmp_instr_def_alloch*)vm_malloc(sizeof(vmp_instr_def_alloch));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ALLOCH_C, sizeof(vmi_instr_alloch_c));
	instr->type = type;
	instr->amount = 0;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_alloch_const(vm_int16 amount)
{
	vmp_instr_def_alloch* instr = (vmp_instr_def_alloch*)vm_malloc(sizeof(vmp_instr_def_alloch));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ALLOCH_C, sizeof(vmi_instr_alloch_c));
	instr->type = NULL;
	instr->amount = amount;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_freeh()
{
	vmp_instr_def_freeh* instr = (vmp_instr_def_freeh*)vm_malloc(sizeof(vmp_instr_def_freeh));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_FREEH, sizeof(vmi_instr_freeh));
	instr->type = NULL;
	instr->amount = 0;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_sturef(const vmp_type* type)
{
	ASSERT_NOT_NULL(type);

	if (type->size > UINT8_MAX) {
		vmp_instr_def_sturef* instr = (vmp_instr_def_sturef*)vm_malloc(sizeof(vmp_instr_def_sturef));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STUREF, sizeof(vmi_instr_sturef));
		instr->type = type;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
	else {
		vmp_instr_def_sturef_s* instr = (vmp_instr_def_sturef_s*)vm_malloc(sizeof(vmp_instr_def_sturef_s));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STUREF_S, sizeof(vmi_instr_sturef_s));
		instr->type = type;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}	
}

vmp_instr* vmp_instr_stelem(const vmp_type* type)
{
	ASSERT_NOT_NULL(type);

	if (type->of_type->size > UINT8_MAX) {
		vmp_instr_def_stelem* instr = (vmp_instr_def_stelem*)vm_malloc(sizeof(vmp_instr_def_stelem));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STELEM, sizeof(vmi_instr_stelem));
		instr->type = type;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
	else {
		vmp_instr_def_stelem_s* instr = (vmp_instr_def_stelem_s*)vm_malloc(sizeof(vmp_instr_def_stelem_s));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_STELEM_S, sizeof(vmi_instr_stelem_s));
		instr->type = type;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
}

vmp_instr* vmp_instr_ldelem(const vmp_type* type)
{
	ASSERT_NOT_NULL(type);

	if (type->of_type->size > UINT8_MAX) {
		vmp_instr_def_ldelem* instr = (vmp_instr_def_ldelem*)vm_malloc(sizeof(vmp_instr_def_ldelem));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDELEM, sizeof(vmi_instr_ldelem));
		instr->type = type;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
	else {
		vmp_instr_def_ldelem_s* instr = (vmp_instr_def_ldelem_s*)vm_malloc(sizeof(vmp_instr_def_ldelem_s));
		if (instr == NULL)
			return NULL;
		VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDELEM_S, sizeof(vmi_instr_ldelem_s));
		instr->type = type;
		return VMC_PIPELINE_INSTR_BASE(instr);
	}
}

vmp_instr* vmp_instr_ldc_i8(const vmp_type* type, vmp_const constant)
{
	ASSERT_NOT_NULL(type);

	vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)vm_malloc(sizeof(vmp_instr_def_ldc));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_LDC_I8, sizeof(vmi_instr_ldc_i64));
	instr->type = type;
	instr->constant = constant;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_call(const vmp_func* func)
{
	vmp_instr_def_call* instr = (vmp_instr_def_call*)vm_malloc(sizeof(vmp_instr_def_call));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CALL, sizeof(vmi_instr_call));
	instr->target_func = func;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_callnative(const vmp_func* func)
{
	vmp_instr_def_callnative* instr = (vmp_instr_def_callnative*)vm_malloc(sizeof(vmp_instr_def_callnative));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CALLNATIVE, sizeof(vmi_instr_callnative));
	instr->target_func = func;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_calluref(const vmp_func* func)
{
	vmp_instr_def_calluref* instr = (vmp_instr_def_calluref*)vm_malloc(sizeof(vmp_instr_def_calluref));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CALLUREF, sizeof(vmi_instr_calluref));
	instr->func_def = func;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_add(const vmp_type* type)
{
	vmp_instr_def_add* instr = (vmp_instr_def_add*)vm_malloc(sizeof(vmp_instr_def_add));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_ADD, sizeof(vmi_instr_add));
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_sub(const vmp_type* type)
{
	vmp_instr_def_sub* instr = (vmp_instr_def_sub*)vm_malloc(sizeof(vmp_instr_def_sub));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_SUB, sizeof(vmi_instr_sub));
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_mul(const vmp_type* type)
{
	vmp_instr_def_mul* instr = (vmp_instr_def_mul*)vm_malloc(sizeof(vmp_instr_def_mul));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_MUL, sizeof(vmi_instr_mul));
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_div(const vmp_type* type)
{
	vmp_instr_def_div* instr = (vmp_instr_def_div*)vm_malloc(sizeof(vmp_instr_def_div));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_DIV, sizeof(vmi_instr_div));
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_clt(const vmp_type* type)
{
	vmp_instr_def_cmp* instr = (vmp_instr_def_cmp*)vm_malloc(sizeof(vmp_instr_def_cmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CMP, sizeof(vmi_instr_cmp));
	instr->comparator = VMI_INSTR_CMP_PROP1_LT;
	instr->compare_type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_cgt(const vmp_type* type)
{
	vmp_instr_def_cmp* instr = (vmp_instr_def_cmp*)vm_malloc(sizeof(vmp_instr_def_cmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CMP, sizeof(vmi_instr_cmp));
	instr->comparator = VMI_INSTR_CMP_PROP1_GT;
	instr->compare_type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_jmpt(const vmp_marker* marker)
{
	vmp_instr_def_jmp* instr = (vmp_instr_def_jmp*)vm_malloc(sizeof(vmp_instr_def_jmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_JMP, sizeof(vmi_instr_jmp));
	instr->op = VMI_INSTR_JMP_PROP1_TRUE;
	instr->marker = marker;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_jmpf(const vmp_marker* marker)
{
	vmp_instr_def_jmp* instr = (vmp_instr_def_jmp*)vm_malloc(sizeof(vmp_instr_def_jmp));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_JMP, sizeof(vmi_instr_jmp));
	instr->op = VMI_INSTR_JMP_PROP1_FALSE;
	instr->marker = marker;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_conv(const vmp_type* from, const vmp_type* to)
{
	vmp_instr_def_conv* instr = (vmp_instr_def_conv*)vm_malloc(sizeof(vmp_instr_def_conv));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_CONV, sizeof(vmi_instr_conv));
	instr->from_type = from;
	instr->to_type = to;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_neg(const vmp_type* type)
{
	vmp_instr_def_neg* instr = (vmp_instr_def_neg*)vm_malloc(sizeof(vmp_instr_def_neg));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_NEG, sizeof(vmi_instr_neg));
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_bit(vm_int32 op, const vmp_type* type)
{
	vmp_instr_def_bit* instr = (vmp_instr_def_bit*)vm_malloc(sizeof(vmp_instr_def_bit));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, VMP_INSTR_BIT, sizeof(vmi_instr_bit));
	instr->op = op;
	instr->type = type;
	return VMC_PIPELINE_INSTR_BASE(instr);
}

vmp_instr* vmp_instr_bit_not(const vmp_type* type)
{
	return vmp_instr_bit(VMI_INSTR_BIT_PROPS2_NOT, type);
}

vmp_instr* vmp_instr_basic(vmp_instr_type type, vm_int32 size)
{
	vmp_instr_def_basic* instr = (vmp_instr_def_basic*)vm_malloc(sizeof(vmp_instr_def_basic));
	if (instr == NULL)
		return NULL;
	VMC_PIPELINE_INIT_HEADER(instr, type, size);
	return VMC_PIPELINE_INSTR_BASE(instr);
}

BOOL vmp_instr_test_prev_count(const vmp_instr* instr, vm_int32 count)
{
	ASSERT_NOT_NULL(instr);

	vm_int32 i;
	for (i = 0; i < count; ++i) {
		if (instr == NULL) {
			return FALSE;
		}
		instr = instr->prev;
	}
	return TRUE;
}

BOOL vmp_instr_ldc_i8_leq(const vmp_instr* instr, BOOL ret_if_not_constant, vm_int64 value)
{
	ASSERT_NOT_NULL(instr);

	switch (instr->instr_type)
	{
	case VMP_INSTR_LDC_S:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		if (cmd->constant.i16 > value)
			return FALSE;
		break;
	}
	case VMP_INSTR_LDC:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		if (cmd->constant.i32 > value)
			return FALSE;
		break;
	}
	case VMP_INSTR_LDC_I8:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		if (cmd->constant.i64 > value)
			return FALSE;
		break;
	}
	default:
		return ret_if_not_constant;
	}
	return TRUE;
}

BOOL vmp_instr_test_type_eq(const vmp_instr* instr1, const vmp_instr* instr2)
{
	if (instr2 == NULL)
		return FALSE;
	return instr1->instr_type == instr2->instr_type;
}

vm_int64 vmp_instr_ldc_i8_get(const vmp_instr* instr)
{
	ASSERT_NOT_NULL(instr);
	switch (instr->instr_type)
	{
	case VMP_INSTR_LDC_S:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		return (vm_int64)cmd->constant.i16;
	}
	case VMP_INSTR_LDC:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		return (vm_int64)cmd->constant.i32;
	}
	case VMP_INSTR_LDC_I8:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		return (vm_int64)cmd->constant.i64;
	}
	default:
		return -1;
	}
	return TRUE;
}

vmp_instr* vmp_instr_link(vmp_instr* instr, vmp_instr* next)
{
	if (instr == NULL)
		return next;
	if (instr->next != NULL) {
		next->next = instr->next;
		instr->next->prev = next;
	}
	instr->next = next;
	next->prev = instr;
	return instr;
}

vmp_instr* vmp_instr_get_last(vmp_instr* instr)
{
	while (instr->next != NULL) {
		instr = instr->next;
	}
	return instr;
}

const vmp_instr* vmp_instr_build(const vmp_instr* h, struct vmp_builder* builder)
{
	const vmp_func* const func = h->func;

	switch (h->instr_type)
	{
	case VMP_INSTR_LDA:
	{
		const vmp_instr_def_lda* const cmd = (vmp_instr_def_lda*)h;
		const vmp_arg* const arg = cmd->arg;
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
		const vmp_arg* const arg = cmd->arg;
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
		instr.props1 = cmd->type->data_type;
		instr.i32 = cmd->constant.i32;
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
		instr.props1 = cmd->type->data_type;
		instr.i16 = cmd->constant.i16;
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
		instr.props1 = cmd->type->data_type;
		instr.i64 = cmd->constant.i64;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldc_i64))) {
			break;
		}
		break;
	}
	case VMP_INSTR_STL:
	{
		const vmp_instr_def_stl* const cmd = (vmp_instr_def_stl*)h;
		const vmp_local* const local = cmd->local;

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
		const vmp_local* const local = cmd->local;

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
		const vmp_local* const local = cmd->local;

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
	case VMP_INSTR_LDF:
	{
		const vmp_instr_def_ldf* const cmd = (vmp_instr_def_ldf*)h;

		vmi_instr_ldf instr;
		instr.opcode = 0;
		instr.icode = VMI_LDF;
		instr.addr = builder->bytestream.memory + cmd->target_func->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldf))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LOCALS:
	{
		const vmp_instr_def_locals* const cmd = (vmp_instr_def_locals*)h;
		if (h->func->locals.count > 0) {
			vmi_instr_locals instr;
			instr.opcode = 0;
			instr.icode = VMI_LOCALS;
			instr.size = h->func->locals_stack_size;
			if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_locals))) {
				return NULL;
			}
		}
		break;
	}
	case VMP_INSTR_LDG:
	{
		const vmp_instr_def_ldg* const cmd = (vmp_instr_def_ldg*)h;

		vmi_instr_ldg instr;
		instr.opcode = 0;
		instr.icode = VMI_LDG;
		instr.size = cmd->global->type->size;
		instr.addr = builder->bytestream.memory + cmd->global->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldg))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDG_A:
	{
		const vmp_instr_def_ldg_a* const cmd = (vmp_instr_def_ldg_a*)h;

		vmi_instr_ldg_a instr;
		instr.opcode = 0;
		instr.icode = VMI_LDG_A;
		instr.size = cmd->global->type->size;
		instr.addr = builder->bytestream.memory + cmd->global->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldg_a))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_STG:
	{
		const vmp_instr_def_stg* const cmd = (vmp_instr_def_stg*)h;

		vmi_instr_stg instr;
		instr.opcode = 0;
		instr.icode = VMI_STG;
		instr.size = cmd->global->type->size;
		instr.addr = builder->bytestream.memory + cmd->global->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_stg))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_ALLOCS:
	{
		const vmp_instr_def_allocs* const cmd = (vmp_instr_def_allocs*)h;
		if (cmd->type == NULL && cmd->amount <= 0) {
			vmp_builder_message_expected_const_larger_than(builder, 0, 0);
			break;
		}

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
	case VMP_INSTR_FREES:
	{
		const vmp_instr_def_frees* cmd = (vmp_instr_def_frees*)h;
		vm_uint32 size = vmp_instr_allocs_get_size(cmd);
		if (size <= 0) {
			vmp_builder_message_expected_const_larger_than(builder, 0, 0);
			break;
		}
		else if (size > UINT16_MAX) {
			vmp_builder_message_expected_const_smaller_than(builder, 0, UINT16_MAX);
			break;
		}

		if (cmd->type == NULL && cmd->amount <= 0) {
			vmp_builder_message_expected_const_larger_than(builder, 0, 0);
			break;
		}

		// Merge adjacent frees into one instruction
		if (builder->opt_level > 0) {
			while (vmp_instr_test_type_eq(VMC_PIPELINE_INSTR_BASE(cmd), cmd->next)) {
				cmd = (vmp_instr_def_frees*)cmd->next;
				const vm_int16 merge_size = vmp_instr_allocs_get_size(cmd);
				if (merge_size <= 0) {
					vmp_builder_message_expected_const_larger_than(builder, 0, 0);
					break;
				}
				else if (merge_size > UINT16_MAX) {
					vmp_builder_message_expected_const_smaller_than(builder, 0, UINT16_MAX);
					break;
				}
				size += merge_size;
			}
			// Validation if merged
			if (size > UINT16_MAX) {
				vmp_builder_message_expected_const_smaller_than(builder, 0, UINT16_MAX);
				break;
			}
		}

		vmi_instr_frees instr;
		instr.opcode = 0;
		instr.icode = VMI_FREES;
		instr.size = (vm_uint16)size;
		
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_frees))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_ALLOCH:
	{
		const vmp_instr_def_alloch* const cmd = (vmp_instr_def_alloch*)h;

		// requires:
		// -1 = value of size(int)
		if (!vmp_instr_test_prev_count(h, 1)) {
			vmp_builder_message_instr_requires_prev_instr(builder, "alloch");
			break;
		}

		vmi_instr_alloch instr;
		instr.opcode = 0;
		instr.icode = VMI_ALLOCH;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_alloch))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_ALLOCH_C:
	{
		const vmp_instr_def_alloch* const cmd = (vmp_instr_def_alloch*)h;
		if (cmd->type == NULL && cmd->amount <= 0) {
			vmp_builder_message_expected_const_larger_than(builder, 0, 0);
			break;
		}

		vmi_instr_alloch_c instr;
		instr.opcode = 0;
		instr.icode = VMI_ALLOCH_C;
		if (cmd->type != NULL)
			instr.size = cmd->type->size;
		else
			instr.size = cmd->amount;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_alloch_c))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_FREEH:
	{
		const vmp_instr_def_freeh* const cmd = (vmp_instr_def_freeh*)h;

		vmi_instr_freeh instr;
		instr.opcode = 0;
		instr.icode = VMI_FREEH;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_freeh))) {
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
	case VMP_INSTR_STUREF_S:
	{
		const vmp_instr_def_sturef_s* const cmd = (vmp_instr_def_sturef_s*)h;
		if (cmd->type->size > UINT8_MAX) {
			vmp_builder_message_type_too_large(builder, &cmd->type->name, cmd->type->size, UINT8_MAX);
			break;
		}

		vmi_instr_sturef_s instr;
		instr.opcode = 0;
		instr.icode = VMI_STUREF_S;
		instr.size = cmd->type->size;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_sturef_s))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_STELEM:
	{
		const vmp_instr_def_stelem* const cmd = (vmp_instr_def_stelem*)h;
		const vmp_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!vmp_instr_test_prev_count(h, 2)) {
			vmp_builder_message_instr_requires_prev_instr(builder, "stelem");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_ARRAY)) {
			const vm_int64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!vmp_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				vmp_builder_message_expected_const_smaller_than(builder, vmp_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			vmp_builder_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_stelem instr;
		instr.opcode = 0;
		instr.icode = VMI_STELEM;
		instr.size_per_element = cmd->type->of_type->size;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_stelem))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_STELEM_S:
	{
		const vmp_instr_def_stelem_s* const cmd = (vmp_instr_def_stelem_s*)h;
		const vmp_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!vmp_instr_test_prev_count(h, 2)) {
			vmp_builder_message_instr_requires_prev_instr(builder, "stelem_s");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_ARRAY)) {
			const vm_int64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!vmp_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				vmp_builder_message_expected_const_smaller_than(builder, vmp_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			vmp_builder_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_stelem_s instr;
		instr.opcode = 0;
		instr.icode = VMI_STELEM_S;
		instr.size = cmd->type->of_type->size;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_stelem_s))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDELEM:
	{
		const vmp_instr_def_ldelem* const cmd = (vmp_instr_def_ldelem*)h;
		const vmp_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!vmp_instr_test_prev_count(h, 2)) {
			vmp_builder_message_instr_requires_prev_instr(builder, "ldelem");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_ARRAY)) {
			const vm_int64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!vmp_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				vmp_builder_message_expected_const_smaller_than(builder, vmp_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			vmp_builder_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_ldelem instr;
		instr.opcode = 0;
		instr.icode = VMI_LDELEM;
		instr.size_per_element = cmd->type->of_type->size;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldelem))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_LDELEM_S:
	{
		const vmp_instr_def_ldelem_s* const cmd = (vmp_instr_def_ldelem_s*)h;
		const vmp_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!vmp_instr_test_prev_count(h, 2)) {
			vmp_builder_message_instr_requires_prev_instr(builder, "ldelem_s");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_ARRAY)) {
			const vm_int64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!vmp_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				vmp_builder_message_expected_const_smaller_than(builder, vmp_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, VMP_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			vmp_builder_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_ldelem_s instr;
		instr.opcode = 0;
		instr.icode = VMI_LDELEM_S;
		instr.size = cmd->type->of_type->size;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_ldelem_s))) {
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
		instr.expected_stack_size = func->args_stack_size;
		instr.addr = builder->bytestream.memory + func->offset;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_call))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_CALLNATIVE:
	{
		const vmp_instr_def_callnative* const cmd = (vmp_instr_def_callnative*)h;

		vmi_instr_callnative instr;
		instr.opcode = 0;
		instr.icode = VMI_CALLNATIVE;
		instr.expected_stack_size = cmd->target_func->args_stack_size;
		instr.func_ptr = cmd->target_func->native_func;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_call))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_CALLUREF:
	{
		const vmp_instr_def_calluref* const cmd = (vmp_instr_def_calluref*)h;

		vmi_instr_calluref instr;
		instr.opcode = 0;
		instr.icode = VMI_CALLUREF;
		instr.expected_stack_size = cmd->func_def->args_stack_size;
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
		instr.props1 = cmd->type->data_type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_add))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_SUB:
	{
		const vmp_instr_def_sub* const cmd = (vmp_instr_def_sub*)h;

		vmi_instr_sub instr;
		instr.opcode = 0;
		instr.icode = VMI_SUB;
		instr.props1 = cmd->type->data_type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_sub))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_MUL:
	{
		const vmp_instr_def_mul* const cmd = (vmp_instr_def_mul*)h;

		vmi_instr_mul instr;
		instr.opcode = 0;
		instr.icode = VMI_MUL;
		instr.props1 = cmd->type->data_type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_mul))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_DIV:
	{
		const vmp_instr_def_div* const cmd = (vmp_instr_def_div*)h;

		vmi_instr_div instr;
		instr.opcode = 0;
		instr.icode = VMI_DIV;
		instr.props1 = cmd->type->data_type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_div))) {
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
		if (func->args_stack_size > UINT16_MAX) {
			vmp_builder_message_not_implemented(builder, "arguments stack size larger than UINT16_MAX");
			break;
		}
		if (func->locals_stack_size > UINT16_MAX) {
			vmp_builder_message_not_implemented(builder, "local stack size larger than UINT16_MAX");
			break;
		}

		vmi_instr_ret instr;
		instr.opcode = 0;
		instr.icode = VMI_RET;
		instr.pop_locals_size = func->locals_stack_size;
#if defined(VM_STACK_DEBUG)
		// The values pushed on the stack when the function starts are
		// 1. arguments
		// 2. return values
		instr.expected_ebp_offset = func->returns_stack_size + func->args_stack_size;
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
	case VMP_INSTR_CONV:
	{
		const vmp_instr_def_conv* const cmd = (vmp_instr_def_conv*)h;
		if (vmp_type_can_convert(cmd->from_type, cmd->to_type) == FALSE) {
			vmp_builder_message_types_not_compatible(builder, &cmd->from_type->name, &cmd->to_type->name);
			break;
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
	case VMP_INSTR_NEG:
	{
		const vmp_instr_def_neg* const cmd = (vmp_instr_def_neg*)h;

		vmi_instr_neg instr;
		instr.opcode = 0;
		instr.icode = VMI_NEG;
		instr.props1 = cmd->type->data_type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_neg))) {
			return NULL;
		}
		break;
	}
	case VMP_INSTR_BIT:
	{
		const vmp_instr_def_bit* const cmd = (vmp_instr_def_bit*)h;

		vmi_instr_bit instr;
		instr.opcode = 0;
		instr.icode = VMI_BIT;
		instr.props1 = cmd->op;
		instr.props2 = cmd->type->data_type;
		if (!vmp_builder_write(builder, &instr, sizeof(vmi_instr_bit))) {
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
