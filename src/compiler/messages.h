#ifndef _ARC_MESSAGES_H_
#define _ARC_MESSAGES_H_

#include "../arMessage.h"
#include "../arString.h"
#include "message_codes.h"

ARLANG_API BOOL arC_message_panic(const struct arC_state* s, const char* str, arC_message_code code);
ARLANG_API BOOL arC_message_out_of_memory(const struct arC_state* s);
ARLANG_API BOOL arC_message_not_implemented(const struct arC_state* s, const char* extra);
ARLANG_API BOOL arC_message_expected_package(struct arCompiler* c, struct arC_token* t);
ARLANG_API BOOL arC_message_expected_identifier(const struct arC_state* s);
ARLANG_API BOOL arC_message_syntax_error(const struct arC_state* s, const char* msg);
ARLANG_API BOOL arC_message_already_defined(const struct arC_state* s, const arString* name);
ARLANG_API BOOL arC_message_not_defined(const struct arC_state* s, const arString* name);
ARLANG_API BOOL arC_message_symbol_unresolved(const struct arC_state* s, const arString* name);
ARLANG_API BOOL arC_message_type_unresolved(const struct arC_state* s, const arString* name);
ARLANG_API BOOL arC_message_package_unresolved(const struct arC_state* s, const arString* name);
ARLANG_API BOOL arC_message_type_recursive_dependency(const struct arC_state* s, const char* relationships);
ARLANG_API BOOL arC_message_feature_missing(const struct arC_state* s, const char* extra);

#endif
