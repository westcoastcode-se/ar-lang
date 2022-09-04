#include "arBuilder.h"
#include "builder/types.h"
#include "builder/messages.h"
#include "builder/message_codes.h"
#include "arMemory.h"
#include "arBytecode.h"
#include "arProcess.h"

DEFINE_LIST_BASE_DEFAULT(vmp_list_packages, arB_package, 4, 4);
DEFINE_LIST_FIND(vmp_list_packages, arB_package, name);

arBuilder* arBuilder_new()
{
	arBuilder* const p = arSafeMalloc(arBuilder);
	vmp_list_packages_init(&p->packages);
	p->total_body_size = 0;
	p->total_header_size = 0;
	arStringPool_init(&p->string_pool);
	arMessages_init(&p->messages);
	p->panic_error_message.code = VMP_MESSAGE_NONE;
	arByteStream_init(&p->bytestream);
	return p;
}

void arBuilder_destroy(arBuilder* p)
{
	arByteStream_release(&p->bytestream);
	arMessages_destroy(&p->messages);
	arStringPool_destroy(&p->string_pool);
	vmp_list_packages_release(&p->packages);
	arFree(p);
}

BOOL arBuilder_add_package(arBuilder* p, arB_package* pkg)
{
	if (pkg->pipeline != NULL)
		return FALSE;
	pkg->pipeline = p;
	return vmp_list_packages_add(&p->packages, pkg) >= 0;
}

void ar_pipeline_resolve_locals(arBuilder* p, arB_func* func)
{
	// Augment offsets (where is the memory located from the "ebp"'s point of view).
	// EBP points to where the stack is located 
	const arInt32 num_locals = func->locals.count;
	arInt32 i;
	arInt32 offset = 0;
	for (i = 0; i < num_locals; ++i) {
		arB_local* const local = arB_locals_get(&func->locals, i);
		local->offset = offset;
		offset += local->type->size;
	}
	func->locals_stack_size = offset;
	if (num_locals > 0)
		arB_func_inject_after(func, NULL, arB_instr_locals(func));
}

arInt32 ar_pipeline_resolve_package(arBuilder* p, arInt32 offset, arB_package* pkg)
{
	const arInt32 num_funcs = pkg->funcs.count;
	const arInt32 num_globals = pkg->globals.count;
	arInt32 i;
	
	// Make sure that each package global variable has a known offset
	for (i = 0; i < num_globals; ++i) {
		arB_global* const global = vmp_list_globals_get(&pkg->globals, i);
		global->offset = offset;
		offset += global->type->size;
	}

	// Make sure that each function has a known offset
	for (i = 0; i < num_funcs; ++i) {
		arB_func* const func = arB_funcs_get(&pkg->funcs, i);
		// Make sure that local information is correct
		ar_pipeline_resolve_locals(p, func);
		// Set function properties
		func->offset = offset;

		// If no body size is found then figure out how to call it
		if (func->body_size == 0) {
			if (func->native_func == NULL) {
				// TODO: Figure out how to raise an error that we cant link this function
				return -1;
			}
		}
		else {
			if (func->native_func != NULL) {
				// TODO: Figure out how to raise an error that tells us that we have a body and a native function
				return -1;
			}
		}

		offset += func->body_size;
		// TODO: Add support for function type information
	}
	return offset;
}

void ar_pipeline_resolve_header_size(arBuilder* p)
{
	const arInt32 num_packages = p->packages.count;
	arInt32 size = sizeof(ar_process_header);
	arInt32 i, j;

	for(i = 0; i < num_packages; ++i)
	{
		const arB_package* const package = vmp_list_packages_get(&p->packages, i);
		size += sizeof(ar_package_header) + arString_length(&package->name);

		for(j = 0; j < package->funcs.count; ++j)
		{
			const arB_func* const f = arB_funcs_get(&package->funcs, j);
			size += sizeof(ar_function_header) + arString_length(&f->name);
		}
	}
	p->total_header_size = size;
}

BOOL arBuilder_resolve(arBuilder* p)
{
	const arInt32 size = p->packages.count;
	arInt32 offset = 0;

	// Resolve how big the header is
	ar_pipeline_resolve_header_size(p);

	offset = p->total_header_size;
	for (arInt32 i = 0; i < size; ++i) {
		const arInt32 result = ar_pipeline_resolve_package(p, offset, vmp_list_packages_get(&p->packages, i));
		if (result == -1) {
			return FALSE;
		}
		offset = result;
	}
	p->total_body_size = offset;
	return TRUE;
}

const arString* arBuilder_get_string(arBuilder* p, const char* str, arInt32 len)
{
	return arStringPool_stringsz(&p->string_pool, str, len);
}

BOOL arBuilder_compile_package(arBuilder* b, const arB_package* p)
{
	const arInt32 num_funcs = p->funcs.count;
	const arInt32 num_globals = p->globals.count;
	arInt32 i;

	// Reserve memory for global variables
	for (i = 0; i < num_globals; ++i) {
		arB_global* const g = vmp_list_globals_get(&p->globals, i);
		if (g->type->size == 0) {
			arB_message_type_not_defined(b, &g->type->name);
			return FALSE;
		}
		if (!arBuilder_reserve(b, g->type->size)) {
			arB_panic_out_of_memory(b);
			return FALSE;
		}
	}

	// Compile functions
	for (i = 0; i < num_funcs; ++i) {
		const arB_func* const f = arB_funcs_get(&p->funcs, i);
		const arB_definition* instr = f->first_instr;
		while (instr) {
			instr = arB_instr_build(instr, b);
		}
	}

	return TRUE;
}

void ar_builder_write_header(arBuilder* b)
{
	ar_process_header header;
	header.header[0] = 'V';
	header.header[1] = 'M';
	header.header[2] = '0';
	header.version = VM_VERSION;
	header.data_offset = 0;
	header.code_offset = b->total_header_size;
	header.first_package_offset = sizeof(ar_process_header);
	header.packages_count = b->packages.count;
	arBuilder_write(b, &header, sizeof(ar_process_header));
}

arInt32 ar_builder_get_num_functions(arBuilder* b)
{
	const arInt32 size = b->packages.count;
	arInt32 i;
	arInt32 count = 0;

	for (i = 0; i < size; ++i) {
		const arB_package* const package = vmp_list_packages_get(&b->packages, i);
		count += package->funcs.count;
	}

	return count;
}

void ar_builder_write_metadata(arBuilder* b)
{
	const arInt32 size = b->packages.count;
	arInt32 i, j;
	
	// Memory structure for package information:
	// <Package Header>
	// char[]	| name bytes

	for (i = 0; i < size; ++i) {
		const arB_package* const package = vmp_list_packages_get(&b->packages, i);
		ar_package_header package_header = {
			arString_length(&package->name),
			package->funcs.count,
			package->types.count
		};
		arBuilder_write(b, &package_header, sizeof(ar_package_header));
		arBuilder_write(b, package->name.start, arString_length(&package->name)); // name bytes

		// Memory structure for function information:
		// int32	| name length
		// char[]	| name bytes
		for (j = 0; j < package->funcs.count; ++j) {
			const arB_func* const f = arB_funcs_get(&package->funcs, j);
			ar_function_header func_header = {
				arString_length(&f->name),
				f->offset,
				f->args_stack_size
			};
			arBuilder_write(b, &func_header, sizeof(ar_function_header));
			arBuilder_write(b, f->name.start, arString_length(&f->name)); // name bytes
		}

		// Memory structure for type information:
		// int32	| name length
		// char[]	| name bytes
		// ???
	}
}

BOOL arBuilder_compile(arBuilder* b)
{
	const arInt32 size = b->packages.count;

	// The total size of the bytecode should be the header data + the total body size. We resize this right now
	// so that we know that the memory is not moved due to a resize
	arByteStream_resize(&b->bytestream, b->total_header_size + b->total_body_size);

	ar_builder_write_header(b);
	ar_builder_write_metadata(b);

	// Verify that the header size is as large as what is written
	if (arByteStream_get_size(&b->bytestream) != b->total_header_size) {
		arB_message_header_prediction_failed(b, 
			b->total_header_size, arByteStream_get_size(&b->bytestream));
		return FALSE;
	}
	
	// Compile the eactual bytecode
	for (arInt32 i = 0; i < size; ++i) {
		const arB_package* p = vmp_list_packages_get(&b->packages, i);
		if (!arBuilder_compile_package(b, p)) {
			return FALSE;
		}
	}

	return arBuilder_success(b);
}

BOOL arBuilder_write(arBuilder* builder, const void* ptr, arInt32 size)
{
	if (!arByteStream_write(&builder->bytestream, ptr, size)) {
		arB_panic_out_of_memory(builder);
		return FALSE;
	}
	return TRUE;
}

BOOL arBuilder_reserve(arBuilder* builder, arInt32 size)
{
	arByte* ptr = arByteStream_reserve(&builder->bytestream, size);
	if (ptr == NULL)
		return FALSE;
	memset(ptr, 0, size);
	return TRUE;
}

arByte* arBuilder_get_bytecode(arBuilder* b)
{
	return b->bytestream.memory;
}
