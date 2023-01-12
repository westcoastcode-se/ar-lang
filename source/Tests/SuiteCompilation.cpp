#include "Utils.hpp"

struct TestUtilsCompilation : TestUtils
{
	void BeforeEach()
	{}

	void AfterEach()
	{}
};

struct TestUtilsCompilationWithInterpreter : TestUtilsCompilation
{
	Interpreter::Process* process = nullptr;
	Interpreter::Thread* thread = nullptr;

	void BeforeEach()
	{
		TestUtilsCompilation::BeforeEach();
		process = nullptr;
		thread = nullptr;
	}

	void AfterEach()
	{
		if (thread)
		{
			delete thread;
			thread = nullptr;
		}

		if (process)
		{
			delete process;
			process = nullptr;
		}

		TestUtilsCompilation::AfterEach();
	}

	void CompileAndInvoke(ReadOnlyString packageName, ReadOnlyString functionName)
	{
		// Generate the bytecode
		Byte* bytecode = nullptr;

		// New process and load the bytecode
		process = new Process();
		process->Load(bytecode);

		// Search for the function
		auto package = process->FindPackage(packageName);
		if (package == nullptr)
			Throw(Error() << "could not find package '" << packageName << "'");
		auto function = package->FindFunction(functionName);
		if (function == nullptr)
			Throw(Error() << "could not find function '" << functionName << "'");

		thread = process->NewThread();
		process->Exec(thread, function);
	}

	void CompileAndInvoke(ReadOnlyString func)
	{
		CompileAndInvoke("Main", func);
	}

	void VerifyStackSize(I32 expected)
	{
		const I32 size = thread->GetStack().GetSize();
		if (expected != size) {
			Throw(Error() << "expected stack size to be " << expected << " but was " << size);
		}
	}

	template<typename T>
	T Pop() {
		return *(T*)thread->PopStack(sizeof(T));
	}

	template<typename T, int SIZE>
	vector<T> Pop()
	{
		T* arr = (T*)thread->PopStack(sizeof(T[SIZE]));
		return std::vector<T>(arr, arr + SIZE);
	}
};

// Run all compilation tests
void SuiteCompilation()
{
}
