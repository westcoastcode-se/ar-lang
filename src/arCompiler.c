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
CONST_VM_STRING(arCompiler, root, "<root>", 6)

BOOL arCompiler_import_root(arC_syntax_tree_node_package* package, const arC_state* s)
{
	arC_syntax_tree_node_import* import = arC_syntax_tree_node_import_new(s->compiler->root_node);
	if (import == NULL)
		return arC_message_out_of_memory(s);
	arC_syntax_tree_add(asC_syntax_tree(package), asC_syntax_tree(import));
	return TRUE;
}

arC_syntax_tree_node_type* arCompiler_root_add_type(arC_syntax_tree_node_package* root, const arString* name, arUint32 size, arUint32 flags, arUint8 data_type, arC_syntax_tree_node_type* of_type)
{
	arC_syntax_tree_node_type* const node = arC_syntax_tree_node_type_new(name);
	if (node == NULL) {
		// TODO: Out of memory
		return NULL;
	}
	node->symbol->flags = flags;
	node->symbol->data_type = data_type;
	if (of_type)
		node->symbol->of_type = of_type->symbol;
	node->symbol->size = size;
	arC_package_add_type(root->symbol, node->symbol);
	arC_syntax_tree_add(asC_syntax_tree(root), asC_syntax_tree(node));
	return node;
}

arC_syntax_tree_node_package* arCompiler_create_system_package()
{
	arC_syntax_tree_node_package* const root = arC_syntax_tree_node_package_new(GET_CONST_VM_STRING(arCompiler, root));
	if (root == NULL)
		return NULL;

	arC_syntax_tree_node_type* type; 
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, bool), sizeof(arInt32), 0, ARLANG_PRIMITIVE_BOOL, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, pbool), sizeof(BOOL*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, int8), sizeof(arInt8), 0, ARLANG_PRIMITIVE_I8, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, pint8), sizeof(arInt8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, uint8), sizeof(arUint8), 0, ARLANG_PRIMITIVE_UI8, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, puint8), sizeof(arUint8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, int16), sizeof(arInt16), 0, ARLANG_PRIMITIVE_I16, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, pint16), sizeof(arInt16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, uint16), sizeof(arUint16), 0, ARLANG_PRIMITIVE_UI16, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, puint16), sizeof(arUint16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, int32), sizeof(arInt32), 0, ARLANG_PRIMITIVE_I32, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, pint32), sizeof(arInt32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, uint32), sizeof(arUint32), 0, ARLANG_PRIMITIVE_UI32, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, puint32), sizeof(arUint32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, int64), sizeof(arInt64), 0, ARLANG_PRIMITIVE_I64, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, pint64), sizeof(arInt64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, uint64), sizeof(arUint64), 0, ARLANG_PRIMITIVE_UI64, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, puint64), sizeof(arUint64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, float32), sizeof(arFloat32), 0, ARLANG_PRIMITIVE_F32, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, pfloat32), sizeof(arFloat32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, float64), sizeof(arFloat64), 0, ARLANG_PRIMITIVE_F64, NULL);
	type = arCompiler_root_add_type(root, GET_CONST_VM_STRING(arCompiler, pfloat64), sizeof(arFloat64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type);

	return root;
}

BOOL arCompiler_init(arCompiler* c)
{
	// Prepare a root node and associate types so that they are global
	c->root_node = arCompiler_create_system_package(c->root_node);
	if (c->root_node == NULL)
		return FALSE;
	return TRUE;
}

arCompiler* arCompiler_new()
{
	arCompiler* const p = arSafeMalloc(arCompiler);
	arMessages_init(&p->messages);
	arMessage_init(&p->panic);
	p->sources = p->sources_end = NULL;
	p->root_node = NULL;
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
		arC_syntax_tree_node_destroy(asC_syntax_tree(ptr->root_node));

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

arC_type* arCompiler_find_or_create_type(arC_token* t, const arC_state* s)
{
	// Is this a pointer type - for example: *int32
	if (t->type == ARTOK_PTR) {
		arC_token_next(t);
		// Fetch the type the pointer is based on. If NULL then an error has occurred
		arC_type* of_type = arCompiler_find_or_create_type(t, s);
		if (of_type == NULL)
			return NULL;

		// TODO: Implement the rest!
		arC_message_not_implemented(s);
		return NULL;
	}
	// Is this an array type - for example: [2]int32
	else if (t->type == ARTOK_SQUARE_L) {
		// TODO: Implement the rest!
		arC_message_not_implemented(s);
		return NULL;
	}
	
	if (t->type != ARTOK_IDENTITY) {
		arC_message_expected_identifier(s);
		return NULL;
	}

	arC_syntax_tree_node_package* closet_package_node = s->package_node;

	//
	// There are some rules regarding where types are allowed to be defined:
	// 
	// 1. Types in a package can be defined anywhere in the package
	// 2. Types inside functions MUST be defined before the type is used
	// 3. Types are not allowed to be inside other types
	// 4. Imports should only be considered on the first level of access

	arC_syntax_tree_node parent_node = s->parent_node;
	arString name = t->string;
	arC_syntax_tree_node node = arC_syntax_tree_find_incl_imports(parent_node, &name, BIT(arC_SYNTAX_TREE_TYPE) | BIT(arC_SYNTAX_TREE_PACKAGE));
	if (node == NULL) {
		arC_token_next(t);
		// This might be a package or a type. If the next token is a dot then 
		// assume that the type we searched for is a package. If it's a package then all searches we are doing after this should 
		// only include children
		if (t->type == ARTOK_DOT) {
			arC_syntax_tree_node_package* const package_node = arC_syntax_tree_node_package_new(&name);
			if (package_node == NULL) {
				arC_message_out_of_memory(s);
				return NULL;
			}
			if (!arCompiler_import_root(package_node, s))
				return FALSE;
			node = asC_symbol(package_node);
			arC_syntax_tree_add(asC_symbol(closet_package_node), node);
			arC_package_add_package(closet_package_node->symbol, package_node->symbol);
			closet_package_node = package_node;
		}
		else {
			arC_syntax_tree_node_type* const type_node = arC_syntax_tree_node_type_new(&name);
			if (type_node == NULL) {
				arC_message_out_of_memory(s);
				return NULL;
			}
			node = asC_symbol(type_node);
			arC_syntax_tree_add(parent_node, node);
			arC_package_add_type(closet_package_node->symbol, type_node->symbol);
		}
	}

	if (node != NULL && node->type == arC_SYNTAX_TREE_PACKAGE) {
		// If this is a package, then assume we have the format: Package1.Package2.Type
		while (TRUE) {
			parent_node = node;
			name = t->string;
			node = arC_syntax_tree_find_child_with_type(parent_node, &name, BIT(arC_SYNTAX_TREE_TYPE) | BIT(arC_SYNTAX_TREE_PACKAGE));
			arC_token_next(t);
			if (node == NULL) {
				if (t->type == ARTOK_DOT) {
					arC_syntax_tree_node_package* const package_node = arC_syntax_tree_node_package_new(&name);
					if (package_node == NULL) {
						arC_message_out_of_memory(s);
						return NULL;
					}
					if (!arCompiler_import_root(package_node, s))
						return FALSE;
					node = asC_symbol(package_node);
					arC_syntax_tree_add(parent_node, node);
					arC_package_add_package(closet_package_node->symbol, package_node->symbol);
					closet_package_node = package_node;
				}
			}

			if (t->type != ARTOK_DOT)
				break;
		}

		// If we never found a node at the end of the package-chain then assume that that's a type (that's not defined yet)
		if (node == NULL) {
			// Prepare a node for the package
			arC_syntax_tree_node_type* const type_node = arC_syntax_tree_node_type_new(&name);
			if (type_node == NULL) {
				arC_message_out_of_memory(s);
				return NULL;
			}
			node = asC_symbol(type_node);
			arC_syntax_tree_add(parent_node, node);
			arC_package_add_type(closet_package_node->symbol, type_node->symbol);
		}
	}
	arC_token_next(t);
	assert(node->type == arC_SYNTAX_TREE_TYPE);
	return ((arC_syntax_tree_node_type*)node)->symbol;
}

BOOL arCompiler_parse_type(arCompiler* c, arC_token* t, const arC_state* state)
{
	return arC_message_not_implemented(state);
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

BOOL arCompiler_parse_func(arCompiler* c, arC_token* t, const arC_state* s)
{
	// Expected function name
	if (t->type != ARTOK_IDENTITY) {
		return arC_message_expected_identifier(s);
	}

	// Search for a function with the supplied name
	arC_syntax_tree_node_func* func = (arC_syntax_tree_node_func*)arC_syntax_tree_find_child_with_type(s->parent_node, &t->string, arC_SYNTAX_TREE_FUNC);
	if (func == NULL) {
		// Create the node if it cant be found
		func = arC_syntax_tree_node_func_new(&t->string);
		if (func == NULL)
			return arC_message_out_of_memory(s);

		//arC_syntax_tree_node closest_container = arC_syntax_tree_find_incl_parents(s->parent_node, BIT(arC_SYNTAX_TREE_FUNC) | BIT(arC_SYNTAX_TREE_PACKAGE));
		// TODO: Add support for functions with scope in functions
		//if (closest_container->type != arC_SYNTAX_TREE_FUNC)
		//	return arC_message_not_implemented(s);
		func->closest_package_node = s->package_node;
		// Add this function to the parent node. The parent node might've been a package or another function (inner)
		arC_syntax_tree_add(s->parent_node, asC_syntax_tree(func));
		arC_package_add_func(s->package_node->symbol, func->symbol);
	}
	arC_func* const symbol = func->symbol;

	// Function is already defined
	if (arC_syntax_tree_has_children(asC_syntax_tree(func))) {
		// TODO: Might already be defined. Perhaps this function has another signature?
		return arC_message_already_defined(s, asC_symbol_signature(symbol));
	}

	// Expected a '(' rune
	if (arC_token_next(t) != ARTOK_PARAN_L) {
		return arC_message_syntax_error(s, "expected: (");
	}

	// Parse argument information until we reach that end ')' token
	arC_token_next(t);
	while (t->type != ARTOK_PARAN_R) {
		// Ignore comma
		if (t->type == ARTOK_COMMA) {
			if (arC_token_next(t) != ARTOK_IDENTITY)
				return arC_message_expected_identifier(s);
		}

		// Identity first
		arC_arg* arg = arC_arg_new(&t->string);
		arC_func_add_arg(symbol, arg);
		arC_token_next(t);

		// Find the type
		arC_type* type = arCompiler_find_or_create_type(t, s);
		if (type == NULL)
			return FALSE;
		arg->type = type;
	}
	arC_token_next(t);

	// Return values present?
	if (t->type == ARTOK_IDENTITY) {
		// One return type
		arC_return* ret = arC_return_new();
		arC_func_add_return(symbol, ret);

		// Find the type
		arC_type* type = arCompiler_find_or_create_type(t, s);
		if (type == NULL)
			return FALSE;
		ret->type = type;
	}
	else if (t->type == ARTOK_PARAN_L) {
		// Parse return information until we reach that end ')' token
		arC_token_next(t);
		while (t->type != ARTOK_PARAN_R) {
			// Ignore comma
			if (t->type == ARTOK_COMMA) {
				if (arC_token_next(t) != ARTOK_IDENTITY)
					return arC_message_expected_identifier(s);
			}

			arC_return* ret = arC_return_new();
			arC_func_add_return(symbol, ret);

			// Find the type
			arC_type* type = arCompiler_find_or_create_type(t, s);
			if (type == NULL)
				return FALSE;
			ret->type = type;
		}
		// Skip ')'
		arC_token_next(t);
	}

	// Expected body for now. Perhaps re-use this function to parse a function pointer definition in the future
	if (t->type != ARTOK_BRACKET_L) {
		return arC_message_syntax_error(s, "expected {");
	}
	arC_token_next(t);

	// Parse the body
	const arC_state child_scope = {
			s->compiler,
			s->token,
			asC_syntax_tree(func),
			s->package_node,
			func,
			s->type_node
	};
	if (!arCompiler_parse_body(c, t, &child_scope))
		return FALSE;
	arC_token_next(t);
	return TRUE;
}

BOOL arCompiler_parse_package(arCompiler* c, arC_token* t, const arC_state* s)
{
	// Only packages and root are allowed to be parent to packages
	assert(s->parent_node->type == arC_SYNTAX_TREE_PACKAGE);

	// Expected package name
	if (t->type != ARTOK_IDENTITY) {
		return arC_message_expected_identifier(s);
	}

	// Search for a package in the syntax tree
	arC_syntax_tree_node_package* package = (arC_syntax_tree_node_package*)arC_syntax_tree_find_child_with_type(s->parent_node, &t->string, BIT(arC_SYNTAX_TREE_PACKAGE));
	if (package == NULL) {
		// Create a package and put it into the current parent node
		package = arC_syntax_tree_node_package_new(&t->string);
		if (package == NULL)
			return arC_message_out_of_memory(s);
		if (!arCompiler_import_root(package, s))
			return FALSE;
		arC_syntax_tree_add(s->parent_node, asC_syntax_tree(package));
	}
	arC_token_next(t);

	// This is a recursive package name. Continue until we don't have any more package symbols
	if (t->type == ARTOK_DOT) {
		const arC_state child_scope = {
			s->compiler,
			s->token,
			asC_syntax_tree(package),
			package,
			s->func_node,
			s->type_node
		};
		arC_token_next(t);
		return arCompiler_parse_package(c, t, &child_scope);
	}

	if (t->type != ARTOK_NEWLINE) {
		// arC_message_expected_newline(c);
		return arC_message_not_implemented(s);
	}

	// Parse package content
	while (1)
	{
		switch (t->type)
		{
		case ARTOK_KEYWORD_FUNC:
		{
			arC_token_next(t);
			const arC_state child_scope = {
				s->compiler,
				s->token,
				asC_syntax_tree(package),
				package,
				s->func_node,
				s->type_node
			};
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
			return arC_message_not_implemented(s);
		}
	}
}

BOOL arCompiler_parse_source_code(arCompiler* c, arC_token* t)
{
	// Parse all tokens and build a syntax tree from it
	while (1) {
		if (t->type == ARTOK_KEYWORD_PACKAGE) {
			const arC_state state = { c, t, asC_syntax_tree(c->root_node), c->root_node, NULL, NULL};
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
	
	const arC_state state = {
		c, NULL, NULL, NULL, NULL, NULL
	};
	// Optimize tree
	if (!arC_synax_tree_optimize(&state, c->root_node))
		return NULL;

	// Resolve all types
	if (!arC_syntax_tree_node_resolve(asC_syntax_tree(c->root_node), &state))
		return NULL;

	// Compile the syntax tree
	if (!arC_syntax_tree_compile(c, asC_syntax_tree(c->root_node)))
		return NULL;

	// Pre pipeline logic
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
