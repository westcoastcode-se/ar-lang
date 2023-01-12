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

	void AfterEach()
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
	Bytes bytecode;

	void BeforeEach()
	{
		UtilsBuilder::BeforeEach();
		bytecode = nullptr;
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

		if (bytecode)
		{
			free(bytecode);
			bytecode = nullptr;
		}

		UtilsBuilder::AfterEach();
	}

	void CompileAndInvoke(ReadOnlyString packageName, ReadOnlyString functionName)
	{
		// Generate the bytecode
		bytecode = linker->Link();

		// New process and load the bytecode
		process = new Process();
		process->Load(bytecode, false);

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

struct SuiteBuilderPrimitives : UtilsBuilderWithInterpreter
{
	template<typename T>
	void Ldc_T(T value)
	{
		auto primtiveType = GetPrimitiveType<T>();

		auto main = linker->AddPackage(new Builder::Package("Main"));
		auto add = main->Add(new Builder::Function("Get"));

		// ldc_<T> $value
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

	void operator()()
	{
		TEST(Ldc_T<I8>(123));
	}
};

// Run all builder tests
void SuiteBuilder()
{
	SUITE(SuiteBuilderPrimitives);
}
