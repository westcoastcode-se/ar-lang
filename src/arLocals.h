#ifndef _VMI_LOCALS_H_
#define _VMI_LOCALS_H_

#include "arconf.h"

// A local block containing memory for local variable data
struct arLocalsBlock
{
	arByte* memory;
	arByte* top;
	arByte* end;

	struct arLocalsBlock* prev;
	struct arLocalsBlock* next;
};
typedef struct arLocalsBlock arLocalsBlock;

// Local variables
struct arLocals
{
	arLocalsBlock* current;
	arLocalsBlock* free_block;
};
typedef struct arLocals arLocals;

// Initialize the locals container
extern BOOL arLocals_init(arLocals* l);

// Release locals memory
extern void arLocals_release(arLocals* l);

// Take the supplied number of bytes as local variables
extern arByte* arLocals_take(arLocals* l, arInt32 size);

// Give back local variable memory
extern BOOL arLocals_give_back(arLocals* l, arInt32 size);

#endif
