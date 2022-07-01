#include "vmp.h"
#include "vmp_types.h"
#include "vmp_messages.h"
#include "../interpreter/vmi_process.h"
#include "../vmc/vmc_debug.h"

void vmp_pipeline_add_vm_package(vmp_pipeline* p)
{
	// Create vm package
	vmp_package* const vm = vmp_package_newsz("vm", 2);
	vmp_pipeline_add_package(p, vm);

	// Add default types
	vmp_package_new_typesz(vm, "int8", 4, sizeof(vm_int8));
	vmp_package_new_typesz(vm, "uint8", 5, sizeof(vm_uint8));
	vmp_package_new_typesz(vm, "int16", 5, sizeof(vm_int16));
	vmp_package_new_typesz(vm, "uint16", 6, sizeof(vm_uint16));
	vmp_package_new_typesz(vm, "int32", 5, sizeof(vm_int32));
	vmp_package_new_typesz(vm, "uint32", 6, sizeof(vm_uint32));
	vmp_package_new_typesz(vm, "int64", 5, sizeof(vm_int64));
	vmp_package_new_typesz(vm, "uint64", 6, sizeof(vm_uint64));
	vmp_package_new_typesz(vm, "float32", 7, sizeof(vm_float32));
	vmp_package_new_typesz(vm, "float64", 7, sizeof(vm_float64));
}

vmp_pipeline* vmp_pipeline_new()
{
	vmp_pipeline* p = (vmp_pipeline*)vmc_malloc(sizeof(vmp_pipeline));
	if (p == NULL)
		return NULL;
	vmp_list_packages_init(&p->packages);
	vmp_pipeline_add_vm_package(p);
	return p;
}

void vmp_pipeline_destroy(vmp_pipeline* p)
{
	vmp_list_packages_release(&p->packages);
	vmc_free(p);
}

BOOL vmp_pipeline_add_package(vmp_pipeline* p, vmp_package* pkg)
{
	if (pkg->pipeline != NULL)
		return FALSE;
	pkg->pipeline = p;
	return vmp_list_packages_add(&p->packages, pkg) >= 0;
}

vmp_package* vmp_pipeline_find_package(vmp_pipeline* p, const vm_string* name)
{
	return vmp_list_packages_find(&p->packages, name);
}

vmp_package* vmp_pipeline_find_packagesz(vmp_pipeline* p, const char* name, vm_int32 len)
{
	vm_string str = { name, name + len };
	return vmp_list_packages_find(&p->packages, &str);
}

BOOL vmp_pipeline_resolve_args(vmp_pipeline* p, vm_int32 offset, vmp_func* func)
{
	// The offset represents where a specific argument can be found. The order is backwards, because 
	// you push values from the right to the left (i.e. memory for the last return value is pushed first)
	const vm_int32 num_args = func->args.count;
	const vm_int32 num_rets = func->returns.count;
	vm_int32 i;
	vm_int32 stack_offset = func->args_stack_size + func->returns_stack_size;

	for (i = 0; i < num_args; ++i) {
		vmp_arg* const arg = vmp_list_args_get(&func->args, i);
		stack_offset -= arg->type->size;
		arg->offset = stack_offset;
	}

	for (i = 0; i < num_rets; ++i) {
		vmp_return* const ret = vmp_list_returns_get(&func->returns, i);
		stack_offset -= ret->type->size;
		ret->offset = stack_offset;
	}

	return TRUE;
}

vm_int32 vmp_pipeline_resolve_package(vmp_pipeline* p, vm_int32 offset, vmp_package* pkg)
{
	const vm_int32 num_funcs = pkg->funcs.count;

	// Make sure that each function has a known offset
	for (vm_int32 i = 0; i < num_funcs; ++i) {
		vmp_func* const func = vmp_list_funcs_get(&pkg->funcs, i);
		// Make sure that all information of the arguments are known
		if (!vmp_pipeline_resolve_args(p, offset, func)) {
			return FALSE;
		}
		// Set function properties
		func->offset = offset;
		offset += func->body_size;
		// TODO: Add support for function type information
	}
	return offset;
}

BOOL vmp_pipeline_resolve(vmp_pipeline* p)
{
	const vm_int32 size = p->packages.count;
	vm_int32 offset = sizeof(vmi_process_header);
	for (vm_int32 i = 0; i < size; ++i) {
		const vm_int32 result = vmp_pipeline_resolve_package(p, offset, vmp_list_packages_get(&p->packages, i));
		if (result == -1) {
			return FALSE;
		}
		offset = result;
	}
	return TRUE;
}

vmp_builder* vmp_builder_new(vmp_pipeline* p)
{
	vmp_builder* b = (vmp_builder*)vmc_malloc(sizeof(vmp_builder));
	if (b == NULL)
		return NULL;
	b->pipeline = p;

	vm_bytestream_init(&b->bytestream);
	vm_messages_init(&b->messages);
	b->panic_error_message.code = VMP_MESSAGE_NONE;
	return b;
}

BOOL vmp_builder_compile_package(vmp_builder* b, const vmp_package* p)
{
	const vm_int32 size = p->funcs.count;
	vm_int32 i;
	for (i = 0; i < size; ++i) {
		const vmp_func* const f = vmp_list_funcs_get(&p->funcs, i);
		const vmp_instr* instr = f->first_instr;
		while (instr) {
			instr = vmp_instr_build(instr, b);
		}

	}

	return TRUE;
}

void vmp_builder_write_header(vmp_builder* b)
{
	vmi_process_header header;
	header.header[0] = 'V';
	header.header[1] = 'M';
	header.header[2] = '0';
	header.version = VM_VERSION;
	header.data_offset = 0;
	header.code_offset = sizeof(vmi_process_header);
	header.first_package_offset = 0;
	header.packages_count = b->pipeline->packages.count;
	vmp_builder_write(b, &header, sizeof(vmi_process_header));
}

vmi_process_header* vmp_builder_get_header(vmp_builder* b)
{
	return ((vmi_process_header*)vm_bytestream_get(&b->bytestream, 0));
}

vm_int32 vmp_builder_get_num_functions(vmp_builder* b)
{
	const vm_int32 size = b->pipeline->packages.count;
	const vmp_package* package;
	vm_int32 i;
	vm_int32 count = 0;

	for (i = 0; i < size; ++i) {
		package = vmp_list_packages_get(&b->pipeline->packages, i);
		count += package->funcs.count;
	}

	return count;
}

void vmp_builder_set_header_info(vmp_builder* b)
{
	const vm_int32 size = b->pipeline->packages.count;
	const vmp_package* package;
	vm_int32 i, j;

	// Set how many packages compiled into the bytecode
	vmp_builder_get_header(b)->packages_count = size;
	// Set how many functions compiled into the bytecode
	vmp_builder_get_header(b)->functions_count = vmp_builder_get_num_functions(b);
	// Set the actual offset of where the package information is found
	vmp_builder_get_header(b)->first_package_offset = vm_bytestream_get_size(&b->bytestream);

	// Memory structure for package information:
	// <Package Header>
	// char[]	| name bytes

	for (i = 0; i < size; ++i) {
		package = vmp_list_packages_get(&b->pipeline->packages, i);
		vmi_package_bytecode_header package_header = {
			vm_string_length(&package->name),
			package->funcs.count,
			package->types.count,
			0,
			0
		};
		vmp_builder_write(b, &package_header, sizeof(vmi_package_bytecode_header));
		vmp_builder_write(b, (void*)package->name.start, vm_string_length(&package->name)); // name bytes
	}

	// Memory structure for function information:
	// int32	| name length
	// char[]	| name bytes

	for (i = 0; i < size; ++i) {
		package = vmp_list_packages_get(&b->pipeline->packages, i);
		for (j = 0; j < package->funcs.count; ++j) {
			const vmp_func* const f = vmp_list_funcs_get(&package->funcs, j);
			vmi_package_func_bytecode_header func_header = {
				vm_string_length(&f->name),
				f->offset,
				f->args_stack_size + f->returns_stack_size
			};
			vmp_builder_write(b, &func_header, sizeof(vmi_package_func_bytecode_header));
			vmp_builder_write(b, (void*)f->name.start, vm_string_length(&f->name)); // name bytes
		}
	}

	// Memory structure for type information:
	// int32	| name length
	// char[]	| name bytes
}

BOOL vmp_builder_compile(vmp_builder* b)
{
	const vm_int32 size = b->pipeline->packages.count;
	vmp_builder_write_header(b);

	for (vm_int32 i = 0; i < size; ++i) {
		const vmp_package* p = vmp_list_packages_get(&b->pipeline->packages, i);
		if (!vmp_builder_compile_package(b, p)) {
			return FALSE;
		}
	}
	vmp_builder_set_header_info(b);
	return TRUE;
}

void vmp_builder_destroy(vmp_builder* b)
{
	vm_messages_destroy(&b->messages);
	vm_bytestream_release(&b->bytestream);
	vmc_free(b);
}

BOOL vmp_builder_write(vmp_builder* builder, const void* ptr, vm_int32 size)
{
	if (!vm_bytestream_write(&builder->bytestream, ptr, size)) {
		vmp_builder_panic_out_of_memory(builder);
		return FALSE;
	}
	return TRUE;
}

vm_byte* vmp_builder_get_bytecode(vmp_builder* b)
{
	return b->bytestream.memory;
}
