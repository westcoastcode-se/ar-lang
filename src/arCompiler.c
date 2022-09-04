#include "arCompiler.h"
#include "arMemory.h"
#include "compiler/optimizations.h"
#include "compiler/tokens.h"
#include "compiler/messages.h"

CONST_VM_STRING(arCompiler, bool, "bool", 4)
CONST_VM_STRING(arCompiler, pbool, "*bool", 5)
CONST_VM_STRING(arCompiler, int8, "int8", 4)
CONST_VM_STRING(arCompiler, pint8, "*int8", 5)
CONST_VM_STRING(arCompiler, uint8, "uint8", 5)
CONST_VM_STRING(arCompiler, puint8, "*uint8", 6)
CONST_VM_STRING(arCompiler, int16, "int16", 5)
CONST_VM_STRING(arCompiler, pint16, "*int16", 6)
CONST_VM_STRING(arCompiler, uint16, "uint16", 6)
CONST_VM_STRING(arCompiler, puint16, "*uint16", 7)
CONST_VM_STRING(arCompiler, int32, "int32", 5)
CONST_VM_STRING(arCompiler, pint32, "*int32", 6)
CONST_VM_STRING(arCompiler, uint32, "uint32", 6)
CONST_VM_STRING(arCompiler, puint32, "*uint32", 7)
CONST_VM_STRING(arCompiler, int64, "int64", 5)
CONST_VM_STRING(arCompiler, pint64, "*int64", 6)
CONST_VM_STRING(arCompiler, uint64, "uint64", 6)
CONST_VM_STRING(arCompiler, puint64, "*uint64", 7)
CONST_VM_STRING(arCompiler, float32, "float32", 7)
CONST_VM_STRING(arCompiler, pfloat32, "*float32", 8)
CONST_VM_STRING(arCompiler, float64, "float64", 7)
CONST_VM_STRING(arCompiler, pfloat64, "*float64", 8)
CONST_VM_STRING(arCompiler, System, "System", 6)

BOOL arCompiler_create_system_package(arCompiler* c)
{
	c->packages = c->packages_end = arC_package_new(GET_CONST_VM_STRING(arCompiler, System));
	if (c->packages == NULL)
		return FALSE;

	arC_type* type;

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, bool), sizeof(arInt32), 0, ARLANG_PRIMITIVE_BOOL, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, pbool), sizeof(BOOL*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, int8), sizeof(arInt8), 0, ARLANG_PRIMITIVE_I8, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, pint8), sizeof(arInt8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, uint8), sizeof(arUint8), 0, ARLANG_PRIMITIVE_UI8, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, puint8), sizeof(arUint8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, int16), sizeof(arInt16), 0, ARLANG_PRIMITIVE_I16, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, pint16), sizeof(arInt16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, uint16), sizeof(arUint16), 0, ARLANG_PRIMITIVE_UI16, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, puint16), sizeof(arUint16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, int32), sizeof(arInt32), 0, ARLANG_PRIMITIVE_I32, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, pint32), sizeof(arInt32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, uint32), sizeof(arUint32), 0, ARLANG_PRIMITIVE_UI32, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, puint32), sizeof(arUint32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, int64), sizeof(arInt64), 0, ARLANG_PRIMITIVE_I64, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, pint64), sizeof(arInt64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, uint64), sizeof(arUint64), 0, ARLANG_PRIMITIVE_UI64, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, puint64), sizeof(arUint64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, float32), sizeof(arFloat32), 0, ARLANG_PRIMITIVE_F32, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, pfloat32), sizeof(arFloat32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, float64), sizeof(arFloat64), 0, ARLANG_PRIMITIVE_F64, NULL));
	arC_package_add_type(c->packages, type);
	type = arC_type_from_props(arC_type_props_get(GET_CONST_VM_STRING(arCompiler, pfloat64), sizeof(arFloat64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
	arC_package_add_type(c->packages, type);

	return TRUE;
}

BOOL arCompiler_init(arCompiler* c)
{
	// Create system package
	if (!arCompiler_create_system_package(c))
		return FALSE;

	// Prepare a root node and associate types so that they are global
	c->root_node = arC_syntax_tree_root_new();
	
	// Register types
	arC_type* type = c->packages->types;
	while (type) {
		arC_syntax_tree_node_type* const st = arC_syntax_tree_node_type_new(type);
		if (st == NULL) {
			// TODO: Out of memory
			return FALSE;
		}
		arC_syntax_tree_root_add(c->root_node, asC_symbol(st));
		type = type->tail;
	}
	return TRUE;
}


arCompiler* arCompiler_new()
{
	arCompiler* const p = arSafeMalloc(arCompiler);
	arMessages_init(&p->messages);
	arMessage_init(&p->panic);
	p->sources = p->sources_end = NULL;
	p->root_node = NULL;
	p->packages = p->packages_end = NULL;
	if (!arCompiler_init(p)) {
		arCompiler_destroy(p);
		return NULL;
	}
	p->pipeline = NULL;
	return p;
}

void arCompiler_destroy(arCompiler* ptr)
{
	ASSERT_NOT_NULL(ptr);

	if (ptr->pipeline)
		arBuilder_destroy(ptr->pipeline);

	if (ptr->root_node)
		arC_syntax_tree_node_type_destroy(asC_symbol(ptr->root_node));

	// Destroy all packages
	arC_package* package = ptr->packages;
	while (package) {
		arC_package* const tail = package->tail;
		arC_package_destroy(package);
		package = tail;
	}

	// Destroy all source codes
	arC_source_code* source = ptr->sources;
	while (source) {
		arC_source_code* const tail = source->tail;
		arC_source_code_destroy(source);
		source = tail;
	}

	arMessages_destroy(&ptr->messages);
	arFree(ptr);
}

arC_package* arCompiler_find_package(arCompiler* c, const arString* name, const arC_state* state)
{
	arC_package* package = c->packages;
	while (package) {
		if (arC_symbol_has_name(asC_symbol(package), name)) {
			return package;
		}
		package = package->tail;
	}
	return NULL;
}

arC_type* arCompiler_parse_type(arCompiler* c, arC_token* t, const arC_state* state)
{
	// Is this a pointer (*) type?
	if (t->type == ARTOK_PTR) {
		// If no type was found then this is from a package and it wasn't found. This can be deduced because
		// a type from "this" package is always temporarily created with an unknown size
		arC_token_next(t);
		arC_type* base_type = arCompiler_parse_type(c, t, state);
		if (base_type == NULL)
			return NULL;

	}
	else if (t->type == ARTOK_SQUARE_L) {
		// This is an array type
		arC_message_not_implemented(state);
		return NULL;
	}
	else if (t->type != ARTOK_IDENTITY) {
		arC_message_expected_identifier(state);
		return NULL;
	}

	// You might specify a full path to the type: "System.Int32Package1.Package2.Type.InnerType".
	// The first type is considered the current class first.
	//
	// It's expected that you've "imported" at the top of the source code
	arC_symbol* symbol = arC_syntax_tree_find_symbol_incl_imports(state->parent_node, &t->string);
	if (symbol != NULL) {
		if (symbol->type == arC_SYMBOL_PACKAGE) {
			// You've specified a package. It's expected that your supplying a full type name in this case
			arC_token_next(t);
			while (arC_token_next(t) == ARTOK_DOT && symbol->type == arC_SYMBOL_PACKAGE) {
				symbol = arC_package_find_symbol((arC_package*)symbol, &t->string);
				if (symbol == NULL) {
					// TODO: Create a type
					arC_message_not_implemented(state);
					return NULL;
				}
			}
		}
	}

	if (symbol != NULL && symbol->type == arC_SYMBOL_TYPE) {
		return (arC_type*)symbol;
	}

	// Try to fetch the type. If the type doesn't exist then assume that this isn't from a package and can thus be
	// created artificially with an unknown size
	arC_type* type = arC_type_new(&t->string);
	if(symbol == NULL) {
		arC_package_add_type(state->package_node->package, type);
	}
	else if (symbol->type == arC_SYMBOL_PACKAGE) {
		arC_package_add_type((arC_package*)symbol, type);
	}
	else {
		// The type might be an inner type
		arC_message_not_implemented(state);
		return NULL;
	}
	
	// Parse the type and create an "unresolved" type if not encountered yet
	return type;
}


BOOL arCompiler_parse_body(arCompiler* c, arC_token* t, const arC_state* state)
{
	arC_syntax_tree_node node = asC_syntax_tree(state->package_node);

	while (t->type != ARTOK_BRACKET_R) {
		if (t->type == ARTOK_BRACKET_L) {
			const arC_state child_scope = {
				state->compiler,
				state->token,
				node,
				state->package_node,
				state->func_node,
				state->type_node
			};
			arC_token_next(t);
			if (!arCompiler_parse_body(c, t, &child_scope))
				return FALSE;
			continue;
		}
		else if (t->type == ARTOK_BRACKET_R) {
			continue;
		}
		else if (t->type == ARTOK_NEWLINE) {
			arC_token_next(t);
			continue;
		}
		else {
			
		}		
		node = arC_syntax_tree_parse_keywords(c, t, state);
		if (arC_syntax_tree_is_error(node))
			return FALSE;
		if (node != NULL)
			arC_syntax_tree_add(state->parent_node, node);
	}

	return TRUE;
}

BOOL arCompiler_parse_func(arCompiler* c, arC_token* t, const arC_state* state)
{
	// Expected function name
	if (t->type != ARTOK_IDENTITY) {
		return arC_message_expected_identifier(state);
	}

	//
	arC_func* func = arC_package_find_func(state->package_node->package, &t->string);
	if (func == NULL) {
		func = arC_func_new(&t->string);
		if (func == NULL) 
			return arC_message_out_of_memory(state);

		// Parse signature

		// Add function 
		if (!arC_package_add_func(state->package_node->package, func))
			return arC_message_out_of_memory(state);
	}

	if (func->syntax_tree != NULL) {
		// Body declared twice? This might be a function with another signature
		return arC_message_not_implemented(state);
	}

	// Create a new node and add it to the package
	arC_syntax_tree_node_func* const node = arC_syntax_tree_node_func_new(func);
	if (node == NULL)
		return arC_message_out_of_memory(state);
	arC_syntax_tree_node_package_add_func(state->package_node, node);

	// Expected a '(' rune
	if (arC_token_next(t) != ARTOK_PARAN_L) {
		return arC_message_syntax_error(state, "expected: (");
	}

	// Parse argument information until we reach that end ')' token
	while (arC_token_next(t) != ARTOK_PARAN_R) {
		// Ignore comma
		if (t->type == ARTOK_COMMA) {
			if (arC_token_next(t) != ARTOK_IDENTITY)
				return arC_message_expected_identifier(state);
		}

		// Identity first
		arC_arg* arg = arC_arg_new(&t->string);
		arC_func_add_arg(func, arg);

		arC_token_next(t);
		arC_type* const type = arCompiler_parse_type(c, t, state);
		if (type == NULL) {
			return FALSE;
		}
		arg->type = asC_symbol(type);
	}
	arC_token_next(t);

	// Return values present?
	if (t->type == ARTOK_IDENTITY) {
		// One return type
		arC_return* ret = arC_return_new();
		arC_func_add_return(func, ret);

		arC_type* const type = arCompiler_parse_type(c, t, state);
		if (type == NULL) {
			return FALSE;
		}
		ret->type = asC_symbol(type);
		arC_token_next(t);
	}
	else if (t->type == ARTOK_PARAN_L) {
		// Parse return information until we reach that end ')' token
		while (arC_token_next(t) != ARTOK_PARAN_R) {
			// Ignore comma
			if (t->type == ARTOK_COMMA) {
				if (arC_token_next(t) != ARTOK_IDENTITY)
					return arC_message_expected_identifier(state);
			}

			arC_return* ret = arC_return_new();
			arC_func_add_return(func, ret);

			arC_type* const type = arCompiler_parse_type(c, t, state);
			if (type == NULL) {
				return FALSE;
			}
			ret->type = asC_symbol(type);
		}
		// Skip '('
		arC_token_next(t);
	}

	// Expected body for now. Perhaps re-use this function to parse a function pointer definition in the future
	if (t->type != ARTOK_BRACKET_L) {
		return arC_message_syntax_error(state, "expected {");
	}
	arC_token_next(t);

	// Parse the body
	const arC_state child_scope = {
			state->compiler,
			state->token,
			asC_syntax_tree(node),
			state->package_node,
			node,
			state->type_node
	};
	if (!arCompiler_parse_body(c, t, &child_scope))
		return FALSE;
	arC_token_next(t);
	return TRUE;
}

BOOL arCompiler_parse_package(arCompiler* c, arC_token* t, const arC_state* state)
{
	// Expected package name
	if (t->type != ARTOK_IDENTITY) {
		return arC_message_expected_identifier(state);
	}

	// Search for a package of a specific name and parent
	arC_package* package = arCompiler_find_package(c, &t->string, state);
	if (package == NULL) {
		// Create a package representation
		package = arC_package_new(&t->string);
		if (package == NULL) 
			return arC_message_out_of_memory(state);

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
	arC_token_next(t);

	arC_syntax_tree_node_package* const node = arC_syntax_tree_node_package_new(package);
	if (node == NULL)
		return arC_message_out_of_memory(state);

	// This is a recursive package name. Continue until we don't have any more package name keywords
	if (t->type == ARTOK_DOT) {
		const arC_state child_scope = {
			state->compiler,
			state->token,
			asC_syntax_tree(node),
			node,
			state->func_node,
			state->type_node
		};
		arC_token_next(t);
		return arCompiler_parse_package(c, t, &child_scope);
	}
	
	if (t->type != ARTOK_NEWLINE) {
		// arC_message_expected_newline(c);
		return arC_message_not_implemented(state);
	}

	// Add the package to the root node
	arC_syntax_tree_root_add(c->root_node, asC_syntax_tree(node));

	// Parse package content
	while (1)
	{
		switch (t->type)
		{
		case ARTOK_KEYWORD_FUNC:
		{
			// Parse the function
			const arC_state child_scope = {
				state->compiler,
				state->token,
				asC_syntax_tree(node),
				node,
				state->func_node,
				state->type_node
			};
			arC_token_next(t);
			if (!arCompiler_parse_func(c, t, &child_scope)) {
				return FALSE;
			}
			continue;
		}
		case ARTOK_NEWLINE:
		{
			arC_token_next(t);
			continue;
		}
		case ARTOK_EOF:
			return TRUE;
		default:
			return arC_message_not_implemented(state);
		}
	}
}

BOOL arCompiler_parse_source_code(arCompiler* c, arC_token* t)
{
	// Parse all tokens and build a syntax tree from it
	while (1) {
		if (t->type == ARTOK_KEYWORD_PACKAGE) {
			const arC_state state = { c, t, NULL, NULL, NULL, NULL };
			arC_token_next(t);
			if (!arCompiler_parse_package(c, t, &state)) {
				return FALSE;
			}
		}
		else if (t->type == ARTOK_EOF) {
			break;
		}
		else if (t->type == ARTOK_NEWLINE) {
			arC_token_next(t);
		}
		else {
			arC_message_expected_package(c, t);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL arCompiler_add_source_code(arCompiler* c, arC_source_code* source)
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
	arC_lexer lexer;
	arC_token token;
	arC_lexer_init(&lexer, source->source.start);
	arC_token_init(&token, &lexer);
	return arCompiler_parse_source_code(c, &token);
}

void arCompiler_raise_message(arCompiler* c, arMessage* msg)
{
	arMessages_append(&c->messages, msg);
}

arByte* arCompiler_compile(arCompiler* c)
{
	c->pipeline = arBuilder_new();

	// Solve all pending packages, types, functions etc
	arC_package* package = c->packages;
	while (package) {
		arB_package* resolved = arC_package_resolve_package(package, c);
		if (resolved == NULL)
			return NULL;
		if (!arBuilder_add_package(c->pipeline, resolved))
			return NULL;
		package = package->tail;
	}

	// Optimize the syntax tree before compiling it into bytecode
	const arC_state state = {
		c, NULL, NULL, NULL, NULL, NULL
	};
	if (!arC_synax_tree_optimize(&state, c->root_node))
		return NULL;

	// Compile the syntax tree
	if (!arC_syntax_tree_compile(c, asC_syntax_tree(c->root_node)))
		return NULL;


	if (!arBuilder_resolve(c->pipeline)) {
		return NULL;
	}

	// Let the actual builder compile the bytecode. If compilation didn't work then make sure to move all messages
	// from the builder to the compiler
	if (!arBuilder_compile(c->pipeline)) {
		arMessages_move(&c->pipeline->messages, &c->messages);
		return FALSE;
	}

	return arBuilder_get_bytecode(c->pipeline);
}
