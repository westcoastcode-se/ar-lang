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

arC_syntax_tree_node_type* arCompiler_root_add_type(arC_syntax_tree_node_package* root, const arString* name, arUint32 size, arUint32 flags, arUint8 data_type, arC_syntax_tree_node_type* of_type)
{
	// Prepare a signature for types in the root package
	arC_type_sign signature;
	arC_type_sign_init(&signature);
	signature.name = *name;
	signature.package = root->symbol;
	signature.signature = *name;
	signature.short_signature = *name;

	// Create the type node
	arC_syntax_tree_node_type* const node = arC_syntax_tree_node_type_new(&signature);
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
	p->pipeline = arBuilder_new();
	if (p->pipeline == NULL) {
		arCompiler_destroy(p);
		return NULL;
	}
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

BOOL arC_func_sign_parse(arC_func_sign* sign, const arC_state* s)
{
	arC_token* const t = s->token;

	// Expected function name
	if (t->type != ARTOK_IDENTITY)
		return arC_message_expected_identifier(s);

	sign->name = t->string;
	sign->arguments = sign->arguments_end = NULL;
	sign->arguments_count = 0;
	sign->returns = sign->returns_end = NULL;
	sign->returns_count = 0;
	sign->package = s->package_node->symbol;

	// Expected a '(' rune
	if (arC_token_next(t) != ARTOK_PARAN_L)
		return arC_message_syntax_error(s, "expected: (");

	// Parse argument information until we reach that end ')' token
	arC_token_next(t);
	while (t->type != ARTOK_PARAN_R) {
		// Ignore comma
		if (t->type == ARTOK_COMMA) {
			if (arC_token_next(t) != ARTOK_IDENTITY)
				return arC_message_expected_identifier(s);
		}

		// Identity first
		arC_arg* const arg = arC_arg_new(&t->string);
		if (arg == NULL)
			return arC_message_out_of_memory(s);

		if (sign->arguments == NULL) {
			sign->arguments = sign->arguments_end = arg;
		}
		else {
			sign->arguments_end->tail = arg;
			arg->head = sign->arguments_end;
			sign->arguments_end = arg;
		}
		sign->arguments_count++;

		// Find the type
		arC_token_next(t);
		arC_syntax_tree_node_type* node_type = arC_syntax_tree_node_type_parse(t, s);
		if (node_type == NULL)
			return FALSE;
		arg->type = node_type->symbol;
	}
	arC_token_next(t);

	// Return values present?
	if (t->type == ARTOK_IDENTITY) {
		// One return type
		arC_return* const ret = arC_return_new();
		if (ret == NULL)
			return arC_message_out_of_memory(s);

		if (sign->returns == NULL) {
			sign->returns = sign->returns_end = ret;
		}
		else {
			sign->returns_end->tail = ret;
			ret->head = sign->returns_end;
			sign->returns_end = ret;
		}
		sign->returns_count++;

		// Find the type
		arC_syntax_tree_node_type* node_type = arC_syntax_tree_node_type_parse(t, s);
		if (node_type == NULL)
			return FALSE;
		ret->type = node_type->symbol;
	}
	else if (t->type == ARTOK_PARAN_L) {
		// Parse return information until we reach that end ')' token
		arC_token_next(t);
		while (t->type != ARTOK_PARAN_R) {
			// Ignore comma
			if (t->type == ARTOK_COMMA) {
				arC_token_next(t);
			}

			arC_return* const ret = arC_return_new();
			if (ret == NULL)
				return arC_message_out_of_memory(s);

			if (sign->returns == NULL) {
				sign->returns = sign->returns_end = ret;
			}
			else {
				sign->returns_end->tail = ret;
				ret->head = sign->returns_end;
				sign->returns_end = ret;
			}
			sign->returns_count++;

			// Find the type
			arC_syntax_tree_node_type* node_type = arC_syntax_tree_node_type_parse(t, s);
			if (node_type == NULL)
				return FALSE;
			ret->type = node_type->symbol;
		}
		// Skip ')'
		arC_token_next(t);
	}

	if (!arC_func_sign_build(sign, s))
		return FALSE;
	return TRUE;
}

arC_syntax_tree_node_func* arCompiler_find_or_create_func(arC_token* t, const arC_state* s)
{
	// Expected function name
	if (t->type != ARTOK_IDENTITY) {
		arC_message_expected_identifier(s);
		return NULL;
	}

	// Parse the signature
	arC_func_sign signature;
	if (!arC_func_sign_parse(&signature, s))
		return FALSE;

	// Search for the function using the signature. If no one is found then create a function node
	arC_syntax_tree_node_func* func = (arC_syntax_tree_node_func*)arC_syntax_tree_find_child_with_type(s->parent_node, &signature.signature, arC_SYNTAX_TREE_FUNC);
	if (func == NULL) {
		// Create the node if it cant be found
		func = arC_syntax_tree_node_func_new(&signature);
		if (func == NULL) {
			arC_message_out_of_memory(s);
			return NULL;
		}

		//arC_syntax_tree_node closest_container = arC_syntax_tree_find_incl_parents(s->parent_node, BIT(arC_SYNTAX_TREE_FUNC) | BIT(arC_SYNTAX_TREE_PACKAGE));
		// TODO: Add support for functions with scope in functions
		//if (closest_container->type != arC_SYNTAX_TREE_FUNC)
		//	return arC_message_not_implemented(s);
		func->closest_package_node = s->package_node;
		// Add this function to the parent node. The parent node might've been a package or another function (inner)
		arC_syntax_tree_add(s->parent_node, asC_syntax_tree(func));
		arC_package_add_func(s->package_node->symbol, func->symbol);
	}
	return func;
}

BOOL arCompiler_parse_func(arCompiler* c, arC_token* t, const arC_state* s)
{
	// Expected function name
	if (t->type != ARTOK_IDENTITY) {
		return arC_message_expected_identifier(s);
	}

	// Search for a function with the supplied name
	arC_syntax_tree_node_func* func = arCompiler_find_or_create_func(t, s);
	if (func == NULL)
		return FALSE;

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
		if (!arC_syntax_tree_node_import_root(package, s))
			return FALSE;
		arC_syntax_tree_add(s->parent_node, asC_syntax_tree(package));
		// Does this package have a parent that's not the root node
		if (s->package_node != NULL && s->package_node != c->root_node) {
			arC_package_add_package(s->package_node->symbol, package->symbol);
		}
		arC_package_build_signature(package->symbol, s);
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
