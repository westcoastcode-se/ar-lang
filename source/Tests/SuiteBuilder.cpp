#include "Utils.hpp"

struct UtilsBuilder : TestUtils
{
	Linker* linker = nullptr;
	Builder::Package* root = nullptr;
	unordered_map<String, Builder::Type*> types;
	unordered_map<String, Builder::Package*> packages;

	void BeforeEach()
	{
		linker = new Linker();
		packages["<root>"] = root = linker->AddPackage(new Builder::Package(""));
		types["int8"] = root->Add(new Builder::Type("int8", sizeof(I8), 0, PrimitiveType::I8));
		types["uint8"] = root->Add(new Builder::Type("uint8", sizeof(U8), 0, PrimitiveType::U8));
		types["int16"] = root->Add(new Builder::Type("int16", sizeof(I16), 0, PrimitiveType::I16));
		types["uint16"] = root->Add(new Builder::Type("uint16", sizeof(U16), 0, PrimitiveType::U16));
		types["int32"] = root->Add(new Builder::Type("int32", sizeof(I32), 0, PrimitiveType::I32));
		types["uint32"] = root->Add(new Builder::Type("uint32", sizeof(U32), 0, PrimitiveType::U32));
		types["int64"] = root->Add(new Builder::Type("int64", sizeof(I64), 0, PrimitiveType::I64));
		types["uint64"] = root->Add(new Builder::Type("uint64", sizeof(U64), 0, PrimitiveType::U64));
		types["float32"] = root->Add(new Builder::Type("float32", sizeof(F32), 0, PrimitiveType::F32));
		types["float64"] = root->Add(new Builder::Type("float64", sizeof(F64), 0, PrimitiveType::F64));
		types["bool"] = root->Add(new Builder::Type("bool", sizeof(IB), 0, PrimitiveType::Bool));
	}

	void AfterEach(const std::exception* e)
	{
		if (linker != NULL) {
			delete linker;
			linker = nullptr;
		}
		types.clear();
		packages.clear();
	}

	template<typename T>
	Builder::Type* GetPrimitiveType()
	{
		return GetPrimitiveType(Name<T>());
	}

	Builder::Type* GetPrimitiveType(const char* name)
	{
		return types[String(name)];
	}
};

struct UtilsBuilderWithInterpreter : UtilsBuilder
{
	Interpreter::Process* process = nullptr;
	Interpreter::Thread* thread = nullptr;

	void BeforeEach()
	{
		UtilsBuilder::BeforeEach();
		process = nullptr;
		thread = nullptr;
	}

	void AfterEach(const std::exception* e)
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

		UtilsBuilder::AfterEach(e);
	}

	void CompileAndInvoke(ReadOnlyString packageName, ReadOnlyString functionName)
	{
		// Generate the bytecode
		Byte* bytecode = linker->Link();

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

struct Constants : UtilsBuilderWithInterpreter
{
	template<typename T>
	void Ldc_s_T(T value)
	{
		auto primtiveType = GetPrimitiveType<T>();

		auto main = linker->AddPackage(new Builder::Package("Main"));
		auto add = main->Add(new Builder::Function("Get"));
		add->AddReturn(GetPrimitiveType<T>());

		// ldc_s <T> $value
		// ret

		auto& instr = add->Begin();
		instr.Ldc(primtiveType, Const((T)value));
		instr.Ret();
		instr.End();

		CompileAndInvoke("Get()");
		VerifyStackSize(sizeof(T));
		VerifyStackSize(sizeof(T));
		AssertEquals(Pop<T>(), (T)value);
	}

	template<typename T>
	void Ldc_T(T value)
	{
		auto main = linker->AddPackage(new Builder::Package("Main"));
		auto add = main->Add(new Builder::Function("Get"));
		add->AddReturn(GetPrimitiveType<T>());

		// ldc <T> $value
		// ret

		auto& instr = add->Begin();
		instr.Ldc(GetPrimitiveType<T>(), Const((T)value));
		instr.Ret();		
		instr.End();

		CompileAndInvoke("Get()");
		VerifyStackSize(sizeof(T));
		VerifyStackSize(sizeof(T));
		AssertEquals(Pop<T>(), (T)value);
	}

	template<typename T>
	void Ldc_l_T(T value)
	{
		auto main = linker->AddPackage(new Builder::Package("Main"));
		auto add = main->Add(new Builder::Function("Get"));
		add->AddReturn(GetPrimitiveType<T>());

		// ldc_l <T> $value
		// ret

		auto& instr = add->Begin();
		instr.Ldc(GetPrimitiveType<T>(), Const((T)value));
		instr.Ret();
		instr.End();

		CompileAndInvoke("Get()");
		VerifyStackSize(sizeof(T));
		VerifyStackSize(sizeof(T));
		AssertEquals(Pop<T>(), (T)value);
	}

	void operator()()
	{
		// Test shorthand constants
		TEST(Ldc_s_T<I8>(0));
		TEST(Ldc_s_T<I8>(1));
		TEST(Ldc_s_T<U8>(0));
		TEST(Ldc_s_T<U8>(1));
		TEST(Ldc_s_T<I16>(0));
		TEST(Ldc_s_T<I16>(1));
		TEST(Ldc_s_T<U16>(0));
		TEST(Ldc_s_T<U16>(1));

		// Test shorthand non-specialized constants
		TEST(Ldc_s_T<I8>(-122));
		TEST(Ldc_s_T<I8>(57));
		TEST(Ldc_s_T<U8>(255));
		TEST(Ldc_s_T<I16>(-1221));
		TEST(Ldc_s_T<U16>(UINT16_MAX));

		// Constants (32bit)
		TEST(Ldc_T<I32>(-12345));
		TEST(Ldc_T<U32>((U32)INT32_MAX + (U32)100));
		TEST(Ldc_T<F32>(-123.45f));

		// Constants (64bit)
		TEST(Ldc_l_T<I64>((I64)UINT32_MAX + (I64)100));
		TEST(Ldc_l_T<U64>((U64)INT64_MAX + (U64)100));
		TEST(Ldc_l_T<F64>(123.4315));
	}
};

struct Errors : UtilsBuilderWithInterpreter
{
	void HaltOnPushingTooMuchBeforeReturn()
	{
		auto main = linker->AddPackage(new Builder::Package("Main"));
		auto add = main->Add(new Builder::Function("Get"));

		// ldc <T> $value
		// ret

		auto& instr = add->Begin();
		instr.Ldc(GetPrimitiveType<I32>(), Const(100));
		instr.Ret();
		instr.End();

		try {
			CompileAndInvoke("Get()");
			Throw(Error() << "expected an exception to be thrown");
		}
		catch (const ThreadErrorHaltedExecution&) {
		}
	}

	void HaltOnPushingTooLittleBeforeExec()
	{
		auto main = linker->AddPackage(new Builder::Package("Main"));
		auto add = main->Add(new Builder::Function("Get"));
		add->AddArgument(GetPrimitiveType<I32>());

		// ldc <T> $value
		// ret

		auto& instr = add->Begin();
		instr.Ldc(GetPrimitiveType<I32>(), Const(100));
		instr.Ret();
		instr.End();

		try {
			CompileAndInvoke("Get(int32)");
			Throw(Error() << "expected an exception to be thrown");
		}
		catch (const ThreadErrorStackMismanaged& te) {
			AssertEquals(te.expected, (I32)sizeof(I32));
			AssertEquals(te.currentSize, 0);
		}

	}

	void operator()()
	{
		TEST(HaltOnPushingTooMuchBeforeReturn());
		TEST(HaltOnPushingTooLittleBeforeExec());
		// TODO: HaltOnExecutingOutsideMemory
		// TODO: HaltOnWritingInsideExecutionMemory
	}
};

// Run all builder tests
void SuiteBuilder()
{
	SUITE(Constants);
	SUITE(Errors)
}
