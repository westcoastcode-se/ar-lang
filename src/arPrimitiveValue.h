#ifndef _ARB_CONST_H_
#define _ARB_CONST_H_

#include "arPrimitive.h"

// Represents a constant value up to 8 bytes
typedef struct arPrimitiveValue
{
	// Represents an unknown value up to 8 bytes
	union {
		arInt64 i64;
		arUint64 ui64;
		arFloat64 f64;
		arFloat32 f32;
		arBool bool4;
		arInt32 i32;
		arUint32 ui32;
		arInt16 i16;
		arUint16 ui16;
		arInt8 i8;
		arUint8 ui8;
		arByte* ptr;
	};
	arPrimitive type;
} arPrimitiveValue;

ARLANG_API arPrimitiveValue arPrimitiveValue_i8(arInt8 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_ui8(arUint8 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_i86(arInt16 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_ui86(arUint16 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_i32(arInt32 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_ui32(arUint32 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_i64(arInt64 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_ui64(arUint64 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_f32(arFloat32 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_f64(arFloat64 value);
ARLANG_API arPrimitiveValue arPrimitiveValue_ptr(arByte* value);

// A function pointer that represents when we want to combine two constants
typedef BOOL (*arPrimitiveValue_combine_fn)(arPrimitiveValue* lhs, const arPrimitiveValue* rhs);

// Helper function for when an binop operator between two constants are not implemented
ARLANG_API BOOL arPrimitiveValue_not_implemented(arPrimitiveValue* lhs, const arPrimitiveValue* rhs);

// Helper function for when an binop operator between two constants are not allowed
ARLANG_API BOOL arPrimitiveValue_not_allowed(arPrimitiveValue* lhs, const arPrimitiveValue* rhs);

// Add two constant values put the result back into the lhs variable
// 
// If you add two values where both of the values are less then 4 bytes then the result will be transformed into a 4 byte integer. 
// If one of the types are unsigned, then the resulting value will turn into an unsigned value
// If one of the values are a floating value then the resulting value is turned into a floating value
ARLANG_API BOOL arPrimitiveValue_add(arPrimitiveValue* lhs, const arPrimitiveValue* rhs);

// Subtract two constant values put the result back into the lhs variable
// 
// If you add two values where both of the values are less then 4 bytes then the result will be transformed into a 4 byte integer. 
// If one of the types are unsigned, then the resulting value will turn into an unsigned value
// If one of the values are a floating value then the resulting value is turned into a floating value
ARLANG_API BOOL arPrimitiveValue_sub(arPrimitiveValue* lhs, const arPrimitiveValue* rhs);

// Multiply two constant values put the result back into the lhs variable
// 
// If you add two values where both of the values are less then 4 bytes then the result will be transformed into a 4 byte integer. 
// If one of the types are unsigned, then the resulting value will turn into an unsigned value
// If one of the values are a floating value then the resulting value is turned into a floating value
ARLANG_API BOOL arPrimitiveValue_mult(arPrimitiveValue* lhs, const arPrimitiveValue* rhs);

// Divide two constant values put the result back into the lhs variable
// 
// If you add two values where both of the values are less then 4 bytes then the result will be transformed into a 4 byte integer. 
// If one of the types are unsigned, then the resulting value will turn into an unsigned value
// If one of the values are a floating value then the resulting value is turned into a floating value
ARLANG_API BOOL arPrimitiveValue_div(arPrimitiveValue* lhs, const arPrimitiveValue* rhs);

// Figure out the resulting datatype if two other data-types is processed by an operatur, such as add
ARLANG_API arPrimitive arPrimitiveValue_datatype(arInt32 lhs, arInt32 rhs);

// TODO: Add more operators, such as bit manipulator operators

#endif
