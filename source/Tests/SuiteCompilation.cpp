#include "Utils.hpp"

struct TestUtilsCompilation : TestUtils
{
	Compiler* compiler;
	SyntaxTree* syntaxTree;

	void BeforeEach()
	{
		MemoryTracker::Begin();
		compiler = new Compiler();
	}

	void AfterEach(const std::exception* e)
	{
		if (e != nullptr) {
			DebugSyntaxTree();
		}

		if (compiler)
		{
			delete compiler;
			compiler = nullptr;
		}

		AssertTrue(MemoryTracker::End());
	}

	void DebugSyntaxTree()
	{
		if (compiler == nullptr)
			return;
		StringStream ss;
		compiler->GetSyntaxTree()->ToString(ss);
		std::cerr << ss.str() << std::endl;
	}

	void AddSourceCode(SourceCode* sourceCode)
	{
		syntaxTree = compiler->AddSourceCode(sourceCode);
		AssertNotNull(syntaxTree);
	}

	String Format(const Char* fmt, ...)
	{
		Char bytes[4096];
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf(bytes, fmt, argptr);
		va_end(argptr);
		return String(bytes);
	}
};

struct TestUtilsCompilationWithInterpreter : TestUtilsCompilation
{
	Interpreter::Process* process = nullptr;
	Interpreter::Thread* thread = nullptr;
	int optimizationLevel;

	void BeforeEach()
	{
		TestUtilsCompilation::BeforeEach();
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

		TestUtilsCompilation::AfterEach(e);
	}

	void CompileAndInvoke(ReadOnlyString packageName, ReadOnlyString functionName)
	{
		// Generate the bytecode
		Byte* bytecode = compiler->Compile(optimizationLevel);

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


struct SuiteLexer : TestUtilsCompilation
{
	void VerifyToken(Token& t, TokenType type)
	{
		AssertEquals(t.Next(), type);
	}

	void VerifyUntilEoe(Token& t)
	{
		while (t.Next() == TokenType::Newline);
		AssertEquals(t.GetType(), TokenType::Eof);
	}

	void TestEof()
	{
		Lexer l("");
		Token t(&l);

		VerifyToken(t, TokenType::Eof);
	}

	void SimpleFunction()
	{
		Lexer l(R"(
func Calc(lhs int32, rhs int32) (int32) {
	return lhs + rhs
}
)");
		Token t(&l);

		VerifyToken(t, TokenType::Newline);
		VerifyToken(t, TokenType::Func);
		VerifyToken(t, TokenType::Identity);

		VerifyToken(t, TokenType::ParantLeft);
		VerifyToken(t, TokenType::Identity);
		VerifyToken(t, TokenType::Identity);
		VerifyToken(t, TokenType::Comma);
		VerifyToken(t, TokenType::Identity);
		VerifyToken(t, TokenType::Identity);
		VerifyToken(t, TokenType::ParantRight);

		VerifyToken(t, TokenType::ParantLeft);
		VerifyToken(t, TokenType::Identity);
		VerifyToken(t, TokenType::ParantRight);

		VerifyToken(t, TokenType::BracketLeft);
		VerifyToken(t, TokenType::Newline);
		VerifyToken(t, TokenType::Return);
		VerifyToken(t, TokenType::Identity);
		VerifyToken(t, TokenType::OpPlus);
		VerifyToken(t, TokenType::Identity);
		VerifyToken(t, TokenType::Newline);
		VerifyToken(t, TokenType::BracketRight);

		VerifyUntilEoe(t);
	}

	void NegativeValue()
	{
		Lexer l("-10");
		Token t(&l);

		VerifyToken(t, TokenType::OpMinus);
		VerifyToken(t, TokenType::Int);
	}

	void DecreaseNegativeValue()
	{
		Lexer l("---10");
		Token t(&l);

		VerifyToken(t, TokenType::OpDecrement);
		VerifyToken(t, TokenType::OpMinus);
		VerifyToken(t, TokenType::Int);
	}

	void operator()()
	{
		TEST(TestEof());
		TEST(SimpleFunction());
		TEST(NegativeValue());
		TEST(DecreaseNegativeValue());
	}
};

struct SuiteSyntaxTree : TestUtilsCompilation
{
	template<class T>
	class TypeVisitor : public ISyntaxTreeNodeVisitor
	{
	public:
		VisitorResult Visit(ISyntaxTreeNode* node) {
			if (dynamic_cast<T*>(node)) {
				nodes.Add(static_cast<T*>(node));
			}
			return VisitorResult::Continue;
		}

		Vector<T*> nodes;
	};

	void EmptySyntaxTree()
	{
		AddSourceCode(new SourceCode("", "main.arl"));

		TypeVisitor<ISyntaxTreeNodePackage> visitor;
		syntaxTree->Visit(&visitor, (I32)VisitFlag::IncludeChildren);

		AssertEquals(visitor.nodes.Size(), 1);

		auto package = visitor.nodes[0];
		AssertEquals(package->GetName(), ReadOnlyString("<root>"));

		auto primitives = package->GetChildren();
		AssertEquals(primitives.Size(), 27);
	}

	void EmptyPackage()
	{
		AddSourceCode(new SourceCode(R"(
package Main
)", "main.arl"));

		TypeVisitor<ISyntaxTreeNodePackage> visitor;
		syntaxTree->Visit(&visitor, (I32)VisitFlag::IncludeChildren);

		AssertEquals(visitor.nodes.Size(), 2);

		AssertEquals(visitor.nodes[0]->GetName(), ReadOnlyString("<root>"));
		AssertEquals(visitor.nodes[1]->GetName(), ReadOnlyString("Main"));

		// All packages have an import to the root package by default
		auto children = visitor.nodes[1]->GetChildren();
		AssertEquals(children.Size(), 1);
		AssertType<ISyntaxTreeNodeImport>(children[0]);
	}

	void PackageInPackage()
	{
		AddSourceCode(new SourceCode(R"(
package WestCoastCode.Game
package WestCoastCode.Game.Stuff
)", "main.arl"));

		TypeVisitor<ISyntaxTreeNodePackage> visitor;
		syntaxTree->Visit(&visitor, (I32)VisitFlag::IncludeChildren);

		AssertEquals(visitor.nodes.Size(), 4);

		AssertEquals(visitor.nodes[0]->GetName(), ReadOnlyString("<root>"));
		AssertEquals(visitor.nodes[1]->GetName(), ReadOnlyString("WestCoastCode"));
		AssertEquals(visitor.nodes[2]->GetName(), ReadOnlyString("Game"));
		AssertEquals(visitor.nodes[3]->GetName(), ReadOnlyString("Stuff"));
	}

	void FunctionInPackage()
	{
		AddSourceCode(new SourceCode(R"(
package WestCoastCode

func Main() int32 {
	return 123
}
)", "main.arl"));

		TypeVisitor<ISyntaxTreeNodeFuncDef> visitor;
		syntaxTree->Visit(&visitor, (I32)VisitFlag::IncludeChildren);

		AssertEquals(visitor.nodes.Size(), 1);
		auto func = visitor.nodes[0];

		AssertType<ISyntaxTreeNodePackage>(func->GetParent());
		AssertEquals(func->GetName(), ReadOnlyString("Main"));
		AssertTrue(func->GetArguments().IsEmpty());

		auto returnType = func->GetReturnType();
		AssertNotNull(returnType);
		AssertType<ISyntaxTreeNodeType>(returnType);

		auto body = func->GetBody();
		AssertNotNull(body);

		AssertType<ISyntaxTreeNodeScope>(func->GetBody()->GetChildren()[0]);
	}

	void FunctionWithSpecializedReturn()
	{
		AddSourceCode(new SourceCode(R"(
package WestCoastCode

func Main() Engine.Graphics.Value {
	return 123
}
)", "main.arl"));

		TypeVisitor<ISyntaxTreeNodeFuncDef> visitor;
		syntaxTree->Visit(&visitor, (I32)VisitFlag::IncludeChildren);

		AssertEquals(visitor.nodes.Size(), 1);
		auto func = visitor.nodes[0];

		AssertType<ISyntaxTreeNodePackage>(func->GetParent());
		AssertEquals(func->GetName(), ReadOnlyString("Main"));
		AssertTrue(func->GetArguments().IsEmpty());

		auto returnType = func->GetReturnType();
		AssertNotNull(returnType);
		AssertType<ISyntaxTreeNodeType>(returnType);
	}

	void operator()()
	{
		TEST(EmptySyntaxTree());
		TEST(EmptyPackage());
		TEST(PackageInPackage());
		TEST(FunctionInPackage());
		TEST(FunctionWithSpecializedReturn());
	}
};

struct SuiteCompilationError
{
	void operator()()
	{
		// TODO: Verify errors raised when parsing source code
	}
};

struct SuiteCompileAndThenInterpret : TestUtilsCompilationWithInterpreter
{
	template<typename T>
	void Constant_T(T val)
	{
		AddSourceCode(new SourceCode(Format(R"(
package Main

func Get() %s {
	return (%s)%lld
}
)", Name<T>(), Name<T>(), (I64)val), "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(T));
		AssertEquals(Pop<T>(), val);
	}

	void Constant_U64(U64 val)
	{
		AddSourceCode(new SourceCode(Format(R"(
package Main

func Get() uint64 {
	return (uint64)%llu
}
)", val), "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(U64));
		AssertEquals(Pop<U64>(), val);
	}

	void Constant_F32_Cast(F32 val)
	{
		AddSourceCode(new SourceCode(Format(R"(
package Main

func Get() float32 {
	return (float32)%f
}
)", val), "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(F32));
		AssertEquals(Pop<F32>(), val);
	}

	void Constant_F32_Suffix(F32 val)
	{
		AddSourceCode(new SourceCode(Format(R"(
package Main

func Get() float32 {
	return %ff
}
)", val), "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(F32));
		AssertEquals(Pop<F32>(), val);
	}

	void Constant_F64(F64 val)
	{
		AddSourceCode(new SourceCode(Format(R"(
package Main

func Get() float64 {
	return (float64)%f
}
)", val), "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(F64));
		AssertEquals(Pop<F64>(), val);
	}

	void Add_I32()
	{
		AddSourceCode(new SourceCode(R"(
package Main

func Get() int32 {
	return 10 + 123
}
)", "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(I32));
		AssertEquals(Pop<I32>(), 133);
	}

	void Add_I64()
	{
		AddSourceCode(new SourceCode(R"(
package Main

func Get() int64 {
	return (int64)(10 + 123)
}
)", "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(I64));
		AssertEquals(Pop<I64>(), (I64)133);
	}

	template<typename T>
	void BitNot_T(T val)
	{
		AddSourceCode(new SourceCode(Format(R"(
package Main

func Get() %s {
	return ~(%s)%lld
}
)", Name<T>(), Name<T>(), (I64)val), "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(T));
		AssertEquals(Pop<T>(), (T)(~(T)val));
	}

	void BitNot_U64(U64 val)
	{
		AddSourceCode(new SourceCode(Format(R"(
package Main

func Get() uint64 {
	return ~(uint64)%lluu
}
)", (U64)val), "main.arl"));

		// Compile the source code
		CompileAndInvoke("Get()");

		VerifyStackSize(sizeof(U64));
		AssertEquals(Pop<U64>(), ~(U64)val);
	}

	void operator()(int level)
	{
		// Set the optimization level
		optimizationLevel = level;

		TEST(Constant_T<I8>(-10));
		TEST(Constant_T<I8>(122));
		TEST(Constant_T<U8>(200));
		TEST(Constant_T<I16>(-1234));
		TEST(Constant_T<I16>(INT16_MAX));
		TEST(Constant_T<U16>(20000));
		TEST(Constant_T<I32>(-1));
		TEST(Constant_T<I32>(123));
		TEST(Constant_T<U32>(UINT32_MAX - 100));
		TEST(Constant_T<I64>(-1234512));
		TEST(Constant_T<I64>(123412432));
		TEST(Constant_U64(UINT64_MAX - 100));
		TEST(Constant_F32_Cast(1.23f));
		TEST(Constant_F32_Cast(-1.23f));
		TEST(Constant_F32_Suffix(1.23f));
		TEST(Constant_F32_Suffix(-1.23f));
		TEST(Constant_F64(123.456));
		TEST(Constant_F64(-123.456));

		TEST(Add_I32());
		TEST(Add_I64());
		
		TEST(BitNot_T<I8>(10));
		TEST(BitNot_T<U8>(200));
		TEST(BitNot_T<I16>(123));
		TEST(BitNot_T<U16>(INT16_MAX + 100));
		TEST(BitNot_T<I32>(52344));
		TEST(BitNot_T<I32>(-52344));
		TEST(BitNot_T<U32>(INT32_MAX + (U32)100));
		TEST(BitNot_T<I64>(-124532));
		TEST(BitNot_U64(9223372036854775807ui64 + 1ui64));
	}
};

// Run all compilation tests
void SuiteCompilation()
{
	SUITE(SuiteLexer);
	SUITE(SuiteSyntaxTree);
	SUITE(SuiteCompilationError);
	SUITE_RANGE(SuiteCompileAndThenInterpret, 0, 4);
}
