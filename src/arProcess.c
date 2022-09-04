#include "arProcess.h"
#include "arThread.h"
#include "arBytecode.h"

//
// PRIVATE ///////////////////////////////////////////////////////////////////////////
// 

BOOL _arProcess_load_package_info(arProcess* p, const ar_process_header* header, const arByte* bytecode)
{
	arUint32 i, j;
	const arByte* ptr = (p->bytecode + header->first_package_offset);
	
	// Prepare each package
	for (i = 0; i < header->packages_count; ++i) {
		const ar_package_header* header = (const ar_package_header*)(ptr);
		const char* name_start = ptr + sizeof(ar_package_header);
		arPackage* const current_package = arPackage_new();

		current_package->id = vmi_list_packages_add(&p->packages, current_package);
		current_package->name.start = name_start;
		current_package->name.end = name_start + header->name_length;
		current_package->process = p;

		ptr += sizeof(ar_package_header) + header->name_length;

		for(j = 0; j < header->functions_count; ++j) {
			const ar_function_header* fh = (const ar_function_header*)(ptr);
			name_start = ptr + sizeof(ar_package_header);
			arFunction* const current_func = arFunction_new();
			current_func->id = p->functions_count++;
			current_func->name.start = name_start;
			current_func->name.end = name_start + fh->name_length;
			current_func->ptr = bytecode + fh->ptr_start;
			current_func->expected_stack_size = fh->expected_stack_size;

			vmi_list_functions_add(&current_package->functions, current_func);

			ptr += sizeof(ar_function_header) + fh->name_length;
		}
	}

	// Set function in the global functions list
	p->functions = malloc(sizeof(arFunction*) * p->functions_count);
	for (i = 0; i < p->packages.count; ++i) {
		const arPackage* package = vmi_list_packages_get(&p->packages, i);
		for (j = 0; j < package->functions.count; ++j) {
			const arFunction* func = vmi_list_functions_get(&package->functions, j);
			p->functions[func->id] = func;
		}
	}

	return TRUE;
}

//
// PUBLIC ///////////////////////////////////////////////////////////////////////////
// 

arPackage* arPackage_new()
{
	arPackage* const p = arSafeMalloc(arPackage);
	vmi_list_functions_init(&p->functions);
	return p;
}

void arPackage_destroy(arPackage* p)
{
	vmi_list_functions_release(&p->functions);
	arFree(p);
}

arFunction* arFunction_new()
{
	arFunction* const p = arSafeMalloc(arFunction);
	return p;
}

void arFunction_destroy(arFunction* f)
{
	arFree(f);
}

arProcess* arProcess_new()
{
	arProcess* const p = arSafeMalloc(arProcess);
	memset(p, 0, sizeof(arProcess));
	vmi_list_packages_init(&p->packages);
	p->functions = NULL;
	p->functions_count = 0;
	return p;
}

void arProcess_destroy(arProcess* p)
{
	ASSERT_NOT_NULL(p);
	vmi_list_packages_release(&p->packages);
	arFree(p);
}

arInt32 arProcess_load(arProcess* p, const arByte* bytecode)
{
	// Validate the bytecode
	const ar_process_header* header = (const ar_process_header*)bytecode;
	if (header->header[0] != 'V' || header->header[1] != 'M' || header->header[2] != '0')
		return -1;
	if (header->version != VM_VERSION)
		return -1;

	// Set values
	p->bytecode = bytecode;

	// Load package information
	if (!_arProcess_load_package_info(p, header, bytecode)) {
		return -1;
	}
	return 0;
}

arInt32 arProcess_exec(arProcess* p, struct arThread* t, const arFunction* func)
{
	arInt32 result;
	// TODO: Add support for multiple threads
	// TODO: Add support for routines/light-weight threads
	p->current_thread = t;
	result = arThread_entrypoint(t, func);
	p->current_thread = NULL;
	return result;
}

const arPackage* arProcess_find_package(const arProcess* p, const char* name, int len)
{
	const arUint32 num_packages = p->packages.count;
	arUint32 i = 0;
	for (i = 0; i < num_packages; ++i) {
		const arPackage* const package = vmi_list_packages_get(&p->packages, i);
		if (arString_cmpsz(&package->name, name, len)) {
			return package;
		}
	}
	return NULL;
}

const arFunction* arPackage_find_function(const arPackage* p, const char* name, int len)
{
	arUint32 i;
	const arUint32 count = p->functions.count;
	for (i = 0; i < count; ++i) {
		const arFunction* const func = vmi_list_functions_get(&p->functions, i);
		if (arString_cmpsz(&func->name, name, len)) {
			return func;
		}
	}
	return NULL;
}

const arPackage* arProcess_get_package(const arProcess* p, arObjectId id)
{
	return vmi_list_packages_get(&p->packages, id);
}

const arFunction* arProcess_get_function(const arProcess* p, arObjectId id)
{
	if (p->functions_count >= id)
		return NULL;
	return p->functions[id];
}
