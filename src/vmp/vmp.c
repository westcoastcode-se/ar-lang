#include "vmp.h"
#include "vmp_types.h"
#include "vmp_messages.h"
#include "../interpreter/vmi_process.h"
#include "../vmc/vmc_debug.h"

//
// Constants
//

VM_STRING_CONST(bool, "bool", 4);
VM_STRING_CONST(pbool, "*bool", 5);
VM_STRING_CONST(int8, "int8", 4);
VM_STRING_CONST(pint8, "*int8", 5);
VM_STRING_CONST(uint8, "uint8", 5);
VM_STRING_CONST(puint8, "*uint8", 6);
VM_STRING_CONST(int16, "int16", 5);
VM_STRING_CONST(pint16, "*int16", 6);
VM_STRING_CONST(uint16, "uint16", 6);
VM_STRING_CONST(puint16, "*uint16", 7);
VM_STRING_CONST(int32, "int32", 5);
VM_STRING_CONST(pint32, "*int32", 6);
VM_STRING_CONST(uint32, "uint32", 6);
VM_STRING_CONST(puint32, "*uint32", 7);
VM_STRING_CONST(int64, "int64", 5);
VM_STRING_CONST(pint64, "*int64", 6);
VM_STRING_CONST(uint64, "uint64", 6);
VM_STRING_CONST(puint64, "*uint64", 7);
VM_STRING_CONST(float32, "float32", 7);
VM_STRING_CONST(pfloat32, "*float32", 8);
VM_STRING_CONST(float64, "float64", 7);
VM_STRING_CONST(pfloat64, "*float64", 8);

void vmp_pipeline_add_vm_package(vmp_pipeline* p)
{
	vmp_type* type;

	// Create vm package
	vmp_package* const vm = vmp_package_newsz("vm", 2);
	vmp_pipeline_add_package(p, vm);

	//
	// Add default types
	//

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(bool), sizeof(vm_int32), 0, VMI_INSTR_PROP_BOOL, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(pbool), sizeof(BOOL*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(int8), sizeof(vm_int8), 0, VMI_INSTR_PROP_INT8, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(pint8), sizeof(vm_int8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(uint8), sizeof(vm_uint8), 0, VMI_INSTR_PROP_UINT8, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(puint8), sizeof(vm_uint8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(int16), sizeof(vm_int16), 0, VMI_INSTR_PROP_INT16, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(pint16), sizeof(vm_int16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(uint16), sizeof(vm_uint16), 0, VMI_INSTR_PROP_UINT16, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(puint16), sizeof(vm_uint16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(int32), sizeof(vm_int32), 0, VMI_INSTR_PROP_INT32, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(pint32), sizeof(vm_int32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(uint32), sizeof(vm_uint32), 0, VMI_INSTR_PROP_UINT32, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(puint32), sizeof(vm_uint32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(int64), sizeof(vm_int64), 0, VMI_INSTR_PROP_INT64, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(pint64), sizeof(vm_int64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(uint64), sizeof(vm_uint64), 0, VMI_INSTR_PROP_UINT64, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(puint64), sizeof(vm_uint64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(float32), sizeof(vm_float32), 0, VMI_INSTR_PROP_FLOAT32, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(pfloat32), sizeof(vm_float32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);

	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(float64), sizeof(vm_float64), 0, VMI_INSTR_PROP_FLOAT64, NULL));
	vmp_package_add_type(vm, type);
	type = vmp_type_new_from_props(vmp_type_props_get(VM_STRING_CONST_GET(pfloat64), sizeof(vm_float64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmp_package_add_type(vm, type);
}

vmp_pipeline* vmp_pipeline_new()
{
	vmp_pipeline* p = (vmp_pipeline*)vmc_malloc(sizeof(vmp_pipeline));
	if (p == NULL)
		return NULL;
	vmp_list_packages_init(&p->packages);
	p->total_body_size = 0;
	p->total_header_size = 0;
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

void vmp_pipeline_resolve_args(vmp_pipeline* p, vmp_func* func)
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
}

void vmp_pipeline_resolve_locals(vmp_pipeline* p, vmp_func* func)
{
	// Augment offsets (where is the memory located from the "ebp"'s point of view).
	// EBP points to where the stack is located 
	const vm_int32 num_locals = func->locals.count;
	vm_int32 i;
	vm_int32 offset = func->returns_stack_size + func->args_stack_size + sizeof(vmi_ip) + sizeof(vm_byte*);
	for (i = 0; i < num_locals; ++i) {
		vmp_local* const local = vmp_list_locals_get(&func->locals, i);
		local->offset = offset;
		offset += local->type->size;
	}
}

vm_int32 vmp_pipeline_resolve_package(vmp_pipeline* p, vm_int32 offset, vmp_package* pkg)
{
	const vm_int32 num_funcs = pkg->funcs.count;

	// Make sure that each function has a known offset
	for (vm_int32 i = 0; i < num_funcs; ++i) {
		vmp_func* const func = vmp_list_funcs_get(&pkg->funcs, i);
		// Make sure that all information of the arguments are known
		vmp_pipeline_resolve_args(p, func);
		// Make sure that local information is correct
		vmp_pipeline_resolve_locals(p, func);
		// Set function properties
		func->offset = offset;
		offset += func->body_size;
		// TODO: Add support for function type information
	}
	return offset;
}

void vmp_pipeline_resolve_header_size(vmp_pipeline* p)
{
	const vm_int32 num_packages = p->packages.count;
	vm_int32 size = sizeof(vmi_process_header);
	vm_int32 i, j;

	for(i = 0; i < num_packages; ++i)
	{
		const vmp_package* const package = vmp_list_packages_get(&p->packages, i);
		size += sizeof(vmi_package_bytecode_header) + vm_string_length(&package->name);

		for(j = 0; j < package->funcs.count; ++j)
		{
			const vmp_func* const f = vmp_list_funcs_get(&package->funcs, j);
			size += sizeof(vmi_package_func_bytecode_header) + vm_string_length(&f->name);
		}
	}
	p->total_header_size = size;
}

BOOL vmp_pipeline_resolve(vmp_pipeline* p)
{
	const vm_int32 size = p->packages.count;
	vm_int32 offset = 0;

	// Resolve how big the header is
	vmp_pipeline_resolve_header_size(p);

	offset = p->total_header_size;
	for (vm_int32 i = 0; i < size; ++i) {
		const vm_int32 result = vmp_pipeline_resolve_package(p, offset, vmp_list_packages_get(&p->packages, i));
		if (result == -1) {
			return FALSE;
		}
		offset = result;
	}
	p->total_body_size = offset;
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
	header.code_offset = b->pipeline->total_header_size;
	header.first_package_offset = sizeof(vmi_process_header);
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
	vm_int32 i;
	vm_int32 count = 0;

	for (i = 0; i < size; ++i) {
		const vmp_package* const package = vmp_list_packages_get(&b->pipeline->packages, i);
		count += package->funcs.count;
	}

	return count;
}

void vmp_builder_write_metadata(vmp_builder* b)
{
	const vm_int32 size = b->pipeline->packages.count;
	vm_int32 i, j;
	
	// Memory structure for package information:
	// <Package Header>
	// char[]	| name bytes

	for (i = 0; i < size; ++i) {
		const vmp_package* const package = vmp_list_packages_get(&b->pipeline->packages, i);
		vmi_package_bytecode_header package_header = {
			vm_string_length(&package->name),
			package->funcs.count,
			package->types.count
		};
		vmp_builder_write(b, &package_header, sizeof(vmi_package_bytecode_header));
		vmp_builder_write(b, package->name.start, vm_string_length(&package->name)); // name bytes

		// Memory structure for function information:
		// int32	| name length
		// char[]	| name bytes
		for (j = 0; j < package->funcs.count; ++j) {
			const vmp_func* const f = vmp_list_funcs_get(&package->funcs, j);
			vmi_package_func_bytecode_header func_header = {
				vm_string_length(&f->name),
				f->offset,
				f->args_stack_size + f->returns_stack_size
			};
			vmp_builder_write(b, &func_header, sizeof(vmi_package_func_bytecode_header));
			vmp_builder_write(b, f->name.start, vm_string_length(&f->name)); // name bytes
		}

		// Memory structure for type information:
		// int32	| name length
		// char[]	| name bytes
		// ???
	}
}

BOOL vmp_builder_compile(vmp_builder* b)
{
	const vm_int32 size = b->pipeline->packages.count;

	// The total size of the bytecode should be the header data + the total body size. We resize this right now
	// so that we know that the memory is not moved due to a resize
	vm_bytestream_resize(&b->bytestream, b->pipeline->total_header_size + b->pipeline->total_body_size);

	vmp_builder_write_header(b);
	vmp_builder_write_metadata(b);

	// Verify that the header size is as large as what is written
	if (vm_bytestream_get_size(&b->bytestream) != b->pipeline->total_header_size) {
		vmp_builder_message_header_prediction_failed(b, 
			b->pipeline->total_header_size, vm_bytestream_get_size(&b->bytestream));
		return FALSE;
	}
	
	// Compile the eactual bytecode
	for (vm_int32 i = 0; i < size; ++i) {
		const vmp_package* p = vmp_list_packages_get(&b->pipeline->packages, i);
		if (!vmp_builder_compile_package(b, p)) {
			return FALSE;
		}
	}

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