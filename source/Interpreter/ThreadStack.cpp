#include "ThreadStack.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

ThreadStack::ThreadStack()
{
	_top = _blocks;
	_end = _blocks + VM_STACK_DEFAULT_SIZE;
#ifdef VM_STACK_DEBUG
	memset(_blocks, 0xFFFFFFFF, VM_STACK_DEFAULT_SIZE);
#endif
}

Byte* ThreadStack::Push(I32 size)
{
	char* top = _top;
	if (top + size >= _end)
		return NULL;
	_top += size;
#ifdef VM_STACK_DEBUG
	memset(top, 0xFFFFFFFF, (size_t)(_top - top));
#endif
	return top;
}

Byte* ThreadStack::Pop(I32 size)
{
	_top -= size;
	// Did we pop to much memory?
	if (_top < _blocks) {
		_top += size;
		return NULL;
	}
	return _top;
}
