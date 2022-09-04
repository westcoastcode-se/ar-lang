#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_ldc_i1(arThread* t, arIP ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	arInt8* result = (arInt8*)arStack_push(&t->stack, sizeof(arInt8));
	*result = instr->i8;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC <i8> %d", instr->i8);
#endif
	return ip + sizeof(vmi_instr_ldc_i32);
}

arIP _arThread_ldc_i2(arThread* t, arIP ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	arInt16* result = (arInt16*)arStack_push(&t->stack, sizeof(arInt16));
	*result = instr->i16;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC <i16> %d", instr->i16);
#endif
	return ip + sizeof(vmi_instr_ldc_i32);
}

arIP _arThread_ldc_i4(arThread* t, arIP ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	arInt32* result = (arInt32*)arStack_push(&t->stack, sizeof(arInt32));
	*result = instr->i32;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC <i32> %d", instr->i32);
#endif
	return ip + sizeof(vmi_instr_ldc_i32);
}

arIP _arThread_ldc_i8(arThread* t, arIP ip)
{
	const vmi_instr_ldc_i64* instr = (const vmi_instr_ldc_i64*)ip;
	arInt64* result = (arInt64*)arStack_push(&t->stack, sizeof(arInt64));
	*result = instr->i64;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC <i64> %lld", instr->i64);
#endif
	return ip + sizeof(vmi_instr_ldc_i64);
}

arIP _arThread_ldc_f4(arThread* t, arIP ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	arFloat32* result = (arFloat32*)arStack_push(&t->stack, sizeof(arFloat32));
	*result = instr->f32;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC <f32> %f", instr->f32);
#endif
	return ip + sizeof(vmi_instr_ldc_i32);
}

arIP _arThread_ldc_f8(arThread* t, arIP ip)
{
	const vmi_instr_ldc_i64* instr = (const vmi_instr_ldc_i64*)ip;
	arFloat64* result = (arFloat64*)arStack_push(&t->stack, sizeof(arFloat64));
	*result = instr->f64;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC <f64> %lf", instr->f64);
#endif
	return ip + sizeof(vmi_instr_ldc_i64);
}

arIP _arThread_ldc_s_i1(arThread* t, arIP ip, arInt8 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	arInt8* result = (arInt8*)arStack_push(&t->stack, sizeof(arInt8));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

arIP _arThread_ldc_s_i2(arThread* t, arIP ip, arInt16 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	arInt16* result = (arInt16*)arStack_push(&t->stack, sizeof(arInt16));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

arIP _arThread_ldc_s_i4(arThread* t, arIP ip, arInt32 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	arInt32* result = (arInt32*)arStack_push(&t->stack, sizeof(arInt32));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

arIP _arThread_ldc_s_i8(arThread* t, arIP ip, arInt64 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	arInt64* result = (arInt64*)arStack_push(&t->stack, sizeof(arInt64));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

arIP _arThread_ldc_s_f4(arThread* t, arIP ip, arFloat32 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	arFloat32* result = (arFloat32*)arStack_push(&t->stack, sizeof(arFloat32));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

arIP _arThread_ldc_s_f8(arThread* t, arIP ip, arFloat64 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	arFloat64* result = (arFloat64*)arStack_push(&t->stack, sizeof(arFloat64));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}
