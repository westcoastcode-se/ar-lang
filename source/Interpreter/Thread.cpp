#include "Thread.h"
#include "Process.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

Thread::Thread(Process* process)
	: _process(process), _bytecode(process->GetBytecode())
{
}

Thread::~Thread()
{
}

void Thread::ExecEntrypoint(const Function* entrypoint)
{

}
