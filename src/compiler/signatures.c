#include "signatures.h"
#include "symbol.h"
#include "syntax_tree.h"
#include "messages.h"
#include "tokens.h"
#include "../arUtils.h"
#include "../arCompiler.h"

void arC_signature_package_init(arC_signature_package* p)
{
	arString_zero(&p->signature);
	arString_zero(&p->local_signature);
	arString_zero(&p->name);
	p->parent = NULL;
}

BOOL arC_signature_package_fixed(arC_signature_package* p, const struct arC_state* s, arInt32 length)
{
	arByte signature_data[1024];
	arByte* sig = signature_data;

	sig = arString_cpy(sig, asC_symbol_signature(p->parent));
	sig = arStrcpy(sig, ".", 1);
	sig = arString_cpy(sig, &p->name);
	const arString* const result = arStringPool_stringsz(arC_state_get_string_pool(s), signature_data, length);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	p->signature = *result;
	return TRUE;
}

BOOL arC_signature_package_dynamic(arC_signature_package* p, const struct arC_state* s, arInt32 length)
{
	arByte* signature_data = arMalloc(length);
	arByte* sig = signature_data;
	if (signature_data == NULL)
		return arC_message_out_of_memory(s);

	sig = arString_cpy(sig, asC_symbol_signature(p->parent));
	sig = arStrcpy(sig, ".", 1);
	sig = arString_cpy(sig, &p->name);
	const arString* const result = arStringPool_stringsz(arC_state_get_string_pool(s), signature_data, length);
	arFree(signature_data);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	p->signature = *result;
	return TRUE;
}

BOOL arC_signature_package_build(arC_signature_package* p, const arC_state* s)
{
	ASSERT_NOT_NULL(p);
	// If the parent is the root package
	if (p->parent->parent == NULL) {
		p->signature = p->name;
		p->local_signature = p->name;
		return TRUE;
	}

	// Pre-calculate the total length of the package signature
	const int total_length = arString_length(asC_symbol_signature(p->parent)) + 1 +
		arString_length(&p->name);
	if (total_length > 1024)
		return arC_signature_package_dynamic(p, s, total_length);
	return arC_signature_package_fixed(p, s, total_length);
}

void arC_signature_type_init(arC_signature_type* p)
{
	arString_zero(&p->signature);
	arString_zero(&p->local_signature);
	arString_zero(&p->name);
	p->parent = NULL;
}

BOOL arC_signature_type_init_and_build(arC_signature_type* p, const arString* name, arC_symbol* parent, const arC_state* s)
{
	arC_signature_type_init(p);
	p->parent = parent;
	p->name = *name;
	return arC_signature_type_build(p, s);
}

BOOL arC_signature_type_build_fixed(arC_signature_type* sign, const arC_state* s, arInt32 length)
{
	arByte signature_data[1024];
	arByte* sig = signature_data;

	sig = arString_cpy(sig, &sign->parent->signature);
	sig = arStrcpy(sig, "#", 1);
	sig = arString_cpy(sig, &sign->name);

	const arString* const result = arStringPool_stringsz(&s->compiler->pipeline->string_pool, signature_data, length);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	sign->signature = *result;
	// The short signature for the function startd after the "<package signature>#"
	sign->local_signature.start = sign->signature.start + arString_length(&sign->parent->signature) + 1;
	sign->local_signature.end = sign->signature.end;
	return TRUE;
}

BOOL arC_signature_type_build_dynamic(arC_signature_type* sign, const arC_state* s, arInt32 length)
{
	arByte* signature_data = arMalloc(length);
	arByte* sig = signature_data;
	if (signature_data == NULL)
		return arC_message_out_of_memory(s);

	sig = arString_cpy(sig, &sign->parent->signature);
	sig = arStrcpy(sig, "#", 1);
	sig = arString_cpy(sig, &sign->name);

	const arString* const result = arStringPool_stringsz(&s->compiler->pipeline->string_pool, signature_data, length);
	arFree(signature_data);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	sign->signature = *result;
	// The short signature for the function startd after the "<package signature>#"
	sign->local_signature.start = sign->signature.start + arString_length(&sign->parent->signature) + 1;
	sign->local_signature.end = sign->signature.end;
	return TRUE;
}

BOOL arC_signature_type_build(arC_signature_type* sign, const arC_state* s)
{
	ASSERT_NOT_NULL(sign);
	const arInt32 length = arString_length(&sign->parent->signature) + 1 +
		arString_length(&sign->name);
	if (length > 1024)
		return arC_signature_type_build_dynamic(sign, s, length);
	return arC_signature_type_build_fixed(sign, s, length);
}

void arC_signature_func_init(arC_signature_func* p)
{
	arString_zero(&p->signature);
	arString_zero(&p->local_signature);
	arString_zero(&p->name);
	p->parent = NULL;
	p->arguments = p->arguments_end = NULL;
	p->arguments_count = 0;
	p->returns = p->returns_end = NULL;
	p->returns_count = 0;
}

BOOL arC_signature_func_parse(arC_signature_func* sign, const arC_state* s)
{
	arC_token* const t = s->token;
	ASSERT_NOT_NULL(s->parent_node && "Signature must be part of a parent node, such as another function or a package");

	// TODO: Add support for functions inside functions
	if (s->parent_node->type != arC_SYNTAX_TREE_PACKAGE)
		return arC_message_not_implemented(s, "#7 adds support for functions inside functions");

	// Expected function name
	if (t->type != ARTOK_IDENTITY)
		return arC_message_expected_identifier(s);

	sign->name = t->string;
	sign->arguments = sign->arguments_end = NULL;
	sign->returns = sign->returns_end = NULL;
	sign->parent = asC_symbol(s->package_node->symbol);

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

	if (!arC_signature_func_build(sign, s))
		return FALSE;
	return TRUE;
}

BOOL arC_signature_func_build(arC_signature_func* sign, const arC_state* s)
{
	ASSERT_NOT_NULL(sign);

	int bytes_left = 1024;
	int short_signature_length = 0;
	arByte signature_data[1024];
	arByte* sig = signature_data;

	sig = arString_cpy_s(sig, &bytes_left, &sign->parent->signature);
	sig = arStrcpy_s(sig, &bytes_left, "#", 1);
	sig = arString_cpy_s(sig, &bytes_left, &sign->name);
	sig = arStrcpy_s(sig, &bytes_left, "(", 1);
	if (sign->arguments != NULL) {
		arC_arg* arg = sign->arguments;
		const arC_arg* const last = sign->arguments_end->tail;
		for (; arg != last; arg = arg->tail) {
			if (arg != sign->arguments)
				sig = arStrcpy_s(sig, &bytes_left, ",", 1);
			const arString* type_sig = asC_symbol_signature(arg->type);
			sig = arStrcpy_s(sig, &bytes_left, type_sig->start, arString_length(type_sig));
		}
	}
	sig = arStrcpy_s(sig, &bytes_left, ")", 1);
	if (bytes_left == 0) {
		// Size of the signature is too large
		return arC_message_not_implemented(s, "dynamic set memory size?");
	}

	const arString* const result = arStringPool_stringsz(&s->compiler->pipeline->string_pool, signature_data, 1024 - bytes_left);
	if (result == NULL)
		return arC_message_out_of_memory(s);
	sign->signature = *result;
	// The local signature for the function starts after the "<package signature>#"
	sign->local_signature.start = sign->signature.start + arString_length(&sign->parent->signature) + 1;
	sign->local_signature.end = sign->signature.end;
	return TRUE;
}
