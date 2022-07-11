#include "vmcode.h"
#include "vmcd_messages.h"
#include <inttypes.h>

vmp_type* vmcd_parse_type(const vmcd_scope* s, BOOL include_imports)
{
	vmcd_token* const t = s->token;
	if (t->type == VMCD_TOKEN_PTR) {
		vmp_type* base_type = vmcd_parse_type(s, include_imports);
		if (base_type == NULL)
			return NULL;
		char memory[128];

		// Stringify the type
		memory[0] = '*';
		vm_str_cpy(&memory[1], base_type->name.start, vm_string_length(&base_type->name));
		const vm_string* pointer_type_name = vmp_pipeline_get_string(s->pipeline, memory, vm_string_length(&base_type->name) + 1);
		if (pointer_type_name == NULL) {
			vmcd_message_out_of_memory(s);
			return NULL;
		}

		// Create the type if it doesn't exist
		vmp_type* type = vmp_package_find_type(base_type->package, pointer_type_name);
		if (type == NULL) {
			type = vmp_type_new(pointer_type_name);
			type->size = sizeof(vm_byte*);
			type->flags |= VMP_TYPE_FLAGS_PTR;
			if (!vmp_type_of_type(type, base_type)) {
				vmcd_message_panic(s, "we are adding the same type multiple times");
				return NULL;
			}
			switch (vmp_package_add_type(base_type->package, type)) {
			case VMP_LIST_ADDED:
				break;
			default:
				// Could not add type. TODO: Add better message
				vmcd_message_not_implemented(s);
				return NULL;
			}
		}
		return type;
	}
	else if (t->type == VMCD_TOKEN_SQUARE_L) {
		vmcd_message_panic(s, "not implemented!");
		return NULL;
	}
	else if (t->type != VMCD_TOKEN_KEYWORD) {
		vmcd_message_expected_identifier(s);
		return NULL;
	}

	// This is an imported package?
	vmp_package* const imported = vmp_package_find_import(s->package, &t->string);
	if (imported != NULL) {
		if (!vmcd_token_next_type(t, VMCD_TOKEN_DOT)) {
			vmcd_message_syntax_error(s, '.');
			return NULL;
		}

		vmcd_scope inner_scope = *s;
		inner_scope.package = imported;
		inner_scope.parent = s;
		return vmcd_parse_type(&inner_scope, FALSE);
	}

	// Try to fetch the type
	vmp_type* type;
	if (include_imports)
		type = vmp_package_find_type_include_imports(s->package, &t->string);
	else
		type = vmp_package_find_type(s->package, &t->string);
	if (type == NULL) {
		type = vmp_type_new(&t->string);
		switch (vmp_package_add_type(s->package, type))
		{
		case VMP_LIST_ADDED:
			break;
		default:
			// Could not add type. TODO: Add better message
			vmcd_message_not_implemented(s);
			return NULL;
		}
	}
	return type;
}

BOOL vmcd_parse_assign_local(const vmcd_scope* s, vmp_local* local)
{
	vmcd_token* const t = s->token;
	vmp_func* const f = s->func;
	vm_string type_name;
	vmp_type* type = NULL;
	vmcd_token_next(t);
	switch (t->type)
	{
	case VMCD_TOKEN_VALUE_INT:
		type_name.start = "int32";
		type_name.end = type_name.start + 5;
		type = vmp_package_find_type_include_imports(s->package, &type_name);
		vmp_func_add_instr(f, vmp_instr_ldc(type, vmp_const_i4(vmcd_token_i4(t))));
		vmp_func_add_instr(f, vmp_instr_stl(local->index));
		return TRUE;
	default:
		return vmcd_message_not_implemented(s);
	}
}

// Parse a constant value, such as 12345
vmp_instr* parse_value(const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	vm_string type_name;
	vmp_instr* instr;
	switch (t->type)
	{
	case VMCD_TOKEN_VALUE_INT:
	{
		const vm_uint64 value = vmcd_token_ui8(t);
		if (value <= INT32_MAX) {
			type_name.start = "int32";
			type_name.end = type_name.start + 5;
			vmp_type* type = vmp_package_find_type_include_imports(s->package, &type_name);
			ASSERT_NOT_NULL(type);
			return vmp_instr_ldc(type, vmp_const_i4(vmcd_token_i4(t)));
		}
		else if (value <= UINT32_MAX) {
			type_name.start = "uint32";
			type_name.end = type_name.start + 6;
			vmp_type* type = vmp_package_find_type_include_imports(s->package, &type_name);
			ASSERT_NOT_NULL(type);
			return vmp_instr_ldc(type, vmp_const_ui4(vmcd_token_ui4(t)));
		}
		else if (value <= INT64_MAX) {
			type_name.start = "int64";
			type_name.end = type_name.start + 5;
			vmp_type* type = vmp_package_find_type_include_imports(s->package, &type_name);
			ASSERT_NOT_NULL(type);
			return vmp_instr_ldc(type, vmp_const_i8(vmcd_token_i8(t)));
		}
		else if (value <= UINT64_MAX) {
			type_name.start = "uint64";
			type_name.end = type_name.start + 6;
			vmp_type* type = vmp_package_find_type_include_imports(s->package, &type_name);
			ASSERT_NOT_NULL(type);
			return vmp_instr_ldc(type, vmp_const_ui8(vmcd_token_ui8(t)));
		}
	}
	case VMCD_TOKEN_VALUE_DECIMAL:
	{
		const vm_float64 value = vmcd_token_f8(t);
		type_name.start = "float64";
		type_name.end = type_name.start + 7;
		vmp_type* type = vmp_package_find_type_include_imports(s->package, &type_name);
		ASSERT_NOT_NULL(type);
		return vmp_instr_ldc(type, vmp_const_f8(vmcd_token_f8(t)));
	}
	case VMCD_TOKEN_VALUE_BOOL:
	{
		type_name.start = "bool";
		type_name.end = type_name.start + 4;
		vmp_type* type = vmp_package_find_type_include_imports(s->package, &type_name);
		ASSERT_NOT_NULL(type);
		return vmp_instr_ldc(type, vmp_const_i4(vmcd_token_bool(t)));
	}
	default:
		vmcd_message_not_implemented(s);
		return NULL;
	}
}

// Statements: 
//	1 + 2 
//		ldc, ldc, add
//	(1 + 2) + 3 
//		ldc, ldc, add, ldc, add
//	1 + (2 + 3) 
//		ldc, ldc, ldc, add, add
// get() + constant + (10 * get())
//		call, ldc, add, ldc, call, mul, add
BOOL parse_statement(const vmcd_scope* s, vmp_instr** result)
{
	vmcd_token* const t = s->token;
	vmp_instr* instr = NULL;

	if (t->type == VMCD_TOKEN_PARAN_L) {
		// Is the statement segmented using parantheses?
		if (!parse_statement(s, &instr)) {
			return FALSE;
		}
	}
	else if (vmcd_token_is_value(t)) {
		// Parse value statement
		instr = parse_value(s);
		if (instr == NULL) {
			return FALSE;
		}
	}
	else if (t->type == VMCD_TOKEN_KEYWORD) {
		// constant or function?
		vmp_keyword* keyword = vmp_func_find_keyword(s->func, &t->string);
		if (keyword == NULL) {
			// keyword doesn't exist? Then the only allowed type is a "func", "global", "const" or "type"
		}
		else {
			switch (keyword->keyword_type)
			{
			case VMP_KEYWORD_ARG:
			{
				const vmp_arg* const arg = (vmp_arg*)keyword;
				instr = vmp_instr_lda(arg->index);
			}
			case VMP_KEYWORD_LOCAL:
			{
				const vmp_local* const local = (vmp_local*)keyword;
				instr = vmp_instr_ldl(local->index);
			}
			case VMP_KEYWORD_FUNC:
			case VMP_KEYWORD_PACKAGE:
			case VMP_KEYWORD_CONST:
			case VMP_KEYWORD_TYPE:
				// This means casting
			}
		}
	}
	else {
		return vmcd_message_not_implemented(s);
	}

	// should be a calculator 
	vmcd_token_next(t);
	if (t->type == VMCD_TOKEN_PLUS) {
		instr = vmp_instr_link(instr, vmp_instr_add());
	}


	return TRUE;
}

BOOL vmcd_parse_decl_assign_local(const vmcd_scope* s, vmp_local* local)
{
	vmcd_token* const t = s->token;
	vmp_func* const f = s->func;
	vm_string type_name;
	vmp_type* type = NULL;
	vmcd_token_next(t);
	switch (t->type)
	{
	case VMCD_TOKEN_VALUE_INT:
		type_name.start = "int32";
		type_name.end = type_name.start + 5;
		type = vmp_package_find_type_include_imports(s->package, &type_name);
		if (type == NULL) {
			return vmcd_message_type_not_found(s, &type_name);
		}
		local->type = type;
		vmp_func_add_instr(f, vmp_instr_ldc(type, vmp_const_i4(vmcd_token_i4(t))));
		vmp_func_add_instr(f, vmp_instr_stl(local->index));
		return TRUE;
	case VMCD_TOKEN_KEYWORD:
	{
		vmp_keyword* keyword = vmp_func_find_keyword(s->func, &t->string);
		if (keyword == NULL) {
			// ??? Might be a function or a constant that's not declared yet
			return vmcd_message_not_implemented(s);
		}
		if (keyword->keyword_type == VMP_KEYWORD_ARG) {
			vmp_arg* const arg = (vmp_arg*)keyword;
			vmp_func_add_instr(f, vmp_instr_lda(arg->index));
		}
		else {
			return vmcd_message_not_implemented(s);
		}
	}
	default:
		return vmcd_message_not_implemented(s);
	}
}

BOOL vmcd_parse_return(const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	if (s->func->returns.count > 0) {
		vmcd_token_next(t);

		// Each return is separated with a comma(,)
		const int num_returns = s->func->returns.count;
		for (int i = 0; i < num_returns; ++i) {
			vmp_return* const ret = vmp_list_returns_get(&s->func->returns, i);

			switch (t->type) {
			case VMCD_TOKEN_KEYWORD:
			{
				vmp_keyword* const keyword = vmp_func_find_keyword(s->func, &t->string);
				if (keyword == NULL) {
					return vmcd_message_expected_identifier(s);
				}

				switch (keyword->keyword_type) {
				case VMP_KEYWORD_LOCAL:
				{
					const vmp_local* const local = (vmp_local*)keyword;
					vmp_func_add_instr(s->func, vmp_instr_ldl(local->index));
					break;
				}
				default:
					return vmcd_message_not_implemented(s);
				}
				break;
			}
			default:
				return vmcd_message_not_implemented(s);
			};

			if (i < (num_returns - 1)) {
				if (!vmcd_token_next_type(t, VMCD_TOKEN_COMMA))
					return vmcd_message_syntax_error(s, ',');
				vmcd_token_next(t);
			}
		}
	}
	vmp_func_add_instr(s->func, vmp_instr_ret());
	return TRUE;
}

BOOL vmcd_parse_keyword(vm_string keyword_name, const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	vmp_keyword* const keyword = vmp_func_find_keyword(s->func, &keyword_name);

	vmcd_token_next(t);
	if (t->type == VMCD_TOKEN_PARAN_L) {
		vmp_func* func = (vmp_func*)keyword;
		// function call in the same package
		if (func == NULL) {
			// Create it and add it to the package
			func = vmp_func_new(&t->string);
			switch (vmp_package_add_func(s->package, func)) {
			case VMP_LIST_ADDED:
				break;
			default:
				// Could not add function. TODO: Add better message
				return vmcd_message_not_implemented(s);
			}
		}

		// Arguments are, indirectly, created using the arguments supplied to it!
		return vmcd_message_not_implemented(s);
	}
	else if (t->type == VMCD_TOKEN_DOT) {
		// Keyword in another package
		return vmcd_message_not_implemented(s);
	}
	else if (t->type == VMCD_TOKEN_ASSIGN) {
		if (keyword == NULL) {
			vmcd_message_variable_not_found(s, &keyword_name);
			return FALSE;
		}
		if (!vmp_keyword_is_assignable(keyword)) {
			vmcd_message_not_assignable(s, &keyword_name);
			return FALSE;
		}
		
		if (keyword->keyword_type == VMP_KEYWORD_LOCAL) {
			vmp_local* const local = (vmp_local*)keyword;
			return vmcd_parse_assign_local(s, local);
		}
		else {
			return vmcd_message_not_implemented(s);
		}
	}
	else if (t->type == VMCD_TOKEN_DECL_ASSIGN) {
		if (keyword != NULL) {
			vmcd_message_duplicated_declaration(s, &keyword_name);
		}
		else {
			// The type is not known yet
			vmp_local* local = vmp_local_new();
			if (vmp_func_add_local(s->func, local) == FALSE) {
				vmcd_message_out_of_memory(s);
				return FALSE;
			}
			vmp_local_set_name(local, &keyword_name);
			return vmcd_parse_decl_assign_local(s, local);
		}
	}
	return TRUE;
}

// Functions have the syntax: "func Name(<args>) (<returns>)
BOOL vmcd_parse_func(const vmcd_scope* s)
{
	vmcd_token* const t = s->token;
	if (!vmcd_token_next_type(t, VMCD_TOKEN_KEYWORD))
		return vmcd_message_expected_identifier(s);

	// This function might've been declared indirectly through a function call
	vmp_func* func = vmp_package_find_func(s->package, &t->string);
	if (func == NULL) {
		func = vmp_func_new(&t->string);
		switch (vmp_package_add_func(s->package, func)) {
		case VMP_LIST_ADDED:
			break;
		default:
			// Could not add function. TODO: Add better message
			return vmcd_message_not_implemented(s);
		}
		vmp_func_add_flag(func, VMP_FUNC_FLAGS_PUBLIC);
	}
	if (func->body_size > 0) {
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
		vmp_arg* arg = vmp_arg_new();
		arg->name = t->string;
		vmp_func_add_arg(func, arg);

		vmp_type* type = vmcd_parse_type(s, TRUE);
		if (type == NULL) {
			return vmcd_message_not_implemented(s);
		}
		arg->type = type;
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

			// Identity first
			vmp_return* ret = vmp_return_new();
			vmp_func_add_return(func, ret);

			vmp_type* type = vmcd_parse_type(s, TRUE);
			if (type == NULL) {
				return vmcd_message_not_implemented(s);
			}
			ret->type = type;
		}
	}
	else if (t->type == VMCD_TOKEN_KEYWORD) {
		// One return value
		// Identity first
		vmp_return* ret = vmp_return_new();
		if (!vmp_func_add_return(func, ret)) {
			return vmcd_message_out_of_memory(s);
		}
		vmp_type* type = vmcd_parse_type(s, TRUE);
		if (type == NULL) {
			return vmcd_message_not_implemented(s);
		}
		ret->type = type;
		vmcd_token_next(t);
	}

	if (t->type != VMCD_TOKEN_BRACKET_L) {
		// Expected function body
		return vmcd_message_syntax_error(s, '{');
	}
	vmp_func_begin_body(func);
	vmcd_scope inner_scope = *s;
	inner_scope.func = func;
	inner_scope.parent = s;

	int depth = 1;
	vmcd_token_next(t);
	// Parse until we reach that end ')' token
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

		switch (t->type)
		{
		case VMCD_TOKEN_KEYWORD:
			// This will is probably a function name or a variable name
			if (!vmcd_parse_keyword(t->string, &inner_scope)) {
				return FALSE;
			}
			break;
		case VMCD_TOKEN_KEYWORD_IF:
		case VMCD_TOKEN_KEYWORD_FOR:
		case VMCD_TOKEN_KEYWORD_VAR:
		case VMCD_TOKEN_KEYWORD_FUNC:
		case VMCD_TOKEN_KEYWORD_TYPE:
		case VMCD_TOKEN_KEYWORD_ELSE:
		case VMCD_TOKEN_KEYWORD_CONST:
		case VMCD_TOKEN_KEYWORD_RETURN:
			if (!vmcd_parse_return(&inner_scope)) {
				return FALSE;
			}
			break;
		case VMCD_TOKEN_KEYWORD_STRUCT:
		case VMCD_TOKEN_KEYWORD_IMPORT:
		case VMCD_TOKEN_KEYWORD_EXTERN:
		case VMCD_TOKEN_KEYWORD_PACKAGE:
		case VMCD_TOKEN_KEYWORD_INTERFACE:
		default:
			break;
		}

		vmcd_token_next(t);
	}

	vmp_func_begin_end(func);
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
	vmp_package* package = vmp_pipeline_find_package(s->pipeline, &s->token->string);
	if (package == NULL) {
		package = vmp_package_new(&s->token->string);
		if (package == NULL)
			return vmcd_message_out_of_memory(s);
		vmp_pipeline_add_package(s->pipeline, package);
	}

	// A package supports the following keywords (for now):
	// * func
	// * import
	// * type
	// * const
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
			
			vmp_package* imported = vmp_pipeline_find_package(s->pipeline, &s->token->string);
			if (imported == NULL) {
				return vmcd_message_package_not_found(s);
			}
			switch (vmp_package_add_import(package, imported)) {
			case VMP_LIST_ADDED:
				break;
			default:
				// Could not import package. TODO: Add better message
				return vmcd_message_package_not_found(s);
			}
		}
		else if (t->type == VMCD_TOKEN_KEYWORD_CONST) {

		}
		else if (t->type == VMCD_TOKEN_KEYWORD_TYPE) {

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
	free(p);
}

BOOL vmcode_parse(vmcode* p, const vm_byte* source_code)
{
	vmcd_lexer l;
	vmcd_token t;
	vmcd_lexer_init(&l, source_code);
	vmcd_token_init(&t, &l);
	if (!vmcd_begin(p, &t)) {
		return FALSE;
	}

	// Resolve memory sizes and offsets for function, argument, returns, locals etc
	if (!vmp_pipeline_resolve(p->pipeline)) {
		vm_messages_move(&p->pipeline->messages, &p->messages);
		return FALSE;
	}

	vmcd_token_release(&t);
	return TRUE;
}

BOOL vmcode_link(vmcode* p)
{
	// Generate the actual bytecode
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
