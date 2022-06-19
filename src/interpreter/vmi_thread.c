#include "vmi_thread.h"
#include "vmi_ops.h"
#include "vmi_debug.h"
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

vmi_ip _vmi_thread_stack_mismanaged_begin(vmi_thread* t, vmi_ip ip, vm_int32 bytes_left)
{
	vmi_thread_shalti(t,
		VMI_THREAD_FLAG_STACK_MISMANAGED,
		"the stack is mismanaged. you are expected to push %d bytes to the stack",
		bytes_left);
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

vmi_ip _vmi_thread_stack_out_of_memory(vmi_thread* t, vmi_ip ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_STACK_OUT_OF_MEMORY, "out of memory");
}

vmi_ip _vmi_thread_not_implemented(vmi_thread* t, vmi_ip ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_NOT_IMPLEMENTED, "not implemented");
}

#include "instr/add.inc.c"
#include "instr/const.inc.c"
#include "instr/conv.inc.c"
#include "instr/cmp.inc.c"
#include "instr/jmp.inc.c"
#include "instr/locals.inc.c"
#include "instr/func.inc.c"
#include "instr/copy_s.inc.c"
#include "instr/stack.inc.c"

vm_int32 _vmi_thread_exec(vmi_thread* t, vmi_ip ip)
{
	const vmi_opcode_header* header;
	while (1)
	{
		if (t->flags != 0)
			return t->flags;
		//vmi_debug_instruction(ip);
		header = (const vmi_opcode_header*)ip;

		// Process specialized instructions first
		switch (header->opcode)
		{
		case VMI_OP_ADD_I16:
			ip = _vmi_thread_addi16(t, ip);
			continue;
		case VMI_OP_ADD_I32:
			ip = _vmi_thread_addi32(t, ip);
			continue;

		case VMI_OP_CONST_INT16:
			ip = _vmi_thread_const_int16(t, ip);
			continue;
		case VMI_OP_CONST_INT32:
			ip = _vmi_thread_const_int32(t, ip);
			continue;

		case VMI_OP_CONV_I16_I32:
			ip = _vmi_thread_conv_i16_i32(t, ip);
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

		default:
			break;
		}

		// Otherwise handle non-specified instructions first.
		switch (header->icode)
		{
		case VMI_CALL:
			ip = _vmi_thread_call(t, ip);
			continue;
		case VMI_BEGIN:
			ip = _vmi_thread_begin(t, ip);
			continue;
		case VMI_LOCALS:
			ip = _vmi_thread_locals(t, ip);
			continue;
		case VMI_ALLOC_S:
			ip = _vmi_thread_alloc_s(t, ip);
			continue;
		case VMI_FREE_S:
			ip = _vmi_thread_free_s(t, ip);
			continue;
		case VMI_LOAD_A:
			ip = _vmi_thread_load_a(t, ip);
			continue;
		case VMI_SAVE_R:
			ip = _vmi_thread_save_r(t, ip);
			continue;
		case VMI_LOAD_L:
			ip = _vmi_thread_load_l(t, ip);
			continue;
		case VMI_SAVE_L:
			ip = _vmi_thread_save_l(t, ip);
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
	t->ebp = NULL;

	// TODO: Add support for multiple threads
	process->first_thread = t;
	return t;
}

vm_int32 vmi_thread_reserve_stack(vmi_thread* t, vm_int32 value)
{
	vm_int32* mem = (vm_int32*)vmi_stack_push(&t->stack, value);
	if (mem == NULL)
		return -1;
	return 0;
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

vm_int32 vmi_thread_exec(vmi_thread* t, vmi_ip ip)
{
	vm_int32 ret;
	t->ip = ip;
	
	// Push the caller to end-of-execution so that the thread stops running when the current function is no longer running.
	// This is always done when a function is being called
	*(vmi_ip*)vmi_stack_push(&t->stack, sizeof(vmi_ip)) = (vmi_ip)&_vmi_force_eoe;

	ret = _vmi_thread_exec(t, ip);
	return ret;
}

void vmi_thread_destroy(vmi_thread* t)
{
	// TODO: Add support for multiple threads
	t->process->first_thread = NULL;
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
