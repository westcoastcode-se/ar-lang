#include "arLocals.h"
#include "arMemory.h"

arLocalsBlock* arLocals_new_block(arInt32 size)
{
	arLocalsBlock* new_block = (arLocalsBlock*)arMalloc(sizeof(arLocalsBlock));
	if (new_block == NULL)
		return NULL;
	new_block->memory = new_block->top = arMalloc(size < VM_LOCALS_MIN_SIZE ? VM_LOCALS_MIN_SIZE : size);
	new_block->end = new_block->memory + size;
	new_block->next = new_block->prev = NULL;
	return new_block;
}

BOOL arLocals_init(arLocals* l)
{
	for (int i = 0; i < VM_LOCALS_BLOCK_COUNT; ++i)
	{
		arLocalsBlock* const block = arLocals_new_block(VM_LOCALS_MIN_SIZE);
		if (block == NULL) {
			return FALSE;
		}
		if (l->free_block != NULL) {
			block->prev = l->free_block;
			l->free_block->next = block;
		}
		l->free_block = block;
	}

	// Take a block (it's at the end of the linked list)
	l->current = NULL;
	return TRUE;
}

void arLocals_release(arLocals* l)
{
	arLocalsBlock* block = l->current;
	while (block) {
		arLocalsBlock* const prev = block->prev;
		arFree(block->memory);
		arFree(block);
		block = prev;
	}
	block = l->free_block;
	while (block) {
		arLocalsBlock* const prev = block->prev;
		arFree(block->memory);
		arFree(block);
		block = prev;
	}
}

arLocalsBlock* arLocals_get_block(arLocals* l, arInt32 size)
{
	if (l->free_block == NULL) {
		// No more free blocks are available then allocate it
		return arLocals_new_block(size);
	}
	else {
		// We have unused blocks that we are allowed to resize if needed
		arLocalsBlock* const block = l->free_block;
		if (block->end - block->memory < size) {
			arByte* new_memory = arRealloc(block->memory, size);
			if (new_memory == NULL) {
				return NULL;
			}
			block->top = block->memory = new_memory;
			block->end = block->memory + size;
		}
		return block;
	}
}

arByte* arLocals_take(arLocals* l, arInt32 size)
{
	arLocalsBlock* block = l->current;
	if (block == NULL) {
		block = l->free_block;
		l->free_block = block->prev;
		block->prev->next = NULL;
		block->prev = NULL;
		l->current = block;
	}

	char* top = block->top;
	if (top + size >= block->end) {
		block = arLocals_get_block(l, size);
		if (block == NULL)
			return NULL;
		l->current->next = block;
		block->prev = l->current;
		l->current = block;
		top = block->top;
	}
	block->top += size;
#ifdef VM_LOCALS_DEBUG
	memset(top, 0xFFFFFFFF, (size_t)(block->top - top));
#endif
	return top;
}

BOOL arLocals_give_back(arLocals* l, arInt32 size)
{
	if (size == 0)
		return TRUE;
	arLocalsBlock* const block = l->current;
	block->top -= size;
	
	if (block->top < block->memory) {
		// We removed to much memory from the local block
		return FALSE;
	}
	else if (block->top == block->memory) {
		// We've removed all memory from the block. Let's put it back into the free queue
		l->current = block->prev;
		if (l->current != NULL)
			l->current->next = NULL;
		l->free_block->next = block;
		block->prev = l->free_block;
		l->free_block = block;
	}
	return TRUE;
}
