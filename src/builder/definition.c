#include "definition.h"
#include "messages.h"
#include "../arBuilder.h"
#include "../arMemory.h"
#include <inttypes.h>

void arB_definition_init(arB_definition* instr, arIncode icode, arInt32 size)
{
	instr->func = NULL;
	instr->instr_type = icode;
	instr->instr_size = size;
	instr->instr_offset = 0;
	instr->next = instr->prev = NULL;
}

arB_definition* arB_instr_lda(const arB_arg* arg)
{
	vmp_instr_def_lda* instr = (vmp_instr_def_lda*)arMalloc(sizeof(vmp_instr_def_lda));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDA, sizeof(arInstruction_lda));
	instr->arg = arg;
	return asB_definition(instr);
}

arB_definition* arB_instr_lda_a(const arB_arg* arg)
{
	vmp_instr_def_lda_a* instr = (vmp_instr_def_lda_a*)arMalloc(sizeof(vmp_instr_def_lda_a));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDA_A, sizeof(arInstruction_lda_a));
	instr->arg = arg;
	return asB_definition(instr);
}

arB_definition* arB_instr_ldl(const arB_local* local)
{
	vmp_instr_def_ldl* instr = (vmp_instr_def_ldl*)arMalloc(sizeof(vmp_instr_def_ldl));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDL, sizeof(arInstruction_ldl));
	instr->local = local;
	return asB_definition(instr);
}

arB_definition* arB_instr_ldl_a(const arB_local* local)
{
	vmp_instr_def_ldl_a* instr = (vmp_instr_def_ldl_a*)arMalloc(sizeof(vmp_instr_def_ldl_a));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDL_A, sizeof(arInstruction_ldl_a));
	instr->local = local;
	return asB_definition(instr);
}

arB_definition* arB_instr_ldf(const arB_func* func)
{
	vmp_instr_def_ldf* instr = (vmp_instr_def_ldf*)arMalloc(sizeof(vmp_instr_def_ldf));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDF, sizeof(vmi_instr_ldf));
	instr->target_func = func;
	return asB_definition(instr);
}

arB_definition* arB_instr_stl(const arB_local* local)
{
	vmp_instr_def_stl* instr = (vmp_instr_def_stl*)arMalloc(sizeof(vmp_instr_def_stl));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_STL, sizeof(arInstruction_stl));
	instr->local = local;
	return asB_definition(instr);
}

arB_definition* arB_instr_locals(const arB_func* func)
{
	vmp_instr_def_locals* instr = (vmp_instr_def_locals*)arMalloc(sizeof(vmp_instr_def_locals));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LOCALS, sizeof(vmi_instr_locals));
	return asB_definition(instr);
}

arB_definition* arB_instr_ldc(const arB_type* type, arPrimitiveValue constant)
{
	switch (constant.type)
	{
	case ARLANG_PRIMITIVE_I64:
	case ARLANG_PRIMITIVE_UI64:
	case ARLANG_PRIMITIVE_F64:
	{
		vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)arMalloc(sizeof(vmp_instr_def_ldc));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_LDC_I8, sizeof(vmi_instr_ldc_i64));
		instr->type = type;
		instr->constant = constant;
		return asB_definition(instr);
	}
	default:
	{

		vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)arMalloc(sizeof(vmp_instr_def_ldc));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_LDC, sizeof(vmi_instr_ldc_i32));
		instr->type = type;
		instr->constant = constant;
		return asB_definition(instr);
	}
	}
}

arB_definition* vmp_instr_ldc_i16(const arB_type* type, arPrimitiveValue constant)
{
	vmp_instr_def_ldc_s* instr = (vmp_instr_def_ldc_s*)arMalloc(sizeof(vmp_instr_def_ldc_s));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDC_S, sizeof(vmi_instr_ldc_s));
	instr->type = type;
	instr->constant = constant;
	return asB_definition(instr);
}

arB_definition* arB_instr_ldg(arB_global* g)
{
	vmp_instr_def_ldg* instr = (vmp_instr_def_ldg*)arMalloc(sizeof(vmp_instr_def_ldg));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDG, sizeof(arInstruction_ldg));
	instr->global = g;
	return asB_definition(instr);
}

arB_definition* arB_instr_ldg_a(arB_global* g)
{
	vmp_instr_def_ldg_a* instr = (vmp_instr_def_ldg_a*)arMalloc(sizeof(vmp_instr_def_ldg_a));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDG_A, sizeof(arInstruction_ldg_a));
	instr->global = g;
	return asB_definition(instr);
}

arB_definition* arB_instr_stg(arB_global* g)
{
	vmp_instr_def_stg* instr = (vmp_instr_def_stg*)arMalloc(sizeof(vmp_instr_def_stg));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_STG, sizeof(arInstruction_stg));
	instr->global = g;
	return asB_definition(instr);
}

arB_definition* arB_instr_allocs(const arB_type* type)
{
	vmp_instr_def_allocs* instr = (vmp_instr_def_allocs*)arMalloc(sizeof(vmp_instr_def_allocs));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_ALLOCS, sizeof(vmi_instr_allocs));
	instr->type = type;
	instr->amount = 0;
	return asB_definition(instr);
}

arB_definition* arB_instr_allocs_const(arInt16 amount)
{
	vmp_instr_def_allocs* instr = (vmp_instr_def_allocs*)arMalloc(sizeof(vmp_instr_def_allocs));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_ALLOCS, sizeof(vmi_instr_allocs));
	instr->type = NULL;
	instr->amount = amount;
	return asB_definition(instr);
}

arInt32 arB_instr_allocs_get_size(const vmp_instr_def_frees* cmd)
{
	if (cmd->type == NULL && cmd->amount <= 0) {
		return 0;
	}

	if (cmd->type != NULL)
		return cmd->type->size;
	else
		return cmd->amount;
}

arB_definition* arB_instr_frees(const arB_type* type)
{
	vmp_instr_def_frees* instr = (vmp_instr_def_frees*)arMalloc(sizeof(vmp_instr_def_frees));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_FREES, sizeof(vmi_instr_frees));
	instr->type = type;
	instr->amount = 0;
	return asB_definition(instr);
}

arB_definition* arB_instr_frees_const(arInt16 amount)
{
	vmp_instr_def_frees* instr = (vmp_instr_def_frees*)arMalloc(sizeof(vmp_instr_def_frees));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_FREES, sizeof(vmi_instr_frees));
	instr->type = NULL;
	instr->amount = amount;
	return asB_definition(instr);
}

arB_definition* arB_instr_alloch(const arB_type* type)
{
	vmp_instr_def_alloch* instr = (vmp_instr_def_alloch*)arMalloc(sizeof(vmp_instr_def_alloch));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_ALLOCH, sizeof(vmi_instr_alloch));
	instr->type = NULL;
	instr->amount = 0;
	return asB_definition(instr);
}

arB_definition* arB_instr_alloch_type(const arB_type* type)
{
	vmp_instr_def_alloch* instr = (vmp_instr_def_alloch*)arMalloc(sizeof(vmp_instr_def_alloch));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_ALLOCH_C, sizeof(vmi_instr_alloch_c));
	instr->type = type;
	instr->amount = 0;
	return asB_definition(instr);
}

arB_definition* arB_instr_alloch_const(arInt16 amount)
{
	vmp_instr_def_alloch* instr = (vmp_instr_def_alloch*)arMalloc(sizeof(vmp_instr_def_alloch));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_ALLOCH_C, sizeof(vmi_instr_alloch_c));
	instr->type = NULL;
	instr->amount = amount;
	return asB_definition(instr);
}

arB_definition* arB_instr_freeh()
{
	vmp_instr_def_freeh* instr = (vmp_instr_def_freeh*)arMalloc(sizeof(vmp_instr_def_freeh));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_FREEH, sizeof(vmi_instr_freeh));
	instr->type = NULL;
	instr->amount = 0;
	return asB_definition(instr);
}

arB_definition* arB_instr_sturef(const arB_type* type)
{
	ASSERT_NOT_NULL(type);

	if (type->size > UINT8_MAX) {
		vmp_instr_def_sturef* instr = (vmp_instr_def_sturef*)arMalloc(sizeof(vmp_instr_def_sturef));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_STUREF, sizeof(vmi_instr_sturef));
		instr->type = type;
		return asB_definition(instr);
	}
	else {
		vmp_instr_def_sturef_s* instr = (vmp_instr_def_sturef_s*)arMalloc(sizeof(vmp_instr_def_sturef_s));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_STUREF_S, sizeof(vmi_instr_sturef_s));
		instr->type = type;
		return asB_definition(instr);
	}	
}

arB_definition* arB_instr_stelem(const arB_type* type)
{
	ASSERT_NOT_NULL(type);

	if (type->of_type->size > UINT8_MAX) {
		vmp_instr_def_stelem* instr = (vmp_instr_def_stelem*)arMalloc(sizeof(vmp_instr_def_stelem));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_STELEM, sizeof(vmi_instr_stelem));
		instr->type = type;
		return asB_definition(instr);
	}
	else {
		vmp_instr_def_stelem_s* instr = (vmp_instr_def_stelem_s*)arMalloc(sizeof(vmp_instr_def_stelem_s));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_STELEM_S, sizeof(vmi_instr_stelem_s));
		instr->type = type;
		return asB_definition(instr);
	}
}

arB_definition* arB_instr_ldelem(const arB_type* type)
{
	ASSERT_NOT_NULL(type);

	if (type->of_type->size > UINT8_MAX) {
		vmp_instr_def_ldelem* instr = (vmp_instr_def_ldelem*)arMalloc(sizeof(vmp_instr_def_ldelem));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_LDELEM, sizeof(vmi_instr_ldelem));
		instr->type = type;
		return asB_definition(instr);
	}
	else {
		vmp_instr_def_ldelem_s* instr = (vmp_instr_def_ldelem_s*)arMalloc(sizeof(vmp_instr_def_ldelem_s));
		if (instr == NULL)
			return NULL;
		arB_definition_init(asB_definition(instr), VMI_LDELEM_S, sizeof(vmi_instr_ldelem_s));
		instr->type = type;
		return asB_definition(instr);
	}
}

arB_definition* arB_instr_ldc_i64(const arB_type* type, arPrimitiveValue constant)
{
	ASSERT_NOT_NULL(type);

	vmp_instr_def_ldc* instr = (vmp_instr_def_ldc*)arMalloc(sizeof(vmp_instr_def_ldc));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_LDC_I8, sizeof(vmi_instr_ldc_i64));
	instr->type = type;
	instr->constant = constant;
	return asB_definition(instr);
}

arB_definition* arB_instr_call(const arB_func* func)
{
	vmp_instr_def_call* instr = (vmp_instr_def_call*)arMalloc(sizeof(vmp_instr_def_call));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_CALL, sizeof(vmi_instr_call));
	instr->target_func = func;
	return asB_definition(instr);
}

arB_definition* arB_instr_callnative(const arB_func* func)
{
	vmp_instr_def_callnative* instr = (vmp_instr_def_callnative*)arMalloc(sizeof(vmp_instr_def_callnative));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_CALLNATIVE, sizeof(vmi_instr_callnative));
	instr->target_func = func;	
	return asB_definition(instr);
}

arB_definition* arB_instr_calluref(const arB_func* func)
{
	vmp_instr_def_calluref* instr = (vmp_instr_def_calluref*)arMalloc(sizeof(vmp_instr_def_calluref));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_CALLUREF, sizeof(vmi_instr_calluref));
	instr->func_def = func;
	return asB_definition(instr);
}

arB_definition* arB_instr_add(const arB_type* type)
{
	vmp_instr_def_add* instr = (vmp_instr_def_add*)arMalloc(sizeof(vmp_instr_def_add));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_ADD, sizeof(vmi_instr_add));
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_sub(const arB_type* type)
{
	vmp_instr_def_sub* instr = (vmp_instr_def_sub*)arMalloc(sizeof(vmp_instr_def_sub));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_SUB, sizeof(vmi_instr_sub));
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_mul(const arB_type* type)
{
	vmp_instr_def_mul* instr = (vmp_instr_def_mul*)arMalloc(sizeof(vmp_instr_def_mul));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_MUL, sizeof(vmi_instr_mul));
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_div(const arB_type* type)
{
	vmp_instr_def_div* instr = (vmp_instr_def_div*)arMalloc(sizeof(vmp_instr_def_div));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_DIV, sizeof(vmi_instr_div));
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_clt(const arB_type* type)
{
	vmp_instr_def_cmp* instr = (vmp_instr_def_cmp*)arMalloc(sizeof(vmp_instr_def_cmp));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_CMP, sizeof(vmi_instr_cmp));
	instr->op = VMI_INSTR_CMP_PROP1_LT;
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_cgt(const arB_type* type)
{
	vmp_instr_def_cmp* instr = (vmp_instr_def_cmp*)arMalloc(sizeof(vmp_instr_def_cmp));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_CMP, sizeof(vmi_instr_cmp));
	instr->op = VMI_INSTR_CMP_PROP1_GT;
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_jmpt(const arB_marker* marker)
{
	vmp_instr_def_jmp* instr = (vmp_instr_def_jmp*)arMalloc(sizeof(vmp_instr_def_jmp));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_JMP, sizeof(vmi_instr_jmp));
	instr->op = VMI_INSTR_JMP_PROP1_TRUE;
	instr->marker = marker;
	return asB_definition(instr);
}

arB_definition* arB_instr_jmpf(const arB_marker* marker)
{
	vmp_instr_def_jmp* instr = (vmp_instr_def_jmp*)arMalloc(sizeof(vmp_instr_def_jmp));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_JMP, sizeof(vmi_instr_jmp));
	instr->op = VMI_INSTR_JMP_PROP1_FALSE;
	instr->marker = marker;
	return asB_definition(instr);
}

arB_definition* arB_instr_conv(const arB_type* from, const arB_type* to)
{
	vmp_instr_def_conv* instr = (vmp_instr_def_conv*)arMalloc(sizeof(vmp_instr_def_conv));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_CONV, sizeof(vmi_instr_conv));
	instr->from_type = from;
	instr->to_type = to;
	return asB_definition(instr);
}

arB_definition* arB_instr_neg(const arB_type* type)
{
	vmp_instr_def_neg* instr = (vmp_instr_def_neg*)arMalloc(sizeof(vmp_instr_def_neg));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_NEG, sizeof(vmi_instr_neg));
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_bit(arInt32 op, const arB_type* type)
{
	vmp_instr_def_bit* instr = (vmp_instr_def_bit*)arMalloc(sizeof(vmp_instr_def_bit));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), VMI_BIT, sizeof(vmi_instr_bit));
	instr->op = op;
	instr->type = type;
	return asB_definition(instr);
}

arB_definition* arB_instr_bit_not(const arB_type* type)
{
	return arB_instr_bit(VMI_INSTR_BIT_PROPS2_NOT, type);
}

arB_definition* arB_instr_empty(arOpcode type, arInt32 size)
{
	vmp_instr_def_basic* instr = (vmp_instr_def_basic*)arMalloc(sizeof(vmp_instr_def_basic));
	if (instr == NULL)
		return NULL;
	arB_definition_init(asB_definition(instr), type, size);
	return asB_definition(instr);
}

arB_definition* arB_instr_ret() 
{
	return arB_instr_empty(VMI_RET, sizeof(vmi_instr_ret));
}

arB_definition* arB_instr_eoe() 
{
	return arB_instr_empty(VMI_EOE, sizeof(vmi_instr_eoe));
}

BOOL arB_instr_test_prev_count(const arB_definition* instr, arInt32 count)
{
	ASSERT_NOT_NULL(instr);

	arInt32 i;
	for (i = 0; i < count; ++i) {
		if (instr == NULL) {
			return FALSE;
		}
		instr = instr->prev;
	}
	return TRUE;
}

BOOL arB_instr_ldc_i8_leq(const arB_definition* instr, BOOL ret_if_not_constant, arInt64 value)
{
	ASSERT_NOT_NULL(instr);

	switch (instr->instr_type)
	{
	case VMI_LDC_S:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		if (cmd->constant.i16 > value)
			return FALSE;
		break;
	}
	case VMI_LDC:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		if (cmd->constant.i32 > value)
			return FALSE;
		break;
	}
	case VMI_LDC_I8:
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

BOOL arB_instr_test_type_eq(const arB_definition* instr1, const arB_definition* instr2)
{
	if (instr2 == NULL)
		return FALSE;
	return instr1->instr_type == instr2->instr_type;
}

arInt64 arB_instr_ldc_i8_get(const arB_definition* instr)
{
	ASSERT_NOT_NULL(instr);
	switch (instr->instr_type)
	{
	case VMI_LDC_S:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		return (arInt64)cmd->constant.i16;
	}
	case VMI_LDC:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		return (arInt64)cmd->constant.i32;
	}
	case VMI_LDC_I8:
	{
		const vmp_instr_def_ldc* const cmd = (const vmp_instr_def_ldc*)instr;
		return (arInt64)cmd->constant.i64;
	}
	default:
		return -1;
	}
	return TRUE;
}

arB_definition* arB_instr_link(arB_definition* instr, arB_definition* next)
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

arB_definition* arB_instr_get_last(arB_definition* instr)
{
	while (instr->next != NULL) {
		instr = instr->next;
	}
	return instr;
}

const arB_definition* arB_instr_build(const arB_definition* h, struct arBuilder* builder)
{
	const arB_func* const func = h->func;

	switch (h->instr_type)
	{
	case VMI_LDA:
	{
		const vmp_instr_def_lda* const cmd = (vmp_instr_def_lda*)h;
		const arB_arg* const arg = cmd->arg;
		arInstruction_lda instr = { 0 };
		instr.icode = VMI_LDA;
		instr.size = arg->type->size;
		instr.offset = arg->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDA_A:
	{
		const vmp_instr_def_lda_a* const cmd = (vmp_instr_def_lda_a*)h; 
		const arB_arg* const arg = cmd->arg;
		arInstruction_lda_a instr = { 0 };
		instr.icode = VMI_LDA_A;
		instr.size = arg->type->size;
		instr.offset = arg->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDC:
	{
		const vmp_instr_def_ldc* const cmd = (vmp_instr_def_ldc*)h;

		vmi_instr_ldc_i32 instr;
		instr.opcode = 0;
		instr.icode = VMI_LDC;
		instr.props1 = cmd->type->data_type;
		instr.i32 = cmd->constant.i32;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDC_S:
	{
		const vmp_instr_def_ldc_s* const cmd = (vmp_instr_def_ldc_s*)h;

		vmi_instr_ldc_s instr;
		instr.opcode = 0;
		instr.icode = VMI_LDC_S;
		instr.props1 = cmd->type->data_type;
		instr.i16 = cmd->constant.i16;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDC_I8:
	{
		const vmp_instr_def_ldc* const cmd = (vmp_instr_def_ldc*)h;

		vmi_instr_ldc_i64 instr;
		instr.opcode = 0;
		instr.icode = VMI_LDC_I8;
		instr.props1 = cmd->type->data_type;
		instr.i64 = cmd->constant.i64;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			break;
		}
		break;
	}
	case VMI_STL:
	{
		const vmp_instr_def_stl* const cmd = (vmp_instr_def_stl*)h;
		const arB_local* const local = cmd->local;

		arInstruction_stl instr = { 0 };
		instr.icode = VMI_STL;
		instr.size = local->type->size;
		instr.offset = local->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDL:
	{
		const vmp_instr_def_ldl* const cmd = (vmp_instr_def_ldl*)h;
		const arB_local* const local = cmd->local;

		arInstruction_ldl instr = { 0 };
		instr.icode = VMI_LDL;
		instr.size = local->type->size;
		instr.offset = local->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDL_A:
	{
		const vmp_instr_def_ldl_a* const cmd = (vmp_instr_def_ldl_a*)h;
		const arB_local* const local = cmd->local;

		arInstruction_ldl_a instr = { 0 };
		instr.icode = VMI_LDL_A;
		instr.size = local->type->size;
		instr.offset = local->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDF:
	{
		const vmp_instr_def_ldf* const cmd = (vmp_instr_def_ldf*)h;

		vmi_instr_ldf instr;
		instr.opcode = 0;
		instr.icode = VMI_LDF;
		instr.addr = builder->bytestream.memory + cmd->target_func->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LOCALS:
	{
		const vmp_instr_def_locals* const cmd = (vmp_instr_def_locals*)h;
		if (h->func->locals.count > 0) {
			vmi_instr_locals instr;
			instr.opcode = 0;
			instr.icode = VMI_LOCALS;
			instr.size = h->func->locals_stack_size;
			if (!arBuilder_write(builder, &instr, h->instr_size)) {
				return NULL;
			}
		}
		break;
	}
	case VMI_LDG:
	{
		const vmp_instr_def_ldg* const cmd = (vmp_instr_def_ldg*)h;

		arInstruction_ldg instr = { 0 };
		instr.icode = VMI_LDG;
		instr.size = cmd->global->type->size;
		instr.addr = builder->bytestream.memory + cmd->global->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDG_A:
	{
		const vmp_instr_def_ldg_a* const cmd = (vmp_instr_def_ldg_a*)h;

		arInstruction_ldg_a instr = { 0 };
		instr.icode = VMI_LDG_A;
		instr.size = cmd->global->type->size;
		instr.addr = builder->bytestream.memory + cmd->global->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_STG:
	{
		const vmp_instr_def_stg* const cmd = (vmp_instr_def_stg*)h;

		arInstruction_stg instr = { 0 };
		instr.icode = VMI_STG;
		instr.size = cmd->global->type->size;
		instr.addr = builder->bytestream.memory + cmd->global->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_ALLOCS:
	{
		const vmp_instr_def_allocs* const cmd = (vmp_instr_def_allocs*)h;
		if (cmd->type == NULL && cmd->amount <= 0) {
			arB_message_expected_const_larger_than(builder, 0, 0);
			break;
		}

		vmi_instr_allocs instr;
		instr.opcode = 0;
		instr.icode = VMI_ALLOCS;
		if (cmd->type != NULL)
			instr.size = cmd->type->size;
		else
			instr.size = cmd->amount;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_FREES:
	{
		const vmp_instr_def_frees* cmd = (vmp_instr_def_frees*)h;
		arUint32 size = arB_instr_allocs_get_size(cmd);
		if (size <= 0) {
			arB_message_expected_const_larger_than(builder, 0, 0);
			break;
		}
		else if (size > UINT16_MAX) {
			arB_message_expected_const_smaller_than(builder, 0, UINT16_MAX);
			break;
		}

		if (cmd->type == NULL && cmd->amount <= 0) {
			arB_message_expected_const_larger_than(builder, 0, 0);
			break;
		}

		vmi_instr_frees instr;
		instr.opcode = 0;
		instr.icode = VMI_FREES;
		instr.size = (arUint16)size;
		
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_ALLOCH:
	{
		const vmp_instr_def_alloch* const cmd = (vmp_instr_def_alloch*)h;

		// requires:
		// -1 = value of size(int)
		if (!arB_instr_test_prev_count(h, 1)) {
			arB_message_instr_requires_prev_instr(builder, "alloch");
			break;
		}

		vmi_instr_alloch instr;
		instr.opcode = 0;
		instr.icode = VMI_ALLOCH;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_ALLOCH_C:
	{
		const vmp_instr_def_alloch* const cmd = (vmp_instr_def_alloch*)h;
		if (cmd->type == NULL && cmd->amount <= 0) {
			arB_message_expected_const_larger_than(builder, 0, 0);
			break;
		}

		vmi_instr_alloch_c instr;
		instr.opcode = 0;
		instr.icode = VMI_ALLOCH_C;
		if (cmd->type != NULL)
			instr.size = cmd->type->size;
		else
			instr.size = cmd->amount;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_FREEH:
	{
		const vmp_instr_def_freeh* const cmd = (vmp_instr_def_freeh*)h;

		vmi_instr_freeh instr;
		instr.opcode = 0;
		instr.icode = VMI_FREEH;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_STUREF_S:
	{
		const vmp_instr_def_sturef_s* const cmd = (vmp_instr_def_sturef_s*)h;
		if (cmd->type->size > UINT8_MAX) {
			arB_message_type_too_large(builder, &cmd->type->name, cmd->type->size, UINT8_MAX);
			break;
		}

		vmi_instr_sturef_s instr;
		instr.opcode = 0;
		instr.icode = VMI_STUREF_S;
		instr.size = cmd->type->size;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_STELEM:
	{
		const vmp_instr_def_stelem* const cmd = (vmp_instr_def_stelem*)h;
		const arB_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!arB_instr_test_prev_count(h, 2)) {
			arB_message_instr_requires_prev_instr(builder, "stelem");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_ARRAY)) {
			const arInt64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!arB_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				arB_message_expected_const_smaller_than(builder, arB_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			arB_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_stelem instr;
		instr.opcode = 0;
		instr.icode = VMI_STELEM;
		instr.size_per_element = cmd->type->of_type->size;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_STELEM_S:
	{
		const vmp_instr_def_stelem_s* const cmd = (vmp_instr_def_stelem_s*)h;
		const arB_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!arB_instr_test_prev_count(h, 2)) {
			arB_message_instr_requires_prev_instr(builder, "stelem_s");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_ARRAY)) {
			const arInt64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!arB_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				arB_message_expected_const_smaller_than(builder, arB_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			arB_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_stelem_s instr;
		instr.opcode = 0;
		instr.icode = VMI_STELEM_S;
		instr.size = cmd->type->of_type->size;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDELEM:
	{
		const vmp_instr_def_ldelem* const cmd = (vmp_instr_def_ldelem*)h;
		const arB_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!arB_instr_test_prev_count(h, 2)) {
			arB_message_instr_requires_prev_instr(builder, "ldelem");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_ARRAY)) {
			const arInt64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!arB_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				arB_message_expected_const_smaller_than(builder, arB_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			arB_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_ldelem instr;
		instr.opcode = 0;
		instr.icode = VMI_LDELEM;
		instr.size_per_element = cmd->type->of_type->size;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_LDELEM_S:
	{
		const vmp_instr_def_ldelem_s* const cmd = (vmp_instr_def_ldelem_s*)h;
		const arB_type* array_type = cmd->type;

		// requires:
		// -1 = value
		// -2 = index
		if (!arB_instr_test_prev_count(h, 2)) {
			arB_message_instr_requires_prev_instr(builder, "ldelem_s");
			break;
		}

		// If this is an array type then the size is known from the beginning
		if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_ARRAY)) {
			const arInt64 count = array_type->size / array_type->of_type->size;
			// If -2 is a constant value then verify that it doesn't exceed count
			if (!arB_instr_ldc_i8_leq(h->prev->prev, TRUE, count - 1)) {
				arB_message_expected_const_smaller_than(builder, arB_instr_ldc_i8_get(h->prev->prev), count);
				break;
			}
		}
		else if (BIT_ISSET(array_type->flags, arB_TYPE_FLAGS_PTR)) {
			// Allow pointers
		}
		else {
			arB_message_type_not_array(builder, &cmd->type->name);
			break;
		}

		vmi_instr_ldelem_s instr;
		instr.opcode = 0;
		instr.icode = VMI_LDELEM_S;
		instr.size = cmd->type->of_type->size;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_CALL:
	{
		const vmp_instr_def_call* const cmd = (vmp_instr_def_call*)h;
		const arB_func* const func = cmd->target_func;

		vmi_instr_call instr;
		instr.opcode = 0;
		instr.icode = VMI_CALL;
		instr.expected_stack_size = func->args_stack_size;
		instr.addr = builder->bytestream.memory + func->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_CALLNATIVE:
	{
		const vmp_instr_def_callnative* const cmd = (vmp_instr_def_callnative*)h;

		vmi_instr_callnative instr;
		instr.opcode = 0;
		instr.icode = VMI_CALLNATIVE;
		instr.expected_stack_size = cmd->target_func->args_stack_size;
		instr.func_ptr = cmd->target_func->native_func;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_CALLUREF:
	{
		const vmp_instr_def_calluref* const cmd = (vmp_instr_def_calluref*)h;

		vmi_instr_calluref instr;
		instr.opcode = 0;
		instr.icode = VMI_CALLUREF;
		instr.expected_stack_size = cmd->func_def->args_stack_size;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_JMP:
	{
		const vmp_instr_def_jmp* const cmd = (vmp_instr_def_jmp*)h;
		const arB_marker* const marker = cmd->marker;

		vmi_instr_jmp instr;
		instr.opcode = 0;
		instr.icode = VMI_JMP;
		instr.props1 = cmd->op;
		instr.destination = builder->bytestream.memory + marker->instr_offset + marker->func->offset;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_MUL:
	case VMI_SUB:
	case VMI_ADD:
	case VMI_DIV:
	case VMI_NEG:
	case VMI_STUREF:
	{
		const arB_def_with_type* const cmd = (arB_def_with_type*)h;

		vmi_instr_add instr;
		instr.opcode = 0;
		instr.icode = h->instr_type;
		instr.props1 = cmd->type->data_type;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_CMP:
	{
		const vmp_instr_def_cmp* const cmd = (vmp_instr_def_cmp*)h;

		vmi_instr_cmp instr;
		instr.opcode = 0;
		instr.icode = VMI_CMP;
		instr.props1 = cmd->op;
		instr.props2 = VMI_INSTR_CMP_PROP2_SIGNED;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_RET:
	{
		const vmp_instr_def_ret* const cmd = (vmp_instr_def_ret*)h;
		if (func->args_stack_size > UINT16_MAX) {
			arB_message_not_implemented(builder, "arguments stack size larger than UINT16_MAX");
			break;
		}
		if (func->locals_stack_size > UINT16_MAX) {
			arB_message_not_implemented(builder, "local stack size larger than UINT16_MAX");
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
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_EOE:
	{
		vmi_instr_eoe instr;
		instr.opcode = 0;
		instr.icode = VMI_EOE;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_CONV:
	{
		const vmp_instr_def_conv* const cmd = (vmp_instr_def_conv*)h;
		if (arB_type_can_convert(cmd->from_type, cmd->to_type) == FALSE) {
			arB_message_types_not_compatible(builder, &cmd->from_type->name, &cmd->to_type->name);
			break;
		}

		vmi_instr_conv instr;
		instr.opcode = 0;
		instr.icode = VMI_CONV;
		instr.props1 = cmd->from_type->data_type;
		instr.props2 = cmd->to_type->data_type;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	case VMI_BIT:
	{
		const vmp_instr_def_bit* const cmd = (vmp_instr_def_bit*)h;

		vmi_instr_bit instr;
		instr.opcode = 0;
		instr.icode = VMI_BIT;
		instr.props1 = cmd->op;
		instr.props2 = cmd->type->data_type;
		if (!arBuilder_write(builder, &instr, h->instr_size)) {
			return NULL;
		}
		break;
	}
	default:
		arB_message_unknown_instr(builder, h->instr_type);
		return NULL;
	}
	return h->next;
}
