#include "vmi_thread.h"
#include "vmi_ops.h"
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

vmi_ip _vmi_thread_stack_out_of_memory(vmi_thread* t, vmi_ip ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_STACK_OUT_OF_MEMORY, "out of memory");
}

vmi_ip _vmi_thread_not_implemented(vmi_thread* t, vmi_ip ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_NOT_IMPLEMENTED, "not implemented");
}

vmi_ip _vmi_thread_begin(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_begin* instr = (const vmi_instr_begin*)ip;
	t->call_frame = (vmi_thread_call_frame*)vmi_stack_push(&t->stack, sizeof(vmi_thread_call_frame));
	t->call_frame->stack_arguments = t->stack.top - instr->return_total_size - instr->argument_total_size - sizeof(vmi_thread_call_frame) - sizeof(vmi_ip);
	t->call_frame->stack_returns = t->stack.top - instr->return_total_size - sizeof(vmi_thread_call_frame) - sizeof(vmi_ip);
	t->call_frame->locals = (vmi_thread_vars_block*)&t->stack.top;
	return ip + sizeof(vmi_instr_begin);
}

vmi_ip _vmi_thread_alloc_s(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_alloc_s* instr = (const vmi_instr_alloc_s*)ip;
	vmi_stack_push(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_alloc_s);
}

vmi_ip _vmi_thread_free_s(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_free_s* instr = (const vmi_instr_free_s*)ip;
	vmi_stack_pop(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_free_s);
}

vmi_ip _vmi_thread_load_a(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_load_a* instr = (const vmi_instr_load_a*)ip;
	char* const target = vmi_stack_push(&t->stack, instr->size);
	if (target == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);

	// TODO: Add support for different sizes
	switch (instr->size) {
	case 4:
		*(vm_int32*)target = *(vm_int32*)(t->call_frame->stack_arguments + instr->offset);
		break;
	case 1:
	case 2:
	case 8:
	default:
		return _vmi_thread_not_implemented(t, ip);
	}
	return ip + sizeof(vmi_instr_load_a);
}

vmi_ip _vmi_thread_save_r(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_save_r* instr = (const vmi_instr_save_r*)ip;

	// TODO: Add support for different sizes
	switch (instr->size) {
	case 4:
		*(vm_int32*)(t->call_frame->stack_returns + instr->offset) = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
		break;
	case 1:
	case 2:
	case 8:
	default:
		return _vmi_thread_not_implemented(t, ip);
	}
	return ip + sizeof(vmi_instr_save_r);
}

// Return to the caller
vmi_ip _vmi_thread_ret(vmi_thread* t, vmi_ip ip)
{
	t->call_frame = (vmi_thread_call_frame*)vmi_stack_pop(&t->stack, sizeof(vmi_thread_call_frame));
	return *(vmi_ip*)vmi_stack_pop(&t->stack, sizeof(vmi_ip));
}

vmi_ip _vmi_thread_load(vmi_thread* t, vmi_ip ip, const vm_uint32 block_index)
{
	vm_int32* const block = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	*block = t->call_frame->locals[block_index].i32;
	return ip + sizeof(vmi_instr_single_instruction);
}

vmi_ip _vmi_thread_loadx(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_loadx* const instr = (const vmi_instr_loadx*)ip;
	vm_int32* const block = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	*block = t->call_frame->locals[instr->block_index].i32;
	return ip + sizeof(vmi_instr_loadx);
}

vmi_ip _vmi_thread_loadl(vmi_thread* t, vmi_ip ip, const vm_uint32 block_index)
{
	vm_int64* const block = (vm_int64*)vmi_stack_push(&t->stack, sizeof(vm_int64));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	*block = t->call_frame->locals[block_index].i64;
	return ip + sizeof(vmi_instr_single_instruction);
}

vmi_ip _vmi_thread_loadlx(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_loadx* const instr = (const vmi_instr_loadx*)ip;
	vm_int64* const block = (vm_int64*)vmi_stack_push(&t->stack, sizeof(vm_int64));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	*block = t->call_frame->locals[instr->block_index].i64;
	return ip + sizeof(vmi_instr_loadx);
}

vmi_ip _vmi_thread_store(vmi_thread* t, vmi_ip ip, const vm_uint32 block_index)
{
	const vm_int32* const block = (vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	t->call_frame->locals[block_index].i32 = *block;
	return ip + sizeof(vmi_instr_single_instruction);
}

vmi_ip _vmi_thread_storex(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_storex* const instr = (const vmi_instr_storex*)ip;
	const vm_int32* const block = (vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	t->call_frame->locals[instr->block_index].i32 = *block;
	return ip + sizeof(vmi_instr_storex);
}

vmi_ip _vmi_thread_storel(vmi_thread* t, vmi_ip ip, const vm_uint32 block_index)
{
	const vm_int64* const block = (const vm_int64*)vmi_stack_pop(&t->stack, sizeof(vm_int64));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	t->call_frame->locals[block_index].i64 = *block;
	return ip + sizeof(vmi_instr_single_instruction);
}

vmi_ip _vmi_thread_storelx(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_storex* const instr = (const vmi_instr_storex*)ip;
	const vm_int64* const block = (const vm_int64*)vmi_stack_pop(&t->stack, sizeof(vm_int64));
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	t->call_frame->locals[instr->block_index].i64 = *block;
	return ip + sizeof(vmi_instr_storex);
}

// Add the two top-most values from the stack. Assume that they are 4 byte integers
vmi_ip _vmi_thread_addi32(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 rhs = *(const vm_int32*)vmi_stack_pop(&t->stack, 4);
	const vm_int32 lhs = *(const vm_int32*)vmi_stack_pop(&t->stack, 4);
	vm_int32* result = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*result = (lhs + rhs);
	return ip + sizeof(vmi_instr_single_instruction);
}

vmi_ip _vmi_thread_const_int32(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_const_int32* instr = (const vmi_instr_const_int32*)ip;
	vm_int32* result = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*result = instr->value;
	return ip + sizeof(vmi_instr_const_int32);
}

vm_int32 _vmi_thread_exec(vmi_thread* t, vmi_ip ip)
{
	const vmi_opcode_header* header;
	while (1)
	{
		if (t->flags != 0)
			return t->flags;
		header = (const vmi_opcode_header*)ip;

		// Process specialized instructions first
		switch (header->opcode)
		{
		case VMI_OP_LOAD0:
			ip = _vmi_thread_load(t, ip, 0);
			continue;
		case VMI_OP_LOAD1:
			ip = _vmi_thread_load(t, ip, 1);
			continue;
		case VMI_OP_LOAD2:
			ip = _vmi_thread_load(t, ip, 2);
			continue;
		case VMI_OP_LOAD3:
			ip = _vmi_thread_load(t, ip, 3);
			continue;
		case VMI_OP_LOAD4:
			ip = _vmi_thread_load(t, ip, 4);
			continue;
		case VMI_OP_LOAD5:
			ip = _vmi_thread_load(t, ip, 5);
			continue;
		case VMI_OP_LOAD6:
			ip = _vmi_thread_load(t, ip, 6);
			continue;
		case VMI_OP_LOADX:
			ip = _vmi_thread_loadx(t, ip);
			continue;

		case VMI_OP_LOADL0:
			ip = _vmi_thread_loadl(t, ip, 0);
			continue;
		case VMI_OP_LOADL1:
			ip = _vmi_thread_loadl(t, ip, 1);
			continue;
		case VMI_OP_LOADL2:
			ip = _vmi_thread_loadl(t, ip, 2);
			continue;
		case VMI_OP_LOADL3:
			ip = _vmi_thread_loadl(t, ip, 3);
			continue;
		case VMI_OP_LOADL4:
			ip = _vmi_thread_loadl(t, ip, 4);
			continue;
		case VMI_OP_LOADL5:
			ip = _vmi_thread_loadl(t, ip, 5);
			continue;
		case VMI_OP_LOADL6:
			ip = _vmi_thread_loadl(t, ip, 6);
			continue;
		case VMI_OP_LOADLX:
			ip = _vmi_thread_loadlx(t, ip);
			continue;

		case VMI_OP_STORE0:
			ip = _vmi_thread_store(t, ip, 0);
			continue;
		case VMI_OP_STORE1:
			ip = _vmi_thread_store(t, ip, 1);
			continue;
		case VMI_OP_STORE2:
			ip = _vmi_thread_store(t, ip, 2);
			continue;
		case VMI_OP_STORE3:
			ip = _vmi_thread_store(t, ip, 3);
			continue;
		case VMI_OP_STORE4:
			ip = _vmi_thread_store(t, ip, 4);
			continue;
		case VMI_OP_STORE5:
			ip = _vmi_thread_store(t, ip, 5);
			continue;
		case VMI_OP_STORE6:
			ip = _vmi_thread_store(t, ip, 6);
			continue;
		case VMI_OP_STOREX:
			ip = _vmi_thread_storex(t, ip);
			continue;

		case VMI_OP_STOREL0:
			ip = _vmi_thread_storel(t, ip, 0);
			continue;
		case VMI_OP_STOREL1:
			ip = _vmi_thread_storel(t, ip, 1);
			continue;
		case VMI_OP_STOREL2:
			ip = _vmi_thread_storel(t, ip, 2);
			continue;
		case VMI_OP_STOREL3:
			ip = _vmi_thread_storel(t, ip, 3);
			continue;
		case VMI_OP_STOREL4:
			ip = _vmi_thread_storel(t, ip, 4);
			continue;
		case VMI_OP_STOREL5:
			ip = _vmi_thread_storel(t, ip, 5);
			continue;
		case VMI_OP_STOREL6:
			ip = _vmi_thread_storel(t, ip, 6);
			continue;
		case VMI_OP_STORELX:
			ip = _vmi_thread_storelx(t, ip);
			continue;

		case VMI_OP_ADD_I32:
			ip = _vmi_thread_addi32(t, ip);
			continue;

		case VMI_OP_CONST_INT32:
			ip = _vmi_thread_const_int32(t, ip);
			continue;

		default:
			break;
		}

		// Otherwise handle non-specified instructions first.
		switch (header->icode)
		{
		case VMI_BEGIN:
			ip = _vmi_thread_begin(t, ip);
			continue;
		//case VMI_VARS:
		//	ip = _vmi_thread_vars(t, ip);
		//	continue;
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
		case VMI_RET:
			ip = _vmi_thread_ret(t, ip);
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

	// TODO: Add support for multiple threads
	process->first_thread = t;
	return t;
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
	*(vmi_ip*)vmi_stack_push(&t->stack, sizeof(vmi_ip)) = &_vmi_force_eoe;
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
