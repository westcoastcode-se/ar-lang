#ifndef _VMP_CONST_H_
#define _VMP_CONST_H_

#include "../vm_config.h"

// Represents a constant value up to 8 bytes
typedef struct vmp_const
{
	// Represents an unknown value up to 8 bytes
	union {
		vm_int64 i8;
		vm_uint64 ui8;
		vm_float64 f8;
		struct {
			vm_float32 f4;
			vm_float32 f4_1;
		};
		struct {
			vm_bool bool4;
			vm_bool bool4_1;
		};
		struct {
			vm_int32 i4;
			vm_int32 i4_1;
		};
		struct {
			vm_uint32 ui4;
			vm_uint32 ui4_1;
		};
		struct {
			vm_int16 i2;
			vm_int16 i2_[3];
		};
		struct {
			vm_uint16 ui2;
			vm_uint16 ui2_[3];
		};
		struct {
			vm_int8 i1;
			vm_int8 i1_[7];
		};
		struct {
			vm_uint8 ui1;
			vm_int8 ui1_[7];
		};
#if defined(VM_64BIT)
		vm_byte* ptr;
#else
		struct {
			vm_byte* ptr;
			vm_byte* ptr_;
		};
#endif
	};
	vm_int32 type;
} vmp_const;

extern vmp_const vmp_const_i1(vm_int8 value);
extern vmp_const vmp_const_ui1(vm_uint8 value);
extern vmp_const vmp_const_i2(vm_int16 value);
extern vmp_const vmp_const_ui2(vm_uint16 value);
extern vmp_const vmp_const_i4(vm_int32 value);
extern vmp_const vmp_const_ui4(vm_uint32 value);
extern vmp_const vmp_const_i8(vm_int64 value);
extern vmp_const vmp_const_ui8(vm_uint64 value);
extern vmp_const vmp_const_f4(vm_float32 value);
extern vmp_const vmp_const_f8(vm_float64 value);
extern vmp_const vmp_const_ptr(vm_byte* value);

// A function pointer that represents when we want to combine two constants
typedef BOOL (*vmp_const_combine_fn)(vmp_const* lhs, const vmp_const* rhs);

// Helper function for when an binop operator between two constants are not implemented
extern BOOL vmp_const_not_implemented(vmp_const* lhs, const vmp_const* rhs);

// Helper function for when an binop operator between two constants are not allowed
extern BOOL vmp_const_not_allowed(vmp_const* lhs, const vmp_const* rhs);

// Add two constant values and return the result. 
// 
// If you add two values where both of the values are less then 4 bytes then the result will be transformed into a 4 byte integer. 
// If one of the types are unsigned, then the resulting value will turn into an unsigned value
// If one of the values are a floating value then the resulting value is turned into a floating value
extern BOOL vmp_const_add(vmp_const* lhs, const vmp_const* rhs);

// Subtract two constant values and return the result
extern BOOL vmp_const_sub(vmp_const* lhs, const vmp_const* rhs);

// Multiply two constant values and return the result
extern BOOL vmp_const_mult(vmp_const* lhs, const vmp_const* rhs);

// Divide two constant values and return the result
extern BOOL vmp_const_div(vmp_const* lhs, const vmp_const* rhs);

#endif
