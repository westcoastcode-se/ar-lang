#ifndef _ARB_MESSAGES_H_
#define _ARB_MESSAGES_H_

#include "../arMessage.h"
#include "../arString.h"

// 
// Functions which helps adding messages to the messages container
//

ARLANG_API BOOL arB_message_unknown_instr(struct arBuilder* b, arInt32 instr_type);
ARLANG_API BOOL arB_message_arg_index_missing(struct arBuilder* b, arInt32 index);
ARLANG_API BOOL arB_message_return_index_missing(struct arBuilder* b, arInt32 index);
ARLANG_API BOOL arB_message_local_index_missing(struct arBuilder* b, arInt32 index);
ARLANG_API BOOL arB_message_header_prediction_failed(struct arBuilder* b, arUint32 expected, arUint32 was);
ARLANG_API BOOL arB_message_types_not_compatible(struct arBuilder* b, const arString* from_name, const arString* to_name);
ARLANG_API BOOL arB_message_type_too_large(struct arBuilder* b, const arString* type_name, arInt32 size, arInt32 max_size);
ARLANG_API BOOL arB_message_type_not_defined(struct arBuilder* b, const arString* type_name);
ARLANG_API BOOL arB_message_type_not_array(struct arBuilder* b, const arString* type_name);
ARLANG_API BOOL arB_message_instr_requires_prev_instr(struct arBuilder* b, const char* instr_name);
ARLANG_API BOOL arB_message_expected_value_on_stack(struct arBuilder* b, const arString* type_name);
ARLANG_API BOOL arB_message_expected_const_smaller_than(struct arBuilder* b, arInt64 value, arInt64 max_value);
ARLANG_API BOOL arB_message_expected_const_larger_than(struct arBuilder* b, arInt64 value, arInt64 min_value);
ARLANG_API BOOL arB_message_not_implemented(struct arBuilder* b, const char* implementation);
ARLANG_API BOOL arB_panic_out_of_memory(struct arBuilder* b);

#endif
