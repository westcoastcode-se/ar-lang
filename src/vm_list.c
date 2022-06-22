#include "vm_list.h"
#include <stdlib.h>

void vm_list_init(vm_list* list)
{
	vm_list_initz(list, 1024, 1024);
}

void vm_list_initz(vm_list* list, int capacity, int resize_count)
{
	list->capacity = capacity;
	list->count = 0;
	list->memory = (void**)malloc(sizeof(void*));
}

int vm_list_add(vm_list* list, void* ptr)
{
	if (list->count == list->capacity) {
		list->capacity += list->resize_count;
		list->memory = realloc(list->memory, list->capacity);
		if (list->memory == NULL)
			return -1;
	}
	list->memory[list->count++] = ptr;
	return list->count;
}

void* vm_list_get(vm_list* list, int idx)
{
	return list->memory[idx];
}
