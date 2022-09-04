#include "arThread.h"
#include "arInstructions.h"
#include "arUtils.h"
#include <string.h>
#include <stdarg.h>

// Value that forces the virtual machine to perform an eoe, resulting in the application quitting
const vmi_instr_eoe _vmi_force_eoe = {
	VMI_EOE
};

arIP _arThread_halt(arThread* t, arIP ip, arInt32 flags, const char* message)
{
	arThread_halt(t, flags, message);
	t->halt_pos = ip;
	return (arIP)&_vmi_force_eoe;
}

arIP _arThread_stack_mismanaged_begin(arThread* t, arIP ip, arInt32 bytes, arInt32 bytes_pushed)
{
	arThread_haltf(t,
		arThread_FLAG_STACK_MISMANAGED,
		"the stack is mismanaged. you are expected to push %d bytes to the stack but was %d",
		bytes, bytes_pushed);
	t->halt_pos = ip;
	return (arIP)&_vmi_force_eoe;
}

arIP _arThread_stack_mismanaged_ret(arThread* t, arIP ip, arInt32 bytes_left)
{
	arThread_haltf(t, 
		arThread_FLAG_STACK_MISMANAGED, 
		"the stack is mismanaged. you have %d un-popped bytes left on the stack",
		bytes_left);
	t->halt_pos = ip;
	return (arIP)&_vmi_force_eoe;
}

arIP _arThread_locals_mismanaged_ret(arThread* t, arIP ip)
{
	arThread_haltf(t,
		arThread_FLAG_LOCALS_MISMANAGED,
		"the local variables are mismanaged");
	t->halt_pos = ip;
	return (arIP)&_vmi_force_eoe;
}

arIP _arThread_locals_out_of_memory(arThread* t, arIP ip)
{
	return _arThread_halt(t, ip, arThread_FLAG_OUT_OF_MEMORY, "locals out of memory");
}

arIP _arThread_stack_out_of_memory(arThread* t, arIP ip)
{
	return _arThread_halt(t, ip, arThread_FLAG_OUT_OF_MEMORY, "stack out of memory");
}

arIP _arThread_not_implemented(arThread* t, arIP ip)
{
	return _arThread_halt(t, ip, arThread_FLAG_NOT_IMPLEMENTED, "not implemented");
}

#include "interpreter/add.inc.c"
#include "interpreter/sub.inc.c"
#include "interpreter/const.inc.c"
#include "interpreter/conv.inc.c"
#include "interpreter/cmp.inc.c"
#include "interpreter/jmp.inc.c"
#include "interpreter/locals.inc.c"
#include "interpreter/globals.inc.c"
#include "interpreter/func.inc.c"
#include "interpreter/copy_s.inc.c"
#include "interpreter/memory.inc.c"
#include "interpreter/arrays.inc.c"
#include "interpreter/sturef.inc.c"
#include "interpreter/neg.inc.c"
#include "interpreter/mul.inc.c"
#include "interpreter/div.inc.c"
#include "interpreter/bit.inc.c"

arInt32 _arThread_exec(arThread* t, arIP ip)
{
	const arOperationHeader* header;
	while (1)
	{
		if (t->flags != 0)
			return t->flags;
#ifdef ARLANG_INSTRUCTION_DEBUG
#	ifdef VM_STACK_DEBUG
		printf("\n%p] [stack=%05d] ", ip, arStack_count(&t->stack));
#	else
		printf("\n%p] ", ip);
#	endif
#endif
		header = (const arOperationHeader*)ip;

		// Process specialized instructions first
		switch (header->opcode)
		{
		case VMI_OP_ADD_I1:
			ip = _arThread_addi1(t, ip);
			continue;
		case VMI_OP_ADD_UI1:
			ip = _arThread_addui1(t, ip);
			continue;
		case VMI_OP_ADD_I2:
			ip = _arThread_addi2(t, ip);
			continue;
		case VMI_OP_ADD_UI2:
			ip = _arThread_addui2(t, ip);
			continue;
		case VMI_OP_ADD_I4:
			ip = _arThread_addi4(t, ip);
			continue;
		case VMI_OP_ADD_UI4:
			ip = _arThread_addui4(t, ip);
			continue;
		case VMI_OP_ADD_I8:
			ip = _arThread_addi8(t, ip);
			continue;
		case VMI_OP_ADD_UI8:
			ip = _arThread_addui8(t, ip);
			continue;
		case VMI_OP_ADD_F4:
			ip = _arThread_addf4(t, ip);
			continue;
		case VMI_OP_ADD_F8:
			ip = _arThread_addf8(t, ip);
			continue;

		case VMI_OP_SUB_I1:
			ip = _arThread_subi1(t, ip);
			continue;
		case VMI_OP_SUB_UI1:
			ip = _arThread_subui1(t, ip);
			continue;
		case VMI_OP_SUB_I2:
			ip = _arThread_subi2(t, ip);
			continue;
		case VMI_OP_SUB_UI2:
			ip = _arThread_subui2(t, ip);
			continue;
		case VMI_OP_SUB_I4:
			ip = _arThread_subi4(t, ip);
			continue;
		case VMI_OP_SUB_UI4:
			ip = _arThread_subui4(t, ip);
			continue;
		case VMI_OP_SUB_I8:
			ip = _arThread_subi8(t, ip);
			continue;
		case VMI_OP_SUB_UI8:
			ip = _arThread_subui8(t, ip);
			continue;
		case VMI_OP_SUB_F4:
			ip = _arThread_subf4(t, ip);
			continue;
		case VMI_OP_SUB_F8:
			ip = _arThread_subf8(t, ip);
			continue;

		case VMI_OP_MUL_I1:
			ip = _arThread_muli1(t, ip);
			continue;
		case VMI_OP_MUL_UI1:
			ip = _arThread_mului1(t, ip);
			continue;
		case VMI_OP_MUL_I2:
			ip = _arThread_muli2(t, ip);
			continue;
		case VMI_OP_MUL_UI2:
			ip = _arThread_mului2(t, ip);
			continue;
		case VMI_OP_MUL_I4:
			ip = _arThread_muli4(t, ip);
			continue;
		case VMI_OP_MUL_UI4:
			ip = _arThread_mului4(t, ip);
			continue;
		case VMI_OP_MUL_I8:
			ip = _arThread_muli8(t, ip);
			continue;
		case VMI_OP_MUL_UI8:
			ip = _arThread_mului8(t, ip);
			continue;
		case VMI_OP_MUL_F4:
			ip = _arThread_mulf4(t, ip);
			continue;
		case VMI_OP_MUL_F8:
			ip = _arThread_mulf8(t, ip);
			continue;

		case VMI_OP_DIV_I1:
			ip = _arThread_divi1(t, ip);
			continue;
		case VMI_OP_DIV_UI1:
			ip = _arThread_divui1(t, ip);
			continue;
		case VMI_OP_DIV_I2:
			ip = _arThread_divi2(t, ip);
			continue;
		case VMI_OP_DIV_UI2:
			ip = _arThread_divui2(t, ip);
			continue;
		case VMI_OP_DIV_I4:
			ip = _arThread_divi4(t, ip);
			continue;
		case VMI_OP_DIV_UI4:
			ip = _arThread_divui4(t, ip);
			continue;
		case VMI_OP_DIV_I8:
			ip = _arThread_divi8(t, ip);
			continue;
		case VMI_OP_DIV_UI8:
			ip = _arThread_divui8(t, ip);
			continue;
		case VMI_OP_DIV_F4:
			ip = _arThread_divf4(t, ip);
			continue;
		case VMI_OP_DIV_F8:
			ip = _arThread_divf8(t, ip);
			continue;

		case VMI_OP_NEG_I1:
			ip = _arThread_negi1(t, ip);
			continue;
		case VMI_OP_NEG_UI1:
			ip = _arThread_negui1(t, ip);
			continue;
		case VMI_OP_NEG_I2:
			ip = _arThread_negi2(t, ip);
			continue;
		case VMI_OP_NEG_UI2:
			ip = _arThread_negui2(t, ip);
			continue;
		case VMI_OP_NEG_I4:
			ip = _arThread_negi4(t, ip);
			continue;
		case VMI_OP_NEG_UI4:
			ip = _arThread_negui4(t, ip);
			continue;
		case VMI_OP_NEG_I8:
			ip = _arThread_negi8(t, ip);
			continue;
		case VMI_OP_NEG_UI8:
			ip = _arThread_negui8(t, ip);
			continue;
		case VMI_OP_NEG_F4:
			ip = _arThread_negf4(t, ip);
			continue;
		case VMI_OP_NEG_F8:
			ip = _arThread_negf8(t, ip);
			continue;

		case VMI_OP_BIT_NOT_I1:
			ip = _arThread_bit_noti1(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI1:
			ip = _arThread_bit_notui1(t, ip);
			continue;
		case VMI_OP_BIT_NOT_I2:
			ip = _arThread_bit_noti2(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI2:
			ip = _arThread_bit_notui2(t, ip);
			continue;
		case VMI_OP_BIT_NOT_I4:
			ip = _arThread_bit_noti4(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI4:
			ip = _arThread_bit_notui4(t, ip);
			continue;
		case VMI_OP_BIT_NOT_I8:
			ip = _arThread_bit_noti8(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI8:
			ip = _arThread_bit_notui8(t, ip);
			continue;

		case VMI_OP_LDC_I1:
			ip = _arThread_ldc_i1(t, ip);
			continue;
		case VMI_OP_LDC_UI1:
			ip = _arThread_ldc_i1(t, ip);
			continue;
		case VMI_OP_LDC_I2:
			ip = _arThread_ldc_i2(t, ip);
			continue;
		case VMI_OP_LDC_UI2:
			ip = _arThread_ldc_i2(t, ip);
			continue;
		case VMI_OP_LDC_I4:
			ip = _arThread_ldc_i4(t, ip);
			continue;
		case VMI_OP_LDC_UI4:
			ip = _arThread_ldc_i4(t, ip);
			continue;
		case VMI_OP_LDC_I8:
			ip = _arThread_ldc_i8(t, ip);
			continue;
		case VMI_OP_LDC_UI8:
			ip = _arThread_ldc_i8(t, ip);
			continue;
		case VMI_OP_LDC_F4:
			ip = _arThread_ldc_f4(t, ip);
			continue;
		case VMI_OP_LDC_F8:
			ip = _arThread_ldc_f8(t, ip);
			continue;

		case VMI_OP_LDC_S_I1_0:
			ip = _arThread_ldc_s_i1(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I1_1:
			ip = _arThread_ldc_s_i1(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I1_N1:
			ip = _arThread_ldc_s_i1(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_I2_0:
			ip = _arThread_ldc_s_i2(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I2_1:
			ip = _arThread_ldc_s_i2(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I2_N1:
			ip = _arThread_ldc_s_i2(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_I4_0:
			ip = _arThread_ldc_s_i4(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I4_1:
			ip = _arThread_ldc_s_i4(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I4_N1:
			ip = _arThread_ldc_s_i4(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_I8_0:
			ip = _arThread_ldc_s_i8(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I8_1:
			ip = _arThread_ldc_s_i8(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I8_N1:
			ip = _arThread_ldc_s_i8(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_F4_0:
			ip = _arThread_ldc_s_f4(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_F4_1:
			ip = _arThread_ldc_s_f4(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_F4_N1:
			ip = _arThread_ldc_s_f4(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_F8_0:
			ip = _arThread_ldc_s_f8(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_F8_1:
			ip = _arThread_ldc_s_f8(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_F8_N1:
			ip = _arThread_ldc_s_f8(t, ip, -1);
			continue;

		case VMI_OP_CONV_I2_I4:
			ip = _arThread_conv_i2_i4(t, ip);
			continue;
		case VMI_OP_CONV_I4_I2:
			ip = _arThread_conv_i4_i2(t, ip);
			continue;

		case VMI_OP_CMP_LT:
			ip = _arThread_cmp_lt(t, ip);
			continue;
		case VMI_OP_CMP_GT:
			ip = _arThread_cmp_gt(t, ip);
			continue;

		case VMI_OP_JMPT:
			ip = _arThread_jmp_true(t, ip);
			continue;
		case VMI_OP_JMPF:
			ip = _arThread_jmp_false(t, ip);
			continue;

		case VMI_OP_COPY_S_INT32:
			ip = _arThread_copy_s_int32(t, ip);
			continue;

		case VMI_OP_STUREF_S_I1:
			ip = _arThread_sturef_s_i1(t, ip);
			continue;
		case VMI_OP_STUREF_S_I2:
			ip = _arThread_sturef_s_i2(t, ip);
			continue;
		case VMI_OP_STUREF_S_I4:
			ip = _arThread_sturef_s_i4(t, ip);
			continue;
		case VMI_OP_STUREF_S_I8:
			ip = _arThread_sturef_s_i8(t, ip);
			continue;

		case VMI_OP_STELEM_S_I1:
			ip = _arThread_stelem_s_i1(t, ip);
			continue;
		case VMI_OP_STELEM_S_I2:
			ip = _arThread_stelem_s_i2(t, ip);
			continue;
		case VMI_OP_STELEM_S_I4:
			ip = _arThread_stelem_s_i4(t, ip);
			continue;
		case VMI_OP_STELEM_S_I8:
			ip = _arThread_stelem_s_i8(t, ip);
			continue;

		case VMI_OP_LDELEM_S_I1:
			ip = _arThread_ldelem_s_i1(t, ip);
			continue;
		case VMI_OP_LDELEM_S_I2:
			ip = _arThread_ldelem_s_i2(t, ip);
			continue;
		case VMI_OP_LDELEM_S_I4:
			ip = _arThread_ldelem_s_i4(t, ip);
			continue;
		case VMI_OP_LDELEM_S_I8:
			ip = _arThread_ldelem_s_i8(t, ip);
			continue;

		default:
			break;
		}

		// Otherwise handle non-specified instructions first.
		switch (header->icode)
		{
		case VMI_CALL:
			ip = _arThread_call(t, ip);
			continue;
		case VMI_CALLNATIVE:
			ip = _arThread_callnative(t, ip);
			continue;
		case VMI_CALLUREF:
			ip = _arThread_calluref(t, ip);
			continue;
		case VMI_LOCALS:
			ip = _arThread_locals(t, ip);
			continue;
		case VMI_ALLOCS:
			ip = _arThread_allocs(t, ip);
			continue;
		case VMI_FREES:
			ip = _arThread_frees(t, ip);
			continue;
		case VMI_ALLOCH:
			ip = _arThread_alloch(t, ip);
			continue;
		case VMI_ALLOCH_C:
			ip = _arThread_alloch_c(t, ip);
			continue;
		case VMI_FREEH:
			ip = _arThread_freeh(t, ip);
			continue;
		case VMI_LDA:
			ip = _arThread_lda(t, ip);
			continue;
		case VMI_LDA_A:
			ip = _arThread_lda_a(t, ip);
			continue;
		case VMI_LDL:
			ip = _arThread_ldl(t, ip);
			continue;
		case VMI_LDF:
			ip = _arThread_ldf(t, ip);
			continue;
		case VMI_STL:
			ip = _arThread_stl(t, ip);
			continue;
		case VMI_LDL_A:
			ip = _arThread_ldl_a(t, ip);
			continue;
		case VMI_LDG:
			ip = _arThread_ldg(t, ip);
			continue;
		case VMI_LDG_A:
			ip = _arThread_ldg_a(t, ip);
			continue;
		case VMI_STG:
			ip = _arThread_stg(t, ip);
			continue;
		case VMI_STUREF:
			ip = _arThread_sturef(t, ip);
			continue;
		case VMI_STUREF_S:
			ip = _arThread_sturef_s(t, ip);
			continue;
		case VMI_STELEM:
			ip = _arThread_stelem(t, ip);
			continue;
		case VMI_STELEM_S:
			ip = _arThread_stelem_s(t, ip);
			continue;
		case VMI_LDELEM:
			ip = _arThread_ldelem(t, ip);
			continue;
		case VMI_LDELEM_S:
			ip = _arThread_ldelem_s(t, ip);
			continue;
		case VMI_RET:
			ip = _arThread_ret(t, ip);
			continue;
		case VMI_JMP:
			ip = _arThread_jmp(t, ip);
			continue;
		default:
#ifdef ARLANG_INSTRUCTION_DEBUG
			printf("UNKNOWN opcode=%d\n", header->opcode);
#endif
			arThread_haltf(t, arThread_FLAG_UNKNOWN_INSTRUCTION, "unknown instruction %d", header->opcode);
		case VMI_EOE:
#ifdef ARLANG_INSTRUCTION_DEBUG
			printf("EOE\n");
#endif
			return t->flags;
		}
	}
}

arThread* arThread_new(arProcess* process)
{
	arThread* const p = arSafeMalloc(arThread);
	memset(p, 0, sizeof(arThread));
	
	// Internals
	p->process = process;
	p->bytecode = process->bytecode;
	arStack_init(&p->stack);
	arLocals_init(&p->locals);
	p->cf_pos = NULL;
	p->cf.ebp = NULL; p->cf.ret = NULL;
	memset(p->call_frames, 0, sizeof(p->call_frames));

	// TODO: Add support for multiple threads
	process->first_thread = p;
	return p;
}

arByte* arThread_getArg(arThread* t, arInt32 offset)
{
	return t->ebp + offset;
}

arByte* arThread_pushStack(arThread* t, arInt32 size)
{
	return arStack_push(&t->stack, size);
}

arByte* arThread_popStack(arThread* t, arInt32 size)
{
	return arStack_pop(&t->stack, size);
}

arInt32 arThread_entrypoint(arThread* t, const arFunction* func)
{
	t->ip = func->ptr;

#if defined(VM_STACK_DEBUG)
	// Verify that we've pushed the data on the stack for the entry-point function to work
	const arByte* expected = t->stack.top - func->expected_stack_size;
	if (t->stack.blocks != expected) {
		const arInt32 stack_size = (arInt32)(t->stack.top - t->stack.blocks);
		_arThread_stack_mismanaged_begin(t, NULL, func->expected_stack_size, stack_size);
		return t->flags;
	}
#endif
	// Setup call frame
	t->cf_pos = t->call_frames;
	t->cf_pos->ret = (arIP)&_vmi_force_eoe;
	t->cf_pos->ebp = t->stack.top - func->expected_stack_size;
	t->cf_pos->locals_ptr = NULL;
	t->cf = *t->cf_pos;
	return _arThread_exec(t, t->ip);
}

arInt32 arThread_exec(arThread* t, const arFunction* func)
{
	return _arThread_exec(t,
		arThread_beginCall(t, t->ip, func->ptr, func->expected_stack_size)
	);
}

arIP arThread_beginCall(arThread* t, arIP current_ip, arIP next_ip, arInt32 expected_stack_size)
{
#if defined(VM_STACK_DEBUG)
	// Verify that we've pushed the bare-minimum data on the stack for the function to work
	// 
	// We must've pushed at least "expected_stack_size" in bytes (we also ignore the required function call stack values)
	const arByte* expected = t->stack.top - expected_stack_size;
	if (t->ebp > expected) {
		const arInt32 stack_size = (arInt32)(t->stack.top - t->ebp);
		return _arThread_stack_mismanaged_begin(t, current_ip, expected_stack_size, stack_size);
	}
#endif
	// Prepare the next call frame
	t->cf_pos++;

	// New ebp is where the arguments can be found.
	t->cf_pos->ebp = t->stack.top - expected_stack_size;

	// The address where the application should continue executing when the function returns
	t->cf_pos->ret = (current_ip + sizeof(vmi_instr_call));
	t->cf = *t->cf_pos;
	return next_ip;
}

void arThread_destroy(arThread* t)
{
	// TODO: Add support for multiple threads
	t->process->first_thread = NULL;
	arLocals_release(&t->locals);
	arStack_release(&t->stack);
	arFree(t);
}

void arThread_halt(arThread* t, arInt32 flags, const char* message)
{
	t->halt_pos = t->ip;
	t->flags |= flags;
	t->ip = (arIP)&_vmi_force_eoe;
	strcpy(t->exit_reason, message);
}

void arThread_haltf(arThread* t, arInt32 flags, const char* format, ...)
{
	va_list argptr;

	t->halt_pos = t->ip;
	t->flags |= flags;
	t->ip = (const char*)&_vmi_force_eoe;

	va_start(argptr, format);
	vsprintf(t->exit_reason, format, argptr);
	va_end(argptr);
}
