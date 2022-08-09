#include "vmcode.h"
#include "vmcd_messages.h"
#include "vmcd_source_code.h"
#include "vmcd_types.h"
#include <inttypes.h>

//
// Constants
//

VM_STRING_CONST(vmcd_bool, "bool", 4);
VM_STRING_CONST(vmcd_pbool, "*bool", 5);
VM_STRING_CONST(vmcd_int8, "int8", 4);
VM_STRING_CONST(vmcd_pint8, "*int8", 5);
VM_STRING_CONST(vmcd_uint8, "uint8", 5);
VM_STRING_CONST(vmcd_puint8, "*uint8", 6);
VM_STRING_CONST(vmcd_int16, "int16", 5);
VM_STRING_CONST(vmcd_pint16, "*int16", 6);
VM_STRING_CONST(vmcd_uint16, "uint16", 6);
VM_STRING_CONST(vmcd_puint16, "*uint16", 7);
VM_STRING_CONST(vmcd_int32, "int32", 5);
VM_STRING_CONST(vmcd_pint32, "*int32", 6);
VM_STRING_CONST(vmcd_uint32, "uint32", 6);
VM_STRING_CONST(vmcd_puint32, "*uint32", 7);
VM_STRING_CONST(vmcd_int64, "int64", 5);
VM_STRING_CONST(vmcd_pint64, "*int64", 6);
VM_STRING_CONST(vmcd_uint64, "uint64", 6);
VM_STRING_CONST(vmcd_puint64, "*uint64", 7);
VM_STRING_CONST(vmcd_float32, "float32", 7);
VM_STRING_CONST(vmcd_pfloat32, "*float32", 8);
VM_STRING_CONST(vmcd_float64, "float64", 7);
VM_STRING_CONST(vmcd_pfloat64, "*float64", 8);

vmcd_package* vmcode_find_package(vmcode* v, const vm_string* name)
{
	return vmcd_list_packages_find(&v->packages, name);
}

vm_int32 vmcode_add_package(vmcode* v, vmcd_package* p)
{
	// Always add the "vm" package
	vmcd_package_add_import(p, vmcd_list_packages_get(&v->packages, 0));
	return vmcd_list_packages_add(&v->packages, p);
}

void vmcode_vm_package(vmcode* p)
{
	vmcd_type* type;

	// Create vm package
	vm_string name;
	name.start = "vm";
	name.end = name.start + 2;
	vmcd_package* const vm = vmcd_package_new(&name);
	vmcd_list_packages_add(&p->packages, vm);

	//
	// Add default types
	//

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_bool), sizeof(vm_int32), 0, VMI_INSTR_PROP_BOOL, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_pbool), sizeof(BOOL*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_int8), sizeof(vm_int8), 0, VMI_INSTR_PROP_INT8, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_pint8), sizeof(vm_int8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_uint8), sizeof(vm_uint8), 0, VMI_INSTR_PROP_UINT8, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_puint8), sizeof(vm_uint8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_int16), sizeof(vm_int16), 0, VMI_INSTR_PROP_INT16, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_pint16), sizeof(vm_int16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_uint16), sizeof(vm_uint16), 0, VMI_INSTR_PROP_UINT16, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_puint16), sizeof(vm_uint16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_int32), sizeof(vm_int32), 0, VMI_INSTR_PROP_INT32, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_pint32), sizeof(vm_int32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_uint32), sizeof(vm_uint32), 0, VMI_INSTR_PROP_UINT32, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_puint32), sizeof(vm_uint32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_int64), sizeof(vm_int64), 0, VMI_INSTR_PROP_INT64, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_pint64), sizeof(vm_int64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_uint64), sizeof(vm_uint64), 0, VMI_INSTR_PROP_UINT64, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_puint64), sizeof(vm_uint64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_float32), sizeof(vm_float32), 0, VMI_INSTR_PROP_FLOAT32, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_pfloat32), sizeof(vm_float32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);

	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_float64), sizeof(vm_float64), 0, VMI_INSTR_PROP_FLOAT64, NULL));
	vmcd_package_add_type(vm, type);
	type = vmcd_type_new_from_props(vmcd_type_props_get(VM_STRING_CONST_GET(vmcd_pfloat64), sizeof(vm_float64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	vmcd_package_add_type(vm, type);
}

// Parse a type from a specific package. It is assumed that everything in the package is already processed and "compiled"
vmcd_type* vmcd_parse_type_from_package(const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	return vmcd_package_find_type(s->package, &t->string);
}

// Parse a type. If the returned type is NULL then a message will be put on the message queue with the error
vmcd_type* vmcd_parse_type(const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	if (t->type == VMCD_TOKEN_PTR) {
		// If no type was found then this is from a package and it wasn't found. This can be deduced because
		// a type from "this" package is always temporarily created with an unknown size
		vmcd_type* base_type = vmcd_parse_type(s);
		if (base_type == NULL)
			return NULL;
		char memory[128];

		// Stringify the type
		memory[0] = '*';
		vm_str_cpy(&memory[1], &VMCD_KEYWORD(base_type)->name);
		const vm_string* pointer_type_name = vm_string_pool_stringsz(&s->vmcd->string_pool, memory, vm_string_length(&base_type->header.name) + 1);
		if (pointer_type_name == NULL) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}

		vmcd_type* type = vmcd_package_find_type(base_type->package, pointer_type_name);
		// Creat the type if it doesn't exist
		if (type == NULL) {
			type = vmcd_type_new(pointer_type_name);
			type->size = sizeof(vm_byte*);
			type->flags |= VMCD_TYPE_FLAG_PTR;
			vmcd_type_of_type(type, base_type);
			if (!vmcd_package_add_type(base_type->package, type)) {
				vmcd_message_out_of_memory(s);
				return NULL;
			}
		}
		return type;
	}
	else if (t->type == VMCD_TOKEN_SQUARE_L) {
		vmcd_message_not_implemented(s);
		return NULL;
	}
	else if (t->type != VMCD_TOKEN_KEYWORD) {
		vmcd_message_expected_identifier(s);
		return NULL;
	}

	// You might specify a full path to the type: "System.Int32Package1.Package2.Type.InnerType".
	// The first type is considered the current class first
	vmcd_package* const imported = vmcd_package_find_import(s->package, &t->string);
	if (imported != NULL) {
		if (!vmcd_token_next_type(t, VMCD_TOKEN_DOT)) {
			vmcd_message_syntax_error(s, '.');
			return NULL;
		}

		vmcd_scope inner_scope = *s;
		inner_scope.package = imported;
		inner_scope.parent = s;
		return vmcd_parse_type_from_package(&inner_scope);
	}

	// Try to fetch the type. If the type doesn't exist then assume that this isn't from a package and can thus be
	// created artificially with an unknown size
	vmcd_type* type = vmcd_package_find_type_include_imports(s->package, &t->string);
	if (type == NULL) {
		// If the type isn't found then assume that the type isn't declared yet
		type = vmcd_type_new(&t->string);
		if (!vmcd_package_add_type(s->package, type)) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}
	}
	return type;
}

// Functions have the syntax: "func Name(<args>) (<returns>)
BOOL vmcd_parse_func(const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	if (!vmcd_token_next_type(t, VMCD_TOKEN_KEYWORD))
		return vmcd_message_expected_identifier(s);

	// This function might've been declared indirectly through a function call
	vmcd_func* func = vmcd_package_find_func(s->package, &t->string);
	if (func == NULL) {
		func = vmcd_func_new(&t->string);
		if (!vmcd_package_add_func(s->package, func)) {
			return vmcd_message_out_of_memory(s);
		}
	}
	if (func->syntax_tree != NULL) {
		// Body declared twice? This might be a function with another signature
		return vmcd_message_not_implemented(s);
	}

	// Expected a '(' token
	if (!vmcd_token_next_type(t, VMCD_TOKEN_PARAN_L))
		return vmcd_message_syntax_error(s, '(');

	// Parse until we reach that end ')' token
	while (!vmcd_token_next_type(t, VMCD_TOKEN_PARAN_R)) {
		// Ignore comma
		if (t->type == VMCD_TOKEN_COMMA) {
			if (!vmcd_token_next_type(t, VMCD_TOKEN_KEYWORD))
				return vmcd_message_expected_identifier(s);
		}

		// Identity first
		vmcd_arg* arg = vmcd_arg_new(&t->string);
		vmcd_func_add_arg(func, arg);

		vmcd_type* type = vmcd_parse_type(s);
		if (type == NULL) {
			// We haven't encountered the type yet?
			return vmcd_message_not_implemented(s);
		}
		arg->type = VMCD_KEYWORD(type);
	}

	vmcd_token_next(t);

	// Function might have one or multiple return values
	if (t->type == VMCD_TOKEN_PARAN_L) {
		// Parse until we reach that end ')' token
		while (!vmcd_token_next_type(t, VMCD_TOKEN_PARAN_R)) {
			// Ignore comma
			if (t->type == VMCD_TOKEN_COMMA) {
				if (!vmcd_token_next_type(t, VMCD_TOKEN_KEYWORD))
					return vmcd_message_expected_identifier(s);
			}

			vmcd_return* ret = vmcd_return_new();
			vmcd_func_add_return(func, ret);

			vmcd_type* type = vmcd_parse_type(s);
			if (type == NULL) {
				return vmcd_message_not_implemented(s);
			}
			ret->type = VMCD_KEYWORD(type);
		}
	}
	else if (t->type == VMCD_TOKEN_KEYWORD) {
		vmcd_return* ret = vmcd_return_new();
		vmcd_func_add_return(func, ret);

		vmcd_type* type = vmcd_parse_type(s);
		if (type == NULL) {
			return vmcd_message_not_implemented(s);
		}
		ret->type = VMCD_KEYWORD(type);
		vmcd_token_next(t);
	}

	if (t->type != VMCD_TOKEN_BRACKET_L) {
		// Expected function body
		return vmcd_message_syntax_error(s, '{');
	}
	vmcd_token_next(t);

	vmcd_scope inner_scope = *s;
	inner_scope.func = func;
	inner_scope.parent = s;

	vmcd_syntax_tree_node st;
	int depth = 1;

	while (depth > 0) {
		if (t->type == VMCD_TOKEN_BRACKET_L) {
			depth++;
			vmcd_token_next(t);
			continue;
		}
		else if (t->type == VMCD_TOKEN_BRACKET_R) {
			depth--;
			vmcd_token_next(t);
			continue;
		}
		else if (t->type == VMCD_TOKEN_NEWLINE) {
			vmcd_token_next(t);
			continue;
		}

		st = vmcd_syntax_tree_next(&inner_scope);
		if (st == NULL)
			return FALSE;

		vmcd_func_add_syntax_tree(func, st);
	}
	return TRUE;
}

// Parse the package we are in
BOOL vmcd_parse_package(const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	// Fetch the name of the package
	if (!vmcd_token_next_type(s->token, VMCD_TOKEN_KEYWORD))
		return vmcd_message_expected_identifier(s);

	// Try to get/create the package
	vmcd_package* package = vmcode_find_package(s->vmcd, &s->token->string);
	if (package == NULL) {
		package = vmcd_package_new(&s->token->string);
		if (package == NULL)
			return vmcd_message_out_of_memory(s);
		vmcode_add_package(s->vmcd, package);
	}

	// A package supports the following keywords (for now):
	// * func
	// * import
	// * type
	// * const NAME = <expr>
	while (1)
	{
		vmcd_token_next(t);
		if (t->type == VMCD_TOKEN_KEYWORD_FUNC) {
			vmcd_scope inner_scope = *s;
			inner_scope.package = package;
			inner_scope.parent = s;
			if (!vmcd_parse_func(&inner_scope)) {
				return FALSE;
			}
		}
		else if (t->type == VMCD_TOKEN_KEYWORD_IMPORT) {
			// Fetch the package name to import
			if (!vmcd_token_next_type(s->token, VMCD_TOKEN_VALUE_STRING))
				return vmcd_message_expected_identifier(s);
			
			vmcd_package* imported = vmcode_find_package(s->vmcd, &s->token->string);
			if (imported == NULL) {
				return vmcd_message_package_not_found(s);
			}

			if (!vmcd_package_add_import(package, imported)) {
				return vmcd_message_out_of_memory(s);
			}
		}
		else if (t->type == VMCD_TOKEN_KEYWORD_CONST) {
			vmcd_message_not_implemented(s);
			return FALSE;
		}
		else if (t->type == VMCD_TOKEN_KEYWORD_TYPE) {
			vmcd_message_not_implemented(s);
			return FALSE;
		}
		else if (t->type == VMCD_TOKEN_KEYWORD_IF) {
			vmcd_message_not_implemented(s);
			return FALSE;
		}
		else if (t->type == VMCD_TOKEN_NEWLINE || t->type == VMCD_TOKEN_COMMENT) {
			// Ignore these types of tokens
		}
		else if (t->type == VMCD_TOKEN_EOF) {
			return TRUE;
		}
		else {
			vmcd_message_unknown_token(s);
			return FALSE;
		}
	}
}

BOOL vmcd_begin(vmcode* p, vmcd_token* t)
{
	const vmcd_scope scope = {p, p->pipeline, t, NULL, NULL, NULL};
	while (1)
	{
		if (t->type == VMCD_TOKEN_KEYWORD_PACKAGE) {
			// First token has to be a package. If this failed then abort the parsing of the source code
			if (!vmcd_parse_package(&scope)) {
				return FALSE;
			}
		}
		else if (t->type == VMCD_TOKEN_NEWLINE || t->type == VMCD_TOKEN_COMMENT) {
			// Ignore these types of tokens
		}
		else if (t->type == VMCD_TOKEN_EOF) {
			// Done!
			break;
		}		
		else {
			vmcd_message_unknown_token(&scope);
			break;
		}
		vmcd_token_next(t);
	}

	return TRUE;
}

vmcode* vmcode_new()
{
	vmcode* p = (vmcode*)malloc(sizeof(vmcode));
	if (p == NULL)
		return NULL;
	vm_string_pool_init(&p->string_pool);
	vmcd_list_sources_init(&p->source_codes);
	vmcd_list_packages_init(&p->packages);
	p->pipeline = vmp_pipeline_new();
	p->builder = NULL;
	vm_messages_init(&p->messages);
	p->panic_error_message.code = VMCD_MESSAGE_NONE;
	return p;
}

void vmcode_destroy(vmcode* p)
{
	vm_messages_destroy(&p->messages);
	if (p->builder != NULL) {
		vmp_builder_destroy(p->builder);
	}
	if (p->pipeline != NULL) {
		vmp_pipeline_destroy(p->pipeline);
	}
	vmcd_list_packages_release(&p->packages);
	vmcd_list_sources_release(&p->source_codes);
	vm_string_pool_destroy(&p->string_pool);
	free(p);
}

BOOL vmcode_add_source_code(vmcode* p, const vm_byte* source_code, const vm_byte* filename)
{
	vmcd_source_code* src = vmcd_source_code_create(source_code);
	vmcd_source_code_set_filename(src, filename, vm_str_len(filename));
	return vmcd_list_sources_add(&p->source_codes, src) >= 0;
}

BOOL vmcode_parse(vmcode* p, const vm_byte* filename)
{
	vmcode_vm_package(p);

	vmcd_lexer l;
	vmcd_token t;

	const vm_string filename_str = { filename, filename + vm_str_len(filename) };
	const vmcd_source_code* const source_code = vmcd_list_sources_find(&p->source_codes, &filename_str);
	if (source_code == NULL) {
		return FALSE;
	}

	vmcd_lexer_init(&l, source_code->source_code.start);
	vmcd_token_init(&t, &l);
	if (!vmcd_begin(p, &t)) {
		vm_messages_move(&t.messages, &p->messages);
		vmcd_token_release(&t);
		vmcd_lexer_release(&l);
		return FALSE;
	}

	// Resolve memory sizes and offsets for function, argument, returns, locals etc
	if (!vmp_pipeline_resolve(p->pipeline)) {
		vm_messages_move(&t.messages, &p->messages);
		vm_messages_move(&p->pipeline->messages, &p->messages);
		vmcd_token_release(&t);
		vmcd_lexer_release(&l);
		return FALSE;
	}

	vmcd_token_release(&t);
	vmcd_lexer_release(&l);
	return TRUE;
}

BOOL vmcode_link(vmcode* p)
{
	// Start by solving all pending types
	vm_int32 i;
	const vm_int32 count = p->packages.count;
	for (i = 0; i < count; ++i) {
		vmp_package* const package = vmcd_package_resolve_package(vmcd_list_packages_get(&p->packages, i), p);
		if (!vmp_pipeline_add_package(p->pipeline, package)) {
			return FALSE;
		}
	}

	// Now it's time to compile each package
	for (i = 0; i < count; ++i) {
		if (!vmcd_package_compile(vmcd_list_packages_get(&p->packages, i), p)) {
			return FALSE;
		}
	}

	if (!vmp_pipeline_resolve(p->pipeline)) {
		//
		return FALSE;
	}

	// Let the actual builder compile the bytecode
	p->builder = vmp_builder_new(p->pipeline);
	vmp_builder_set_opt_level(p->builder, 0);
	if (!vmp_builder_compile(p->builder)) {
		vm_messages_move(&p->builder->messages, &p->messages);
		return FALSE;
	}
	return TRUE;
}

const vm_byte* vmcode_get_bytecode(vmcode* p)
{
	if (p->builder == NULL)
		return NULL;
	return vmp_builder_get_bytecode(p->builder);
}
