#include "compiler.h"
#include "optimizations.h"
#include "../vm_debug.h"

CONST_VM_STRING(zpp, bool, "bool", 4)
CONST_VM_STRING(zpp, pbool, "*bool", 5)
CONST_VM_STRING(zpp, int8, "int8", 4)
CONST_VM_STRING(zpp, pint8, "*int8", 5)
CONST_VM_STRING(zpp, uint8, "uint8", 5)
CONST_VM_STRING(zpp, puint8, "*uint8", 6)
CONST_VM_STRING(zpp, int16, "int16", 5)
CONST_VM_STRING(zpp, pint16, "*int16", 6)
CONST_VM_STRING(zpp, uint16, "uint16", 6)
CONST_VM_STRING(zpp, puint16, "*uint16", 7)
CONST_VM_STRING(zpp, int32, "int32", 5)
CONST_VM_STRING(zpp, pint32, "*int32", 6)
CONST_VM_STRING(zpp, uint32, "uint32", 6)
CONST_VM_STRING(zpp, puint32, "*uint32", 7)
CONST_VM_STRING(zpp, int64, "int64", 5)
CONST_VM_STRING(zpp, pint64, "*int64", 6)
CONST_VM_STRING(zpp, uint64, "uint64", 6)
CONST_VM_STRING(zpp, puint64, "*uint64", 7)
CONST_VM_STRING(zpp, float32, "float32", 7)
CONST_VM_STRING(zpp, pfloat32, "*float32", 8)
CONST_VM_STRING(zpp, float64, "float64", 7)
CONST_VM_STRING(zpp, pfloat64, "*float64", 8)
CONST_VM_STRING(zpp, System, "System", 6)

BOOL zpp_compiler_create_system_package(zpp_compiler* c)
{
	c->packages = c->packages_end = zpp_package_new(GET_CONST_VM_STRING(zpp, System));
	if (c->packages == NULL)
		return FALSE;

	zpp_type* type;

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, bool), sizeof(vm_int32), 0, VMI_INSTR_PROP_BOOL, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, pbool), sizeof(BOOL*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, int8), sizeof(vm_int8), 0, VMI_INSTR_PROP_I8, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, pint8), sizeof(vm_int8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, uint8), sizeof(vm_uint8), 0, VMI_INSTR_PROP_UI8, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, puint8), sizeof(vm_uint8*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, int16), sizeof(vm_int16), 0, VMI_INSTR_PROP_I16, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, pint16), sizeof(vm_int16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, uint16), sizeof(vm_uint16), 0, VMI_INSTR_PROP_UI16, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, puint16), sizeof(vm_uint16*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, int32), sizeof(vm_int32), 0, VMI_INSTR_PROP_I32, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, pint32), sizeof(vm_int32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, uint32), sizeof(vm_uint32), 0, VMI_INSTR_PROP_UI32, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, puint32), sizeof(vm_uint32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, int64), sizeof(vm_int64), 0, VMI_INSTR_PROP_I64, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, pint64), sizeof(vm_int64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, uint64), sizeof(vm_uint64), 0, VMI_INSTR_PROP_UI64, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, puint64), sizeof(vm_uint64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, float32), sizeof(vm_float32), 0, VMI_INSTR_PROP_F32, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, pfloat32), sizeof(vm_float32*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
	zpp_package_add_type(c->packages, type);

	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, float64), sizeof(vm_float64), 0, VMI_INSTR_PROP_F64, NULL));
	zpp_package_add_type(c->packages, type);
	type = zpp_type_new_from_props(zpp_type_props_get(GET_CONST_VM_STRING(zpp, pfloat64), sizeof(vm_float64*), VMP_TYPE_FLAGS_PTR, VMI_INSTR_PROP_PTR, type));
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
	else if (t->type != ZPP_TOKEN_IDENTITY) {
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
	zpp_syntax_tree_node node = ZPP_SYNTAX_TREE(state->package_node);

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
		node = zpp_synax_tree_parse_keywords(c, t, state);
		if (zpp_syntax_tree_is_error(node))
			return FALSE;
		if (node != NULL)
			zpp_syntax_tree_add(state->parent_node, node);
	}

	return TRUE;
}

BOOL zpp_compiler_parse_func(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	// Expected function name
	if (t->type != ZPP_TOKEN_IDENTITY) {
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
		return zpp_message_syntax_error(state, "expected: (");
	}

	// Parse argument information until we reach that end ')' token
	while (zpp_token_next(t) != ZPP_TOKEN_PARAN_R) {
		// Ignore comma
		if (t->type == ZPP_TOKEN_COMMA) {
			if (zpp_token_next(t) != ZPP_TOKEN_IDENTITY)
				return zpp_message_expected_identifier(state);
		}

		// Identity first
		zpp_argument* arg = zpp_argument_new(&t->string);
		zpp_func_add_argument(func, arg);

		zpp_token_next(t);
		zpp_type* const type = zpp_compiler_parse_type(c, t, state);
		if (type == NULL) {
			return FALSE;
		}
		arg->type = ZPP_SYMBOL(type);
	}
	zpp_token_next(t);

	// Return values present?
	if (t->type == ZPP_TOKEN_IDENTITY) {
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
				if (zpp_token_next(t) != ZPP_TOKEN_IDENTITY)
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
		// Skip '('
		zpp_token_next(t);
	}

	// Expected body for now. Perhaps re-use this function to parse a function pointer definition in the future
	if (t->type != ZPP_TOKEN_BRACKET_L) {
		return zpp_message_syntax_error(state, "expected {");
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
	if (!zpp_compiler_parse_body(c, t, &child_scope))
		return FALSE;
	zpp_token_next(t);
	return TRUE;
}

BOOL zpp_compiler_parse_package(zpp_compiler* c, zpp_token* t, const zpp_compiler_state* state)
{
	// Expected package name
	if (t->type != ZPP_TOKEN_IDENTITY) {
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
			continue;
		}
		case ZPP_TOKEN_NEWLINE:
		{
			zpp_token_next(t);
			continue;
		}
		case ZPP_TOKEN_EOF:
			return TRUE;
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
		else if (t->type == ZPP_TOKEN_EOF) {
			break;
		}
		else if (t->type == ZPP_TOKEN_NEWLINE) {
			zpp_token_next(t);
		}
		else {
			zpp_message_expected_package(c, t);
			return FALSE;
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

	// Optimize the syntax tree before compiling it into bytecode
	const zpp_compiler_state state = {
		c, NULL, NULL, NULL, NULL, NULL
	};
	if (!zpp_synax_tree_optimize(&state, c->root_node))
		return NULL;

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
