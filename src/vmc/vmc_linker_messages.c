#include "vmc_linker_messages.h"

BOOL vmc_linker_message_unresolved_addr(vm_messages* m, const vm_string* signature)
{
	return vm_messages_add(m,
		VMC_LINKER_MESSAGE_PREFIX,
		VMC_LINKER_MSG_UNRESOLVED_ADDR,
		VMC_LINKER_MSG_UNRESOLVED_ADDR_STR,
		vm_string_length(signature), signature->start);
}
