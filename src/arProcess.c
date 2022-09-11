#include "arProcess.h"
#include "arThread.h"
#include "arBytecode.h"

//
// PRIVATE ///////////////////////////////////////////////////////////////////////////
// 

const arByte* arProcess_name_from_package_signature(const arString* str)
{
	if (str->start == str->end)
		return str->start;
	const char* const start = str->start;
	const char* s = str->end - 1;
	for (; s != start; --s) {
		if (*s == '.') {
			s++;
			break;
		}
	}
	return s;
}

const arByte* arProcess_shorthand_function_signature(const arString* str)
{
	if (str->start == str->end)
		return str->start;
	// Search until we've reached the package-function delimiter (#)
	const char* const end = str->end;
	const char* s = str->start;
	for (; s != end; ++s) {
		if (*s == '#') {
			s++;
			break;
		}
	}
	if (s == str->end)
		return str->start;
	return s;
}

const arByte* arProcess_name_from_function_signature(const arString* str)
{
	if (str->start == str->end)
		return str->start;
	// Search until we've reached the first '('
	const char* const end = str->end;
	const char* s = str->start;
	for (; s != end; ++s) {
		if (*s == '(') {
			break;
		}
	}
	if (s == str->end)
		return str->start;
	return s;
}

BOOL _arProcess_load_package_info(arProcess* p, const arProcessHeader* header, const arByte* bytecode)
{
	arUint32 i, j;
	const arByte* ptr = (p->bytecode + header->first_package_offset);
	
	// Prepare each package
	for (i = 0; i < header->packages_count; ++i) {
		const arPackageHeader* header = (const arPackageHeader*)(ptr);
		ptr += sizeof(arPackageHeader);

		arPackage* const current_package = arPackage_new();
		current_package->id = vmi_list_packages_add(&p->packages, current_package);
		current_package->process = p;
		current_package->signature.start = ptr;
		current_package->signature.end = ptr + header->signature_length;
		ptr += header->signature_length;
		// The name is actually part of the signature, so we can use it to figure out the name
		current_package->name = current_package->signature;
		current_package->name.end = arProcess_name_from_package_signature(&current_package->signature);

		for(j = 0; j < header->functions_count; ++j) {
			const arFunctionHeader* fh = (const arFunctionHeader*)(ptr);
			ptr += sizeof(arFunctionHeader);
			arFunction* const current_func = arFunction_new();
			current_func->id = p->functions_count++;
			current_func->signature.start = ptr;
			current_func->signature.end = ptr + fh->signature_length;
			ptr += fh->signature_length;
			// The name is actually part of the signature, so we can use it to figure out the name
			current_func->short_signature = current_func->signature;
			current_func->short_signature.start = arProcess_shorthand_function_signature(&current_func->signature);
			// The name is actually part of the signature, so we can use it to figure out the name
			current_func->name = current_func->short_signature;
			current_func->name.end = arProcess_name_from_function_signature(&current_func->short_signature);
			current_func->ptr = bytecode + fh->ptr_start;
			current_func->expected_stack_size = fh->expected_stack_size;

			vmi_list_functions_add(&current_package->functions, current_func);

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
	const arProcessHeader* header = (const arProcessHeader*)bytecode;
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

const arPackage* arProcess_find_package(const arProcess* p, const char* signature, int len)
{
	const arUint32 num_packages = p->packages.count;
	arUint32 i = 0;
	for (i = 0; i < num_packages; ++i) {
		const arPackage* const package = vmi_list_packages_get(&p->packages, i);
		if (arString_cmpsz(&package->signature, signature, len)) {
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
		if (arString_cmpsz(&func->short_signature, name, len)) {
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
