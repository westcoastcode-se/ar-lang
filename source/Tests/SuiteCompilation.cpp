#include "Utils.hpp"

struct TestUtilsCompilation : TestUtils
{
	Compiler* compiler;
	SyntaxTree* syntaxTree;

	void BeforeEach()
	{
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
	}

	void DebugSyntaxTree()
	{
		StringStream ss;
		compiler->GetSyntaxTree()->ToString(ss);
		std::cerr << ss.str() << std::endl;
	}

	void AddSourceCode(SourceCode* sourceCode)
	{
		syntaxTree = compiler->AddSourceCode(sourceCode);
		AssertNotNull(syntaxTree);
	}
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


struct SuiteLexer : TestUtilsCompilation
{
	void BeforeEach()
	{
	}

	void AfterEach(const std::exception* e)
	{
	}

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

		VerifyToken(t, TokenType::Int);
		AssertEquals(t.GetModifier(), TokenModifier::Negative);
	}

	void DecreaseNegativeValue()
	{
		Lexer l("---10");
		Token t(&l);

		VerifyToken(t, TokenType::OpDecrement);
		VerifyToken(t, TokenType::Int);
		AssertEquals(t.GetModifier(), TokenModifier::Negative);
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
	class TypeVisitor : public ISyntaxTreeNodeVisitor<ISyntaxTreeNode>
	{
	public:
		VisitorResult Visit(Node* node) {
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
		AssertEquals(primitives.Size(), 23);
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

		auto returns = func->GetReturns();
		AssertEquals(returns.Size(), 1);
		AssertNotNull(returns[0]->GetReturnType());

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

		auto returns = func->GetReturns();
		AssertEquals(returns.Size(), 1);
		AssertNotNull(returns[0]->GetReturnType());

		DebugSyntaxTree();
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

// Run all compilation tests
void SuiteCompilation()
{
	SUITE(SuiteLexer);
	SUITE(SuiteSyntaxTree);
}
