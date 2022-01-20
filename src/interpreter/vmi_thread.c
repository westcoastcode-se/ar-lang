#include "vmi_thread.h"
#include "vmi_ops.h"
#include <string.h>

// Value that forces the virtual machine to perform an eoe, resulting in the application quitting
const vmi_instr_eoe _vmi_force_eoe = {
	VMI_EOE
};

const vm_byte* _vmi_thread_halt(vmi_thread* t, const vm_byte* ip, vm_int32 flags, const char* message)
{
	t->halt_pos = ip;
	t->flags |= flags;
	strcpy(t->exit_reason, message);
	return (const vm_byte*)&_vmi_force_eoe;
}

const vm_byte* _vmi_thread_stack_out_of_memory(vmi_thread* t, const vm_byte* ip)
{
	return _vmi_thread_halt(t, ip, VMI_THREAD_FLAG_STACK_OUT_OF_MEMORY, "out of memory");
}

const vm_byte* _vmi_thread_vars(vmi_thread* t, const vm_byte* ip)
{
	const vmi_instr_vars* instr = (const vmi_instr_vars*)ip;
	t->locals = vmi_stack_push(&t->stack, instr->blocks);
	return ip + sizeof(vmi_instr_vars);
}

inline const vm_byte* _vmi_thread_load(vmi_thread* t, const vm_byte* ip, vm_uint32 block_index)
{
	vmi_stack_block* const block = vmi_stack_push(&t->stack, 1);
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	block->i32 = t->locals[block_index].i32;
	return ip + sizeof(vmi_instr_load);
}

const vm_byte* _vmi_thread_loadx(vmi_thread* t, const vm_byte* ip)
{
	const vmi_instr_loadx* const instr = (const vmi_instr_loadx*)ip;
	vmi_stack_block* const block = vmi_stack_push(&t->stack, 1);
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	block->i32 = t->locals[instr->block_index].i32;
	return ip + sizeof(vmi_instr_loadx);
}

inline const vm_byte* _vmi_thread_store(vmi_thread* t, const vm_byte* ip, vm_uint32 block_index)
{
	const vmi_stack_block* const block = vmi_stack_pop(&t->stack, 1);
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	t->locals[block_index].i32 = block->i32;
	return ip + sizeof(vmi_instr_store);
}

const vm_byte* _vmi_thread_storex(vmi_thread* t, const vm_byte* ip)
{
	const vmi_instr_storex* const instr = (const vmi_instr_storex*)ip;
	const vmi_stack_block* const block = vmi_stack_pop(&t->stack, 1);
	if (block == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	t->locals[instr->block_index].i32 = block->i32;
	return ip + sizeof(vmi_instr_storex);
}

vm_int32 _vmi_thread_exec(vmi_thread* t, const vm_byte* ip)
{
	const vmi_opcode_header* header;
	while (1)
	{
		header = (const vmi_opcode_header*)ip;
		switch (header->opcode)
		{
		case VMI_VARS:
			ip = _vmi_thread_vars(t, ip);
			break;
		case VMI_OP_LOAD0:
			ip = _vmi_thread_load(t, ip, 0);
			break;
		case VMI_OP_LOAD1:
			ip = _vmi_thread_load(t, ip, 1);
			break;
		case VMI_OP_LOAD2:
			ip = _vmi_thread_load(t, ip, 2);
			break;
		case VMI_OP_LOAD3:
			ip = _vmi_thread_load(t, ip, 3);
			break;
		case VMI_OP_LOAD4:
			ip = _vmi_thread_load(t, ip, 4);
			break;
		case VMI_OP_LOAD5:
			ip = _vmi_thread_load(t, ip, 5);
			break;
		case VMI_OP_LOAD6:
			ip = _vmi_thread_load(t, ip, 6);
			break;
		case VMI_OP_LOADX:
			ip = _vmi_thread_loadx(t, ip);
			break;
		case VMI_OP_STORE0:
			ip = _vmi_thread_store(t, ip, 0);
			break;
		case VMI_OP_STORE1:
			ip = _vmi_thread_store(t, ip, 1);
			break;
		case VMI_OP_STORE2:
			ip = _vmi_thread_store(t, ip, 2);
			break;
		case VMI_OP_STORE3:
			ip = _vmi_thread_store(t, ip, 3);
			break;
		case VMI_OP_STORE4:
			ip = _vmi_thread_store(t, ip, 4);
			break;
		case VMI_OP_STORE5:
			ip = _vmi_thread_store(t, ip, 5);
			break;
		case VMI_OP_STORE6:
			ip = _vmi_thread_store(t, ip, 6);
			break;
		case VMI_OP_STOREX:
			ip = _vmi_thread_storex(t, ip);
			break;
		default:
			t->flags |= VMI_THREAD_FLAG_UNKNOWN_INSTRUCTION;
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

vm_int32 vmi_thread_exec(vmi_thread* t, const vm_byte* ip)
{
	t->ip = ip;
	return _vmi_thread_exec(t, ip);
}

void vmi_thread_delete(vmi_thread* t)
{
	// TODO: Add support for multiple threads
	t->process->first_thread = NULL;
	vmi_stack_release(&t->stack);
	free(t);
}
