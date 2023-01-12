#include "ThreadError.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

ThreadErrorStackMismanaged::ThreadErrorStackMismanaged(I32 expected, I32 current_size)
	: ThreadError(ThreadErrorType::StackMismanaged)
{
	StringStream s;
	s << "the stack is mismanaged. expected " << expected << " bytes but was " << current_size;
	SetError(s.str());
}

ThreadErrorHaltedExecution::ThreadErrorHaltedExecution(I32 flags, const Char* message)
	: ThreadError(ThreadErrorType::HaltedExecution)
{
	StringStream s;
	s << "thread execution halted due to '" << message << "' with flags '" << flags << "' raised";
	SetError(s.str());
}

ThreadErrorExecutionOutsideMemory::ThreadErrorExecutionOutsideMemory()
	: ThreadError(ThreadErrorType::ExecutionOutsideMemory)
{
	StringStream s;
	s << "thread execution halted because execution is being done outside memory";
	SetError(s.str());
}
