#include "compiler.h"
#include "../vm_debug.h"

BOOL zpp_compiler_create_system_package(zpp_compiler* c)
{
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

	// Create vm package
	vm_string name;
	name.start = "System";
	name.end = name.start + 6;

	c->packages = c->packages_end = zpp_package_new(&name);
	if (c->packages == NULL)
		return FALSE;

	zpp_type* type;

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(bool), sizeof(vm_int32), 0, VMI_INSTR_PROP_BOOL, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(pbool), sizeof(BOOL*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(int8), sizeof(vm_int8), 0, VMI_INSTR_PROP_INT8, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(pint8), sizeof(vm_int8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(uint8), sizeof(vm_uint8), 0, VMI_INSTR_PROP_UINT8, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(puint8), sizeof(vm_uint8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(int16), sizeof(vm_int16), 0, VMI_INSTR_PROP_INT16, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(pint16), sizeof(vm_int16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(uint16), sizeof(vm_uint16), 0, VMI_INSTR_PROP_UINT16, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(puint16), sizeof(vm_uint16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(int32), sizeof(vm_int32), 0, VMI_INSTR_PROP_INT32, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(pint32), sizeof(vm_int32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(uint32), sizeof(vm_uint32), 0, VMI_INSTR_PROP_UINT32, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(puint32), sizeof(vm_uint32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(int64), sizeof(vm_int64), 0, VMI_INSTR_PROP_INT64, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(pint64), sizeof(vm_int64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(uint64), sizeof(vm_uint64), 0, VMI_INSTR_PROP_UINT64, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(puint64), sizeof(vm_uint64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(float32), sizeof(vm_float32), 0, VMI_INSTR_PROP_FLOAT32, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(pfloat32), sizeof(vm_float32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(float64), sizeof(vm_float64), 0, VMI_INSTR_PROP_FLOAT64, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(VM_STRING_CONST_GET(pfloat64), sizeof(vm_float64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	return TRUE;
}

BOOL zpp_compiler_init(zpp_compiler* c)
{
	// Create system package
	if (!zpp_compiler_create_system_package(c))
		return FALSE;

	// Prepare a root node and associate types so that they are global
	c->root_node = zpp_syntax_tree_root_new();
	
	// Register types
	zpp_type* type = c->packages->types;
	while (type) {
		zpp_syntax_tree_type* const st = zpp_syntax_tree_type_new(type);
		if (st == NULL) {
			// TODO: Out of memory
			return FALSE;
		}
		zpp_syntax_tree_root_add(c->root_node, ZPP_SYMBOL(st));
		type = type->tail;
	}
	return TRUE;
}


zpp_compiler* zpp_compiler_new()
{
	zpp_compiler* const p = vm_safe_malloc(zpp_compiler);
	vm_messages_init(&p->messages);
	vm_message_init(&p->panic);
	p->sources = p->sources_end = NULL;
	p->root_node = NULL;
	p->packages = p->packages_end = NULL;
	if (!zpp_compiler_init(p)) {
		zpp_compiler_destroy(p);
		return NULL;
	}
	p->pipeline = NULL;
	p->builder = NULL;
	return p;
}

void zpp_compiler_destroy(zpp_compiler* ptr)
{
	ASSERT_NOT_NULL(ptr);

	if (ptr->builder)
		vmp_builder_destroy(ptr->builder);
	if (ptr->pipeline)
		vmp_pipeline_destroy(ptr->pipeline);

	if (ptr->root_node)
		zpp_syntax_tree_destroy(ZPP_SYMBOL(ptr->root_node));

	// Destroy all packages
	zpp_package* package = ptr->packages;
	while (package) {
		zpp_package* const tail = package->tail;
		zpp_package_destroy(package);
		package = tail;
	}

	// Destroy all source codes
	zpp_source_code* source = ptr->sources;
	while (source) {
		zpp_source_code* const tail = source->tail;
		zpp_source_code_destroy(source);
		source = tail;
	}

	vm_messages_destroy(&ptr->messages);
	vm_free(ptr);
}

zpp_package* zpp_compiler_find_package(zpp_compiler* c, const vm_string* name, const zpp_compiler_state* state)
{
	zpp_package* package = c->packages;
	while (package) {
		if (zpp_symbol_has_name(ZPP_SYMBOL(package), name)) {
			return package;
		}
		package = package->tail;
	}
	return NULL;
}

zpp_type* zpp_compiler_parse_type(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	// Is this a pointer (*) type?
	if (t->type == ZPP_TOKEN_PTR) {
		// If no type was found then this is from a package and it wasn't found. This can be deduced because
		// a type from "this" package is always temporarily created with an unknown size
		zpp_token_next(t);
		zpp_type* base_type = zpp_compiler_parse_type(c, t, state);
		if (base_type == NULL)
			return NULL;

	}
	else if (t->type == ZPP_TOKEN_SQUARE_L) {
		// This is an array type
		zpp_message_not_implemented(state);
		return NULL;
	}
	else if (t->type != ZPP_TOKEN_KEYWORD) {
		zpp_message_expected_identifier(state);
		return NULL;
	}

	// You might specify a full path to the type: "System.Int32Package1.Package2.Type.InnerType".
	// The first type is considered the current class first.
	//
	// It's expected that you've "imported" at the top of the source code
	zpp_symbol* symbol = zpp_syntax_tree_find_symbol_include_imports(state->parent_node, &t->string);
	if (symbol != NULL) {
		if (symbol->type == ZPP_SYMBOL_PACKAGE) {
			// You've specified a package. It's expected that your supplying a full type name in this case
			zpp_token_next(t);
			while (zpp_token_next(t) == ZPP_TOKEN_DOT && symbol->type == ZPP_SYMBOL_PACKAGE) {
				symbol = zpp_package_find_symbol((zpp_package*)symbol, &t->string);
				if (symbol == NULL) {
					// TODO: Create a type
					zpp_message_not_implemented(state);
					return NULL;
				}
			}
		}
	}

	if (symbol != NULL && symbol->type == ZPP_SYMBOL_TYPE) {
		return (zpp_type*)symbol;
	}

	// Try to fetch the type. If the type doesn't exist then assume that this isn't from a package and can thus be
	// created artificially with an unknown size
	zpp_type* type = zpp_type_new(&t->string);
	if(symbol == NULL) {
		zpp_package_add_type(state->package_node->package, type);
	}
	else if (symbol->type == ZPP_SYMBOL_PACKAGE) {
		zpp_package_add_type((zpp_package*)symbol, type);
	}
	else {
		// The type might be an inner type
		zpp_message_not_implemented(state);
		return NULL;
	}
	
	// Parse the type and create an "unresolved" type if not encountered yet
	return type;
}


BOOL zpp_compiler_parse_body(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	zpp_syntax_tree_node node;

	while (t->type != ZPP_TOKEN_BRACKET_R) {
		if (t->type == ZPP_TOKEN_BRACKET_L) {
			const zpp_compiler_state child_scope = {
				state->compiler,
				state->token,
				node,
				state->package_node,
				state->func_node,
				state->type_node
			};
			zpp_token_next(t);
			if (!zpp_compiler_parse_body(c, t, &child_scope))
				return FALSE;
			continue;
		}
		else if (t->type == ZPP_TOKEN_BRACKET_R) {
			continue;
		}
		else if (t->type == ZPP_TOKEN_NEWLINE) {
			zpp_token_next(t);
			continue;
		}
		else {
			
		}		
		node = zpp_synax_tree_parse_expression(c, t, state);
		if (node == NULL)
			return FALSE;
		zpp_syntax_tree_add(state->parent_node, node);
	}

	return TRUE;
}

BOOL zpp_compiler_parse_func(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	// Expected function name
	if (t->type != ZPP_TOKEN_KEYWORD) {
		return zpp_message_expected_identifier(state);
	}

	//
	zpp_func* func = zpp_package_find_func(state->package_node->package, &t->string);
	if (func == NULL) {
		func = zpp_func_new(&t->string);
		if (func == NULL) 
			return zpp_message_out_of_memory(state);

		// Parse signature

		// Add function 
		if (!zpp_package_add_func(state->package_node->package, func))
			return zpp_message_out_of_memory(state);
	}

	if (func->syntax_tree != NULL) {
		// Body declared twice? This might be a function with another signature
		return zpp_message_not_implemented(state);
	}

	// Create a new node and add it to the package
	zpp_syntax_tree_func* const node = zpp_syntax_tree_func_new(func);
	if (node == NULL)
		return zpp_message_out_of_memory(state);
	zpp_syntax_tree_package_add_func(state->package_node, node);

	// Expected a '(' rune
	if (zpp_token_next(t) != ZPP_TOKEN_PARAN_L) {
		return zpp_message_syntax_error(state, '(');
	}

	// Parse argument information until we reach that end ')' token
	while (zpp_token_next(t) != ZPP_TOKEN_PARAN_R) {
		// Ignore comma
		if (t->type == ZPP_TOKEN_COMMA) {
			if (zpp_token_next(t) != ZPP_TOKEN_KEYWORD)
				return zpp_message_expected_identifier(state);
		}

		// Identity first
		zpp_argument* arg = zpp_argument_new(&t->string);
		zpp_func_add_argument(func, arg);

		zpp_type* const type = zpp_compiler_parse_type(c, t, state);
		if (type == NULL) {
			return FALSE;
		}
		arg->type = ZPP_SYMBOL(type);
	}
	zpp_token_next(t);

	// Return values present?
	if (t->type == ZPP_TOKEN_KEYWORD) {
		// One return type
		zpp_return* ret = zpp_return_new();
		zpp_func_add_return(func, ret);

		zpp_type* const type = zpp_compiler_parse_type(c, t, state);
		if (type == NULL) {
			return FALSE;
		}
		ret->type = ZPP_SYMBOL(type);
		zpp_token_next(t);
	}
	else if (t->type == ZPP_TOKEN_PARAN_L) {
		// Parse return information until we reach that end ')' token
		while (zpp_token_next(t) != ZPP_TOKEN_PARAN_R) {
			// Ignore comma
			if (t->type == ZPP_TOKEN_COMMA) {
				if (zpp_token_next(t) != ZPP_TOKEN_KEYWORD)
					return zpp_message_expected_identifier(state);
			}

			zpp_return* ret = zpp_return_new();
			zpp_func_add_return(func, ret);

			zpp_type* const type = zpp_compiler_parse_type(c, t, state);
			if (type == NULL) {
				return FALSE;
			}
			ret->type = ZPP_SYMBOL(type);
		}
	}

	// Expected body for now. Perhaps re-use this function to parse a function pointer definition in the future
	if (t->type != ZPP_TOKEN_BRACKET_L) {
		return zpp_message_syntax_error(state, '{');
	}
	zpp_token_next(t);

	// Parse the body
	const zpp_compiler_state child_scope = {
			state->compiler,
			state->token,
			ZPP_SYNTAX_TREE(node),
			state->package_node,
			node,
			state->type_node
	};
	return zpp_compiler_parse_body(c, t, &child_scope);
}

BOOL zpp_compiler_parse_package(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	// Expected package name
	if (t->type != ZPP_TOKEN_KEYWORD) {
		return zpp_message_expected_identifier(state);
	}

	// Search for a package of a specific name and parent
	zpp_package* package = zpp_compiler_find_package(c, &t->string, state);
	if (package == NULL) {
		// Create a package representation
		package = zpp_package_new(&t->string);
		if (package == NULL) 
			return zpp_message_out_of_memory(state);

		if (state->package_node)
			package->parent = state->package_node->package;
		
		// Add the package
		if (c->packages == NULL) {
			c->packages = c->packages_end = package;
		}
		else {
			c->packages_end->tail = package;
			package->head = c->packages_end;
			c->packages_end = package;
		}
	}
	zpp_token_next(t);

	zpp_syntax_tree_package* const node = zpp_syntax_tree_package_new(package);
	if (node == NULL)
		return zpp_message_out_of_memory(state);

	// This is a recursive package name. Continue until we don't have any more package name keywords
	if (t->type == ZPP_TOKEN_DOT) {
		const zpp_compiler_state child_scope = {
			state->compiler,
			state->token,
			ZPP_SYNTAX_TREE(node),
			node,
			state->func_node,
			state->type_node
		};
		zpp_token_next(t);
		return zpp_compiler_parse_package(c, t, &child_scope);
	}
	
	if (t->type != ZPP_TOKEN_NEWLINE) {
		// zpp_message_expected_newline(c);
		return zpp_message_not_implemented(state);
	}

	// Add the package to the root node
	zpp_syntax_tree_root_add(c->root_node, ZPP_SYNTAX_TREE(node));

	// Parse package content
	while (1)
	{
		switch (t->type)
		{
		case ZPP_TOKEN_KEYWORD_FUNC:
		{
			// Parse the function
			const zpp_compiler_state child_scope = {
				state->compiler,
				state->token,
				ZPP_SYNTAX_TREE(node),
				node,
				state->func_node,
				state->type_node
			};
			zpp_token_next(t);
			if (!zpp_compiler_parse_func(c, t, &child_scope)) {
				return FALSE;
			}
			break;
		}
		case ZPP_TOKEN_NEWLINE:
		{
			zpp_token_next(t);
			break;
		}
		default:
			return zpp_message_not_implemented(state);
		}
	}
}

BOOL zpp_compiler_parse_source_code(zpp_compiler* c, zpp_token* t)
{
	// Parse all tokens and build a syntax tree from it
	while (1) {
		if (t->type == ZPP_TOKEN_KEYWORD_PACKAGE) {
			const zpp_compiler_state state = { c, t, NULL, NULL, NULL, NULL };
			zpp_token_next(t);
			if (!zpp_compiler_parse_package(c, t, &state)) {
				return FALSE;
			}
		}
		else if (t->type == ZPP_TOKEN_NEWLINE) {
			zpp_token_next(t);
		}
		else if (t->type == ZPP_TOKEN_EOF) {
			break;
		}
	}
	return TRUE;
}

BOOL zpp_compiler_add_source_code(zpp_compiler* c, zpp_source_code* source)
{
	if (c->sources == NULL)
		c->sources = c->sources_end = source;
	else {
		c->sources_end->tail = source;
		source->head = c->sources_end;
		c->sources_end = source;
	}
	source->compiler = c;

	// Process syntax tree
	zpp_lexer lexer;
	zpp_token token;
	zpp_lexer_init(&lexer, source->source.start);
	zpp_token_init(&token, &lexer);
	return zpp_compiler_parse_source_code(c, &token);
}

void zpp_compiler_raise_message(zpp_compiler* c, vm_message* msg)
{
	vm_messages_append(&c->messages, msg);
}

vm_byte* zpp_compiler_compile(zpp_compiler* c)
{
	c->pipeline = vmp_pipeline_new();

	// Solve all pending packages, types, functions etc
	zpp_package* package = c->packages;
	while (package) {
		vmp_package* resolved = zpp_package_resolve_package(package, c);
		if (!vmp_pipeline_add_package(c->pipeline, resolved)) {
			return NULL;
		}
		package = package->tail;
	}

	// Compile the syntax tree
	if (!zpp_synax_tree_compile(c, ZPP_SYNTAX_TREE(c->root_node)))
		return NULL;


	if (!vmp_pipeline_resolve(c->pipeline)) {
		return NULL;
	}

	// Let the actual builder compile the bytecode
	c->builder = vmp_builder_new(c->pipeline);
	vmp_builder_set_opt_level(c->builder, 0);
	if (!vmp_builder_compile(c->builder)) {
		vm_messages_move(&c->builder->messages, &c->messages);
		return FALSE;
	}

	return vmp_builder_get_bytecode(c->builder);
}
