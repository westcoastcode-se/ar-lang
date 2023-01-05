#include "arCompiler.h"
#include "arMemory.h"
#include "compiler/tokens.h"
#include "compiler/messages.h"
#include "compiler/search.h"
#include "compiler/syntax_tree_compiler.h"
#include "compiler/syntax_tree_optimize.h"
#include "compiler/syntax_tree_resolve.h"
#include "arUtils.h"

CONST_VM_STRING(arCompiler, void, "void", 4)
CONST_VM_STRING(arCompiler, pvoid, "*void", 5)
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

BOOL arCompiler_out_of_memory(arCompiler* c)
{
	const arC_state state = {
		c, NULL, NULL, NULL, NULL, NULL, NULL
	};
	return arC_message_out_of_memory(&state);
}

arC_syntax_tree_typedef* arCompiler_root_add_type(const arC_state* s, arC_syntax_tree_package* root, 
	const arString* name, arUint32 size, arUint32 flags, arUint8 data_type, arC_syntax_tree_typedef* of_type,
	arC_syntax_tree_typedef* inherits_from)
{
	// Create the type node
	arC_syntax_tree_typedef* const node = arC_syntax_tree_typedef_new(s);
	if (node == NULL)
		return NULL;
	node->name = *name;
	node->flags = flags;
	node->resolved.data_type = data_type;
	node->resolved.of_type = of_type;
	node->resolved.inherits_from = inherits_from;
	node->resolved.stack_size = size;
	node->package = root;
	arC_syntax_tree_add_child(asC_syntax_tree(root), asC_syntax_tree(node));
	return node;
}

arC_syntax_tree_package* arCompiler_create_system_package(const arC_state* s)
{
	// Create the root package
	arC_syntax_tree_package* const root = arC_syntax_tree_package_new(s);
	if (root == NULL)
		return NULL;
	root->signature.name = root->signature.local_signature = root->signature.signature = *GET_CONST_VM_STRING(arCompiler, root);
	
	// Add built-in types and functions
	arC_syntax_tree_typedef* voidptr;
	arC_syntax_tree_typedef* type;
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, void), 0, arB_TYPE_FLAGS_VOID, ARLANG_PRIMITIVE_UNKNOWN, NULL, NULL);
	voidptr = type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pvoid), sizeof(void*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, bool), sizeof(arInt32), 0, ARLANG_PRIMITIVE_BOOL, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pbool), sizeof(BOOL*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, int8), sizeof(arInt8), 0, ARLANG_PRIMITIVE_I8, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pint8), sizeof(arInt8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, uint8), sizeof(arUint8), 0, ARLANG_PRIMITIVE_UI8, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, puint8), sizeof(arUint8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, int16), sizeof(arInt16), 0, ARLANG_PRIMITIVE_I16, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pint16), sizeof(arInt16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, uint16), sizeof(arUint16), 0, ARLANG_PRIMITIVE_UI16, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, puint16), sizeof(arUint16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, int32), sizeof(arInt32), 0, ARLANG_PRIMITIVE_I32, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pint32), sizeof(arInt32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, uint32), sizeof(arUint32), 0, ARLANG_PRIMITIVE_UI32, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, puint32), sizeof(arUint32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, int64), sizeof(arInt64), 0, ARLANG_PRIMITIVE_I64, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pint64), sizeof(arInt64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, uint64), sizeof(arUint64), 0, ARLANG_PRIMITIVE_UI64, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, puint64), sizeof(arUint64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, float32), sizeof(arFloat32), 0, ARLANG_PRIMITIVE_F32, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pfloat32), sizeof(arFloat32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, float64), sizeof(arFloat64), 0, ARLANG_PRIMITIVE_F64, NULL, NULL);
	type = arCompiler_root_add_type(s, root, GET_CONST_VM_STRING(arCompiler, pfloat64), sizeof(arFloat64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type, voidptr);
	if (type == NULL)
		return NULL;
	return root;
}

BOOL arCompiler_init(arCompiler* c)
{
	if (!arMemoryPool_init(&c->memory_pool))
		return FALSE;

	const arC_state state = { c, &c->memory_pool, NULL, asC_syntax_tree(c->root_node),
		c->root_node, NULL, NULL };
	// Prepare a root node and associate types so that they are global
	c->root_node = arCompiler_create_system_package(&state);
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
	p->pipeline = NULL;
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

	// Destroy all source codes
	arC_source_code* source = ptr->sources;
	while (source) {
		arC_source_code* const tail = source->tail;
		arC_source_code_destroy(source);
		source = tail;
	}

	arMemoryPool_release(&ptr->memory_pool);
	arMessages_destroy(&ptr->messages);
	arFree(ptr);
}

BOOL arCompiler_parse_type(arCompiler* c, arC_token* t, const arC_state* state)
{
	return arC_message_not_implemented(state, "#8 add support for type aliases");
}

arC_syntax_tree_funcdef_body* arCompiler_create_funcdef_body(arC_token* t, const arC_state* s)
{
	arC_syntax_tree_funcdef_body* const body = arC_syntax_tree_funcdef_body_new(s);
	if (body == NULL)
		return NULL;

	const arC_state child_scope = asC_state(s,
		asC_syntax_tree(body),
		s->package_node,
		s->func_node,
		s->type_node
	);
	// Parse until we've reached the end of the body
	while (t->type != ARTOK_BRACKET_R) {
		if (t->type == ARTOK_BRACKET_L) {
			// Allow scope
			arC_message_not_implemented(s, "Scope in scope");
			return NULL;
		}
		else if (t->type == ARTOK_BRACKET_R) {
			continue;
		}
		else if (t->type == ARTOK_NEWLINE) {
			arC_token_next(t);
			continue;
		}

		arC_syntax_tree_node node = arC_syntax_tree_funcdef_body_parse(&child_scope);
		if (arC_syntax_tree_is_error(node))
			return FALSE;
		arC_syntax_tree_add_child(child_scope.parent_node, node);
	}
	return body;
}

arC_syntax_tree_typeref* arCompiler_parse_typeref(arC_token* t, const arC_state* s)
{
	// Is this a pointer type - for example: *int32
	if (t->type == ARTOK_PTR) {
		arC_token_next(t);
		// Fetch the type the pointer is based on. If NULL then an error has occurred
		arC_syntax_tree_typeref* of_type = arCompiler_parse_typeref(t, s);
		if (of_type == NULL)
			return NULL;

		// TODO: Implement the rest!
		arC_message_not_implemented(s, "#10 add support for pointer types");
		return NULL;
	}
	// Is this an array type - for example: [2]int32
	else if (t->type == ARTOK_SQUARE_L) {
		// TODO: Implement the rest!
		arC_message_not_implemented(s, "#9 add support for array types");
		return NULL;
	}

	if (t->type != ARTOK_IDENTITY) {
		arC_message_expected_identifier(s);
		return NULL;
	}

	arC_syntax_tree_package* closet_package = s->package_node;
	arC_syntax_tree_node parent = s->parent_node;
	arC_syntax_tree_typeref* const result = arC_syntax_tree_typeref_new(s);
	result->name = t->string;
	result->valid_types = BIT(arC_SYNTAX_TREE_TYPEDEF) | BIT(arC_SYNTAX_TREE_PACKAGE);
	arC_syntax_tree_typeref* prev = result;

	arC_token_next(t);
	if (t->type != ARTOK_DOT) {
		prev->valid_types = BIT(arC_SYNTAX_TREE_TYPEDEF);
		return result;
	}

	while (TRUE) {
		arC_syntax_tree_typeref* const ref = arC_syntax_tree_typeref_new(s);
		ref->name = t->string;
		arC_syntax_tree_add_child(asC_syntax_tree(prev), asC_syntax_tree(ref));

		arC_token_next(t);
		if (t->type != ARTOK_DOT) {
			prev->valid_types = BIT(arC_SYNTAX_TREE_TYPEDEF);
			return result;
		}
		else {
			ref->valid_types = BIT(arC_SYNTAX_TREE_TYPEDEF) | BIT(arC_SYNTAX_TREE_PACKAGE);
			prev = ref;
		}
	}
}

// Create a function
arC_syntax_tree_funcdef* arCompiler_create_funcdef_head(arC_token* t, const arC_state* s)
{
	// TODO: Add support for functions inside functions
	if (s->parent_node->type != arC_SYNTAX_TREE_PACKAGE) {
		arC_message_not_implemented(s, "#7 adds support for functions inside functions");
		return NULL;
	}

	// Expected function name
	if (t->type != ARTOK_IDENTITY) {
		arC_message_expected_identifier(s);
		return NULL;
	}

	const arString funcName = t->string;

	// Expected a '(' rune
	if (arC_token_next(t) != ARTOK_PARAN_L) {
		arC_message_syntax_error(s, "expected: (");
		return NULL;
	}

	// Create the node if it cant be found
	arC_syntax_tree_funcdef* const func = arC_syntax_tree_funcdef_new(s);
	if (func == NULL)
		return NULL;
	func->signature.name = funcName;

	// Add tree node for arguments
	arC_syntax_tree_funcdef_args* const args = arC_syntax_tree_funcdef_args_new(s);
	if (func == NULL)
		return NULL;
	arC_syntax_tree_add_child(asC_syntax_tree(func), asC_syntax_tree(args));
	func->args = args;

	// Parse argument information until we reach that end ')' token
	arC_token_next(t);
	while (t->type != ARTOK_PARAN_R) {
		// Ignore comma
		if (t->type == ARTOK_COMMA) {
			if (arC_token_next(t) != ARTOK_IDENTITY) {
				arC_message_expected_identifier(s);
				return NULL;
			}
		}

		// Identity first
		arC_syntax_tree_funcdef_arg* const arg = arC_syntax_tree_funcdef_arg_new(s);
		if (arg == NULL)
			return NULL;
		arg->name = t->string;
		arg->func = func;
		arC_syntax_tree_add_child(asC_syntax_tree(args), asC_syntax_tree(arg));

		// Type second
		arC_token_next(t);
		arC_syntax_tree_typeref* const type = arCompiler_parse_typeref(t, s);
		if (type == NULL)
			return NULL;
		arC_syntax_tree_add_child(asC_syntax_tree(arg), asC_syntax_tree(type));
		arg->type = type;
		args->count++;
	}
	arC_token_next(t);

	// Add tree node for returns
	arC_syntax_tree_funcdef_rets* const rets = arC_syntax_tree_funcdef_rets_new(s);
	if (func == NULL)
		return NULL;
	arC_syntax_tree_add_child(asC_syntax_tree(func), asC_syntax_tree(rets));
	func->rets = rets;

	// Parse return information
	if (t->type == ARTOK_IDENTITY) {
		arC_syntax_tree_funcdef_ret* const ret = arC_syntax_tree_funcdef_ret_new(s);
		if (ret == NULL)
			return NULL;
		ret->func = func;
		arC_syntax_tree_add_child(asC_syntax_tree(rets), asC_syntax_tree(ret));
		rets->count++;

		arC_syntax_tree_typeref* const type = arCompiler_parse_typeref(t, s);
		if (type == NULL)
			return NULL;
		ret->type = type;
		arC_syntax_tree_add_child(asC_syntax_tree(ret), asC_syntax_tree(type));
	}
	else if (t->type == ARTOK_PARAN_L) {
		// Parse return information until we reach that end ')' token
		arC_token_next(t);
		while (t->type != ARTOK_PARAN_R) {
			// Ignore comma
			if (t->type == ARTOK_COMMA) {
				arC_token_next(t);
			}

			arC_syntax_tree_funcdef_ret* const ret = arC_syntax_tree_funcdef_ret_new(s);
			if (ret == NULL)
				return NULL;
			ret->func = func;
			arC_syntax_tree_add_child(asC_syntax_tree(rets), asC_syntax_tree(ret));
			rets->count++;

			arC_syntax_tree_typeref* const type = arCompiler_parse_typeref(t, s);
			if (type == NULL)
				return NULL;
			arC_syntax_tree_add_child(asC_syntax_tree(ret), asC_syntax_tree(type));
		}
		// Skip ')'
		arC_token_next(t);
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
	arC_syntax_tree_funcdef* func = arCompiler_create_funcdef_head(t, s);
	if (func == NULL)
		return FALSE;
	arC_syntax_tree_add_child(s->parent_node, asC_syntax_tree(func));

	// Expected body for now. Perhaps re-use this function to parse a function pointer definition in the future
	if (t->type != ARTOK_BRACKET_L) {
		return arC_message_syntax_error(s, "expected {");
	}
	arC_token_next(t);

	// Parse the body
	const arC_state child_scope = asC_state(s,
		asC_syntax_tree(func),
		s->package_node,
		func,
		s->type_node
	);
	arC_syntax_tree_funcdef_body* const body = arCompiler_create_funcdef_body(t, &child_scope);
	if (body == NULL)
		return FALSE;
	func->body = body;
	arC_syntax_tree_add_child(asC_syntax_tree(func), asC_syntax_tree(body));
	arC_token_next(t);
	return TRUE;
}

void arCompiler_create_signature(const arC_state* s, arC_syntax_tree_package* ref)
{
	// Calculate the package signature
	arByte signature_data[1024];
	arByte* sig = signature_data;
	if (ref->header.parent != NULL) {
		arC_syntax_tree_package* const parent = (arC_syntax_tree_package*)ref->header.parent;
		sig = arString_cpy(sig, &parent->signature.signature);
		sig = arStrcpy(sig, ".", 1);
	}
	sig = arString_cpy(sig, &ref->signature.local_signature);
	const arString* const result = arStringPool_stringsz(arC_state_get_string_pool(s), signature_data, (int)(sig - signature_data));
	if (result == NULL)
		arC_message_out_of_memory(s);
	else
		ref->signature.signature = *result;
}

BOOL arCompiler_parse_package(arCompiler* c, arC_token* t, const arC_state* s)
{
	// Only packages and root are allowed to be parent to packages
	assert(s->parent_node->type == arC_SYNTAX_TREE_PACKAGE);

	// Expected package name
	if (t->type != ARTOK_IDENTITY) {
		return arC_message_expected_identifier(s);
	}

	// Search for a package in the syntax tree. The package string is a little special because
	// we know that a package definition's parent will be part of the same string, so you can figure out the
	// full signature based on the parent's signatures
	arC_syntax_tree_package* package = (arC_syntax_tree_package*)arC_search_children_once(s->parent_node,
		&t->string, BIT(arC_SYNTAX_TREE_PACKAGE));
	if (package == NULL) {
		// The package was not found? Create it
		package = arC_syntax_tree_package_new(s);
		if (package == NULL)
			return FALSE;
		package->signature.name = package->signature.local_signature = t->string;

		// If the parent package is the root then don't include that in the full signature
		if (arC_syntax_tree_package_is_root(s->package_node)) {
			package->signature.signature = package->signature.name;
		}
		else {
			arCompiler_create_signature(s, package);
		}

		// Automatically add the root import to the package
		arC_syntax_tree_import* const root = arC_syntax_tree_import_new(s);
		root->resolved.ref = c->root_node;
		asC_syntax_tree_phase_set(root, arC_SYNTAX_TREE_PHASE_RESOLVE);
		arC_syntax_tree_package_add_import(package, root);

		// Add the new package to the parent package
		arC_syntax_tree_add_child(s->parent_node, asC_syntax_tree(package));
	}
	arC_token_next(t);

	// This is a recursive package name. Continue until we don't have any more package symbols
	if (t->type == ARTOK_DOT) {
		const arC_state child_scope = asC_state(s,
			asC_syntax_tree(package),
			package,
			s->func_node,
			s->type_node
		);
		arC_token_next(t);
		return arCompiler_parse_package(c, t, &child_scope);
	}

	if (t->type != ARTOK_NEWLINE) {
		// arC_message_expected_newline(c);
		return arC_message_not_implemented(s, "N/A");
	}

	// Parse package content
	while (1)
	{
		switch (t->type)
		{
		case ARTOK_KEYWORD_FUNC:
		{
			arC_token_next(t);
			const arC_state child_scope = asC_state(s,
				asC_syntax_tree(package),
				package,
				s->func_node,
				s->type_node
			);
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
			return arC_message_not_implemented(s, "N/A");
		}
	}
}

BOOL arCompiler_parse_source_code(arCompiler* c, arC_token* t)
{
	// Parse all tokens and build a syntax tree from it. Expected package to be at the top of the file
	while (1) {
		if (t->type == ARTOK_KEYWORD_PACKAGE) {
			const arC_state state = { c, &c->memory_pool, t, asC_syntax_tree(c->root_node), 
				c->root_node, NULL, NULL};
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
		c, &c->memory_pool, NULL, NULL, NULL, NULL, NULL
	};
	// Resolve references
	if (!arC_syntax_tree_resolve_references(&state, asC_syntax_tree(c->root_node)))
		return NULL;

	arC_syntax_tree_stdout(asC_syntax_tree(c->root_node));

	// Optimize tree
	if (!arC_syntax_tree_optimize(&state, c->root_node))
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
