#include "thread.h"
#include "vmi_ops.h"
#include "debug.h"
#include "vmi_utils.h"
#include <string.h>
#include <stdarg.h>

// Value that forces the virtual machine to perform an eoe, resulting in the application quitting
const vmi_instr_eoe _vmi_force_eoe = {
	VMI_EOE
};

vmi_ip _vmi_thread_halt(vmi_thread* t, vmi_ip ip, vm_int32 flags, const char* message)
{
	vmi_thread_halti(t, flags, message);
	t->halt_pos = ip;
	return (vmi_ip)&_vmi_force_eoe;
}

vmi_ip _vmi_thread_stack_mismanaged_begin(vmi_thread* t, vmi_ip ip, vm_int32 bytes, vm_int32 bytes_pushed)
{
	vmi_thread_shalti(t,
		VMI_THREAD_FLAG_STACK_MISMANAGED,
		"the stack is mismanaged. you are expected to push %d bytes to the stack but was %d",
		bytes, bytes_pushed);
	t->halt_pos = ip;
	return (vmi_ip)&_vmi_force_eoe;
}

vmi_ip _vmi_thread_stack_mismanaged_ret(vmi_thread* t, vmi_ip ip, vm_int32 bytes_left)
{
	vmi_thread_shalti(t, 
		VMI_THREAD_FLAG_STACK_MISMANAGED, 
		"the stack is mismanaged. you have %d un-popped bytes left on the stack",
		bytes_left);
	t->halt_pos = ip;
	return (vmi_ip)&_vmi_force_eoe;
}

vmi_ip _vmi_thread_locals_mismanaged_ret(vmi_thread* t, vmi_ip ip)
{
	vmi_thread_shalti(t,
		VMI_THREAD_FLAG_LOCALS_MISMANAGED,
		"the local variables are mismanaged");
	t->halt_pos = ip;
	return (vmi_ip)&_vmi_force_eoe;
}

vmi_ip _vmi_thread_locals_out_of_memory(vmi_thread* t, vmi_ip ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_OUT_OF_MEMORY, "locals out of memory");
}

vmi_ip _vmi_thread_stack_out_of_memory(vmi_thread* t, vmi_ip ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_OUT_OF_MEMORY, "stack out of memory");
}

vmi_ip _vmi_thread_not_implemented(vmi_thread* t, vmi_ip ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_NOT_IMPLEMENTED, "not implemented");
}

#include "instr/add.inc.c"
#include "instr/sub.inc.c"
#include "instr/const.inc.c"
#include "instr/conv.inc.c"
#include "instr/cmp.inc.c"
#include "instr/jmp.inc.c"
#include "instr/locals.inc.c"
#include "instr/globals.inc.c"
#include "instr/func.inc.c"
#include "instr/copy_s.inc.c"
#include "instr/memory.inc.c"
#include "instr/arrays.inc.c"
#include "instr/sturef.inc.c"
#include "instr/neg.inc.c"
#include "instr/mul.inc.c"
#include "instr/div.inc.c"
#include "instr/bit.inc.c"

vm_int32 _vmi_thread_exec(vmi_thread* t, vmi_ip ip)
{
	const vmi_opcode_header* header;
	while (1)
	{
		if (t->flags != 0)
			return t->flags;
		//vmi_debug_instruction(ip);
		//vmi_debug_stack(&t->stack);
		header = (const vmi_opcode_header*)ip;

		// Process specialized instructions first
		switch (header->opcode)
		{
		case VMI_OP_ADD_I1:
			ip = _vmi_thread_addi1(t, ip);
			continue;
		case VMI_OP_ADD_UI1:
			ip = _vmi_thread_addui1(t, ip);
			continue;
		case VMI_OP_ADD_I2:
			ip = _vmi_thread_addi2(t, ip);
			continue;
		case VMI_OP_ADD_UI2:
			ip = _vmi_thread_addui2(t, ip);
			continue;
		case VMI_OP_ADD_I4:
			ip = _vmi_thread_addi4(t, ip);
			continue;
		case VMI_OP_ADD_UI4:
			ip = _vmi_thread_addui4(t, ip);
			continue;
		case VMI_OP_ADD_I8:
			ip = _vmi_thread_addi8(t, ip);
			continue;
		case VMI_OP_ADD_UI8:
			ip = _vmi_thread_addui8(t, ip);
			continue;
		case VMI_OP_ADD_F4:
			ip = _vmi_thread_addf4(t, ip);
			continue;
		case VMI_OP_ADD_F8:
			ip = _vmi_thread_addf8(t, ip);
			continue;

		case VMI_OP_SUB_I1:
			ip = _vmi_thread_subi1(t, ip);
			continue;
		case VMI_OP_SUB_UI1:
			ip = _vmi_thread_subui1(t, ip);
			continue;
		case VMI_OP_SUB_I2:
			ip = _vmi_thread_subi2(t, ip);
			continue;
		case VMI_OP_SUB_UI2:
			ip = _vmi_thread_subui2(t, ip);
			continue;
		case VMI_OP_SUB_I4:
			ip = _vmi_thread_subi4(t, ip);
			continue;
		case VMI_OP_SUB_UI4:
			ip = _vmi_thread_subui4(t, ip);
			continue;
		case VMI_OP_SUB_I8:
			ip = _vmi_thread_subi8(t, ip);
			continue;
		case VMI_OP_SUB_UI8:
			ip = _vmi_thread_subui8(t, ip);
			continue;
		case VMI_OP_SUB_F4:
			ip = _vmi_thread_subf4(t, ip);
			continue;
		case VMI_OP_SUB_F8:
			ip = _vmi_thread_subf8(t, ip);
			continue;

		case VMI_OP_MUL_I1:
			ip = _vmi_thread_muli1(t, ip);
			continue;
		case VMI_OP_MUL_UI1:
			ip = _vmi_thread_mului1(t, ip);
			continue;
		case VMI_OP_MUL_I2:
			ip = _vmi_thread_muli2(t, ip);
			continue;
		case VMI_OP_MUL_UI2:
			ip = _vmi_thread_mului2(t, ip);
			continue;
		case VMI_OP_MUL_I4:
			ip = _vmi_thread_muli4(t, ip);
			continue;
		case VMI_OP_MUL_UI4:
			ip = _vmi_thread_mului4(t, ip);
			continue;
		case VMI_OP_MUL_I8:
			ip = _vmi_thread_muli8(t, ip);
			continue;
		case VMI_OP_MUL_UI8:
			ip = _vmi_thread_mului8(t, ip);
			continue;
		case VMI_OP_MUL_F4:
			ip = _vmi_thread_mulf4(t, ip);
			continue;
		case VMI_OP_MUL_F8:
			ip = _vmi_thread_mulf8(t, ip);
			continue;

		case VMI_OP_DIV_I1:
			ip = _vmi_thread_divi1(t, ip);
			continue;
		case VMI_OP_DIV_UI1:
			ip = _vmi_thread_divui1(t, ip);
			continue;
		case VMI_OP_DIV_I2:
			ip = _vmi_thread_divi2(t, ip);
			continue;
		case VMI_OP_DIV_UI2:
			ip = _vmi_thread_divui2(t, ip);
			continue;
		case VMI_OP_DIV_I4:
			ip = _vmi_thread_divi4(t, ip);
			continue;
		case VMI_OP_DIV_UI4:
			ip = _vmi_thread_divui4(t, ip);
			continue;
		case VMI_OP_DIV_I8:
			ip = _vmi_thread_divi8(t, ip);
			continue;
		case VMI_OP_DIV_UI8:
			ip = _vmi_thread_divui8(t, ip);
			continue;
		case VMI_OP_DIV_F4:
			ip = _vmi_thread_divf4(t, ip);
			continue;
		case VMI_OP_DIV_F8:
			ip = _vmi_thread_divf8(t, ip);
			continue;

		case VMI_OP_NEG_I1:
			ip = _vmi_thread_negi1(t, ip);
			continue;
		case VMI_OP_NEG_UI1:
			ip = _vmi_thread_negui1(t, ip);
			continue;
		case VMI_OP_NEG_I2:
			ip = _vmi_thread_negi2(t, ip);
			continue;
		case VMI_OP_NEG_UI2:
			ip = _vmi_thread_negui2(t, ip);
			continue;
		case VMI_OP_NEG_I4:
			ip = _vmi_thread_negi4(t, ip);
			continue;
		case VMI_OP_NEG_UI4:
			ip = _vmi_thread_negui4(t, ip);
			continue;
		case VMI_OP_NEG_I8:
			ip = _vmi_thread_negi8(t, ip);
			continue;
		case VMI_OP_NEG_UI8:
			ip = _vmi_thread_negui8(t, ip);
			continue;
		case VMI_OP_NEG_F4:
			ip = _vmi_thread_negf4(t, ip);
			continue;
		case VMI_OP_NEG_F8:
			ip = _vmi_thread_negf8(t, ip);
			continue;

		case VMI_OP_BIT_NOT_I1:
			ip = _vmi_thread_bit_noti1(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI1:
			ip = _vmi_thread_bit_notui1(t, ip);
			continue;
		case VMI_OP_BIT_NOT_I2:
			ip = _vmi_thread_bit_noti2(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI2:
			ip = _vmi_thread_bit_notui2(t, ip);
			continue;
		case VMI_OP_BIT_NOT_I4:
			ip = _vmi_thread_bit_noti4(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI4:
			ip = _vmi_thread_bit_notui4(t, ip);
			continue;
		case VMI_OP_BIT_NOT_I8:
			ip = _vmi_thread_bit_noti8(t, ip);
			continue;
		case VMI_OP_BIT_NOT_UI8:
			ip = _vmi_thread_bit_notui8(t, ip);
			continue;

		case VMI_OP_LDC_I1:
			ip = _vmi_thread_ldc_i1(t, ip);
			continue;
		case VMI_OP_LDC_UI1:
			ip = _vmi_thread_ldc_i1(t, ip);
			continue;
		case VMI_OP_LDC_I2:
			ip = _vmi_thread_ldc_i2(t, ip);
			continue;
		case VMI_OP_LDC_UI2:
			ip = _vmi_thread_ldc_i2(t, ip);
			continue;
		case VMI_OP_LDC_I4:
			ip = _vmi_thread_ldc_i4(t, ip);
			continue;
		case VMI_OP_LDC_UI4:
			ip = _vmi_thread_ldc_i4(t, ip);
			continue;
		case VMI_OP_LDC_I8:
			ip = _vmi_thread_ldc_i8(t, ip);
			continue;
		case VMI_OP_LDC_UI8:
			ip = _vmi_thread_ldc_i8(t, ip);
			continue;
		case VMI_OP_LDC_F4:
			ip = _vmi_thread_ldc_f4(t, ip);
			continue;
		case VMI_OP_LDC_F8:
			ip = _vmi_thread_ldc_f8(t, ip);
			continue;

		case VMI_OP_LDC_S_I1_0:
			ip = _vmi_thread_ldc_s_i1(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I1_1:
			ip = _vmi_thread_ldc_s_i1(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I1_N1:
			ip = _vmi_thread_ldc_s_i1(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_I2_0:
			ip = _vmi_thread_ldc_s_i2(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I2_1:
			ip = _vmi_thread_ldc_s_i2(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I2_N1:
			ip = _vmi_thread_ldc_s_i2(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_I4_0:
			ip = _vmi_thread_ldc_s_i4(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I4_1:
			ip = _vmi_thread_ldc_s_i4(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I4_N1:
			ip = _vmi_thread_ldc_s_i4(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_I8_0:
			ip = _vmi_thread_ldc_s_i8(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_I8_1:
			ip = _vmi_thread_ldc_s_i8(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_I8_N1:
			ip = _vmi_thread_ldc_s_i8(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_F4_0:
			ip = _vmi_thread_ldc_s_f4(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_F4_1:
			ip = _vmi_thread_ldc_s_f4(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_F4_N1:
			ip = _vmi_thread_ldc_s_f4(t, ip, -1);
			continue;

		case VMI_OP_LDC_S_F8_0:
			ip = _vmi_thread_ldc_s_f8(t, ip, 0);
			continue;
		case VMI_OP_LDC_S_F8_1:
			ip = _vmi_thread_ldc_s_f8(t, ip, 1);
			continue;
		case VMI_OP_LDC_S_F8_N1:
			ip = _vmi_thread_ldc_s_f8(t, ip, -1);
			continue;

		case VMI_OP_CONV_I2_I4:
			ip = _vmi_thread_conv_i2_i4(t, ip);
			continue;
		case VMI_OP_CONV_I4_I2:
			ip = _vmi_thread_conv_i4_i2(t, ip);
			continue;

		case VMI_OP_CMP_LT:
			ip = _vmi_thread_cmp_lt(t, ip);
			continue;
		case VMI_OP_CMP_GT:
			ip = _vmi_thread_cmp_gt(t, ip);
			continue;

		case VMI_OP_JMPT:
			ip = _vmi_thread_jmp_true(t, ip);
			continue;
		case VMI_OP_JMPF:
			ip = _vmi_thread_jmp_false(t, ip);
			continue;

		case VMI_OP_COPY_S_INT32:
			ip = _vmi_thread_copy_s_int32(t, ip);
			continue;

		case VMI_OP_STUREF_S_I1:
			ip = _vmi_thread_sturef_s_i1(t, ip);
			continue;
		case VMI_OP_STUREF_S_I2:
			ip = _vmi_thread_sturef_s_i2(t, ip);
			continue;
		case VMI_OP_STUREF_S_I4:
			ip = _vmi_thread_sturef_s_i4(t, ip);
			continue;
		case VMI_OP_STUREF_S_I8:
			ip = _vmi_thread_sturef_s_i8(t, ip);
			continue;

		case VMI_OP_STELEM_S_I1:
			ip = _vmi_thread_stelem_s_i1(t, ip);
			continue;
		case VMI_OP_STELEM_S_I2:
			ip = _vmi_thread_stelem_s_i2(t, ip);
			continue;
		case VMI_OP_STELEM_S_I4:
			ip = _vmi_thread_stelem_s_i4(t, ip);
			continue;
		case VMI_OP_STELEM_S_I8:
			ip = _vmi_thread_stelem_s_i8(t, ip);
			continue;

		case VMI_OP_LDELEM_S_I1:
			ip = _vmi_thread_ldelem_s_i1(t, ip);
			continue;
		case VMI_OP_LDELEM_S_I2:
			ip = _vmi_thread_ldelem_s_i2(t, ip);
			continue;
		case VMI_OP_LDELEM_S_I4:
			ip = _vmi_thread_ldelem_s_i4(t, ip);
			continue;
		case VMI_OP_LDELEM_S_I8:
			ip = _vmi_thread_ldelem_s_i8(t, ip);
			continue;

		default:
			break;
		}

		// Otherwise handle non-specified instructions first.
		switch (header->icode)
		{
		case VMI_CALL:
			ip = _vmi_thread_call(t, ip);
			continue;
		case VMI_CALLNATIVE:
			ip = _vmi_thread_callnative(t, ip);
			continue;
		case VMI_CALLUREF:
			ip = _vmi_thread_calluref(t, ip);
			continue;
		case VMI_LOCALS:
			ip = _vmi_thread_locals(t, ip);
			continue;
		case VMI_ALLOCS:
			ip = _vmi_thread_allocs(t, ip);
			continue;
		case VMI_FREES:
			ip = _vmi_thread_frees(t, ip);
			continue;
		case VMI_ALLOCH:
			ip = _vmi_thread_alloch(t, ip);
			continue;
		case VMI_ALLOCH_C:
			ip = _vmi_thread_alloch_c(t, ip);
			continue;
		case VMI_FREEH:
			ip = _vmi_thread_freeh(t, ip);
			continue;
		case VMI_LDA:
			ip = _vmi_thread_lda(t, ip);
			continue;
		case VMI_LDA_A:
			ip = _vmi_thread_lda_a(t, ip);
			continue;
		case VMI_LDL:
			ip = _vmi_thread_ldl(t, ip);
			continue;
		case VMI_LDF:
			ip = _vmi_thread_ldf(t, ip);
			continue;
		case VMI_STL:
			ip = _vmi_thread_stl(t, ip);
			continue;
		case VMI_LDL_A:
			ip = _vmi_thread_ldl_a(t, ip);
			continue;
		case VMI_LDG:
			ip = _vmi_thread_ldg(t, ip);
			continue;
		case VMI_LDG_A:
			ip = _vmi_thread_ldg_a(t, ip);
			continue;
		case VMI_STG:
			ip = _vmi_thread_stg(t, ip);
			continue;
		case VMI_STUREF:
			ip = _vmi_thread_sturef(t, ip);
			continue;
		case VMI_STUREF_S:
			ip = _vmi_thread_sturef_s(t, ip);
			continue;
		case VMI_STELEM:
			ip = _vmi_thread_stelem(t, ip);
			continue;
		case VMI_STELEM_S:
			ip = _vmi_thread_stelem_s(t, ip);
			continue;
		case VMI_LDELEM:
			ip = _vmi_thread_ldelem(t, ip);
			continue;
		case VMI_LDELEM_S:
			ip = _vmi_thread_ldelem_s(t, ip);
			continue;
		case VMI_RET:
			ip = _vmi_thread_ret(t, ip);
			continue;
		case VMI_JMP:
			ip = _vmi_thread_jmp(t, ip);
			continue;
		default:
			vmi_thread_shalti(t, VMI_THREAD_FLAG_UNKNOWN_INSTRUCTION, "unknown instruction %d", header->opcode);
		case VMI_EOE:
			return t->flags;
		}

	}
}

vmi_thread* vmi_thread_new(vmi_process* process)
{
	vmi_thread* t = (vmi_thread*)malloc(sizeof(vmi_thread));
	if (t == NULL)
		return NULL;
	memset(t, 0, sizeof(vmi_thread));
	
	// Internals
	t->process = process;
	t->bytecode = process->bytecode;
	vmi_stack_init(&t->stack);
	vmi_locals_init(&t->locals);
	t->cf_pos = NULL;
	t->cf.ebp = NULL; t->cf.ret = NULL;
	memset(t->call_frames, 0, sizeof(t->call_frames));

	// TODO: Add support for multiple threads
	process->first_thread = t;
	return t;
}

vm_byte* vmi_thread_getarg(vmi_thread* t, vm_int32 offset)
{
	return t->ebp + offset;
}

vm_byte* vmi_thread_push_stack(vmi_thread* t, vm_int32 value)
{
	return vmi_stack_push(&t->stack, value);
}

vm_byte* vmi_thread_pop_stack(vmi_thread* t, vm_int32 value)
{
	return vmi_stack_pop(&t->stack, value);
}

vm_int32 vmi_thread_push_i16(vmi_thread* t, vm_int16 value)
{
	vm_int16* mem = (vm_int16*)vmi_stack_push(&t->stack, sizeof(vm_int16));
	if (mem == NULL)
		return -1;
	*mem = value;
	return 0;
}

vm_int32 vmi_thread_push_i32(vmi_thread* t, vm_int32 value)
{
	vm_int32* mem = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	if (mem == NULL)
		return -1;
	*mem = value;
	return 0;
}

vm_int32 vmi_thread_push_ptr(vmi_thread* t, void* value)
{
	void** mem = (void**)vmi_stack_push(&t->stack, sizeof(void*));
	if (mem == NULL)
		return -1;
	*mem = value;
	return 0;
}

vm_int32 vmi_thread_entrypoint(vmi_thread* t, const vmi_function* func)
{
	t->ip = func->ptr;

#if defined(VM_STACK_DEBUG)
	// Verify that we've pushed the data on the stack for the entry-point function to work
	const vm_byte* expected = t->stack.top - func->expected_stack_size;
	if (t->stack.blocks != expected) {
		const vm_int32 stack_size = (vm_int32)(t->stack.top - t->stack.blocks);
		_vmi_thread_stack_mismanaged_begin(t, NULL, func->expected_stack_size, stack_size);
		return t->flags;
	}
#endif
	// Setup call frame
	t->cf_pos = t->call_frames;
	t->cf_pos->ret = (vmi_ip)&_vmi_force_eoe;
	t->cf_pos->ebp = t->stack.top - func->expected_stack_size;
	t->cf_pos->locals_ptr = NULL;
	t->cf = *t->cf_pos;
	return _vmi_thread_exec(t, t->ip);
}

vm_int32 vmi_thread_exec(vmi_thread* t, const vmi_function* func)
{
	return _vmi_thread_exec(t,
		vmi_thread_begin_call(t, t->ip, func->ptr, func->expected_stack_size)
	);
}

vmi_ip vmi_thread_begin_call(vmi_thread* t, vmi_ip current_ip, vmi_ip next_ip, vm_int32 expected_stack_size)
{
#if defined(VM_STACK_DEBUG)
	// Verify that we've pushed the bare-minimum data on the stack for the function to work
	// 
	// We must've pushed at least "expected_stack_size" in bytes (we also ignore the required function call stack values)
	const vm_byte* expected = t->stack.top - expected_stack_size;
	if (t->ebp > expected) {
		const vm_int32 stack_size = (vm_int32)(t->stack.top - t->ebp);
		return _vmi_thread_stack_mismanaged_begin(t, current_ip, expected_stack_size, stack_size);
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

void vmi_thread_destroy(vmi_thread* t)
{
	// TODO: Add support for multiple threads
	t->process->first_thread = NULL;
	vmi_locals_release(&t->locals);
	vmi_stack_release(&t->stack);
	free(t);
}

void vmi_thread_halt(vmi_thread* t, const char* message)
{
	vmi_thread_halti(t, VMI_THREAD_FLAG_MANUAL_HALT, message);
}

void vmi_thread_halti(vmi_thread* t, vm_int32 flags, const char* message)
{
	t->halt_pos = t->ip;
	t->flags |= flags;
	t->ip = (vmi_ip)&_vmi_force_eoe;
	strcpy(t->exit_reason, message);
}

void vmi_thread_shalti(vmi_thread* t, vm_int32 flags, const char* format, ...)
{
	va_list argptr;

	t->halt_pos = t->ip;
	t->flags |= flags;
	t->ip = (const char*)&_vmi_force_eoe;

	va_start(argptr, format);
	vsprintf(t->exit_reason, format, argptr);
	va_end(argptr);
}