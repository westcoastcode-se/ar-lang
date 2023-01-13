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
			StringStream ss;
			syntaxTree->ToString(ss);
			std::cerr << ss.str() << std::endl;
		}

		if (compiler)
		{
			delete compiler;
			compiler = nullptr;
		}
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

struct SuiteSyntaxTree : TestUtilsCompilation
{
	template<class T>
	class TypeVisitor : public ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>
	{
	public:
		bool Visit(Node* node) {
			if (dynamic_cast<const T*>(node)) {
				nodes.push_back(static_cast<const T*>(node));
			}
			return true;
		}

		Vector<const T*> nodes;
	};

	void EmptySyntaxTree()
	{
		AddSourceCode(new SourceCode("", "main.arl"));

		TypeVisitor<ISyntaxTreeNodePackage> visitor;
		syntaxTree->Visit(&visitor);

		AssertEquals((int)visitor.nodes.size(), 1);

		auto package = visitor.nodes[0];
		AssertEquals(package->GetName(), ReadOnlyString("<root>"));

		auto primitives = package->GetChildren();
		AssertEquals((int)primitives.size(), 23);
	}

	void EmptyPackage()
	{
		AddSourceCode(new SourceCode(R"(
package Main
)", "main.arl"));

		TypeVisitor<ISyntaxTreeNodePackage> visitor;
		syntaxTree->Visit(&visitor);

		AssertEquals((int)visitor.nodes.size(), 2);

		AssertEquals(visitor.nodes[0]->GetName(), ReadOnlyString("<root>"));
		AssertEquals(visitor.nodes[1]->GetName(), ReadOnlyString("Main"));

		// All packages have an import to the root package by default
		auto children = visitor.nodes[1]->GetChildren();
		AssertEquals((int)children.size(), 1);
		AssertType<ISyntaxTreeNodeImport>(*children.begin());
	}

	void PackageInPackage()
	{
		AddSourceCode(new SourceCode(R"(
package WestCoastCode.Game
package WestCoastCode.Game.Stuff
)", "main.arl"));

		TypeVisitor<ISyntaxTreeNodePackage> visitor;
		syntaxTree->Visit(&visitor);

		AssertEquals((int)visitor.nodes.size(), 4);

		AssertEquals(visitor.nodes[0]->GetName(), ReadOnlyString("<root>"));
		AssertEquals(visitor.nodes[1]->GetName(), ReadOnlyString("WestCoastCode"));
		AssertEquals(visitor.nodes[2]->GetName(), ReadOnlyString("Game"));
		AssertEquals(visitor.nodes[3]->GetName(), ReadOnlyString("Stuff"));
	}

	void operator()()
	{
		TEST(EmptySyntaxTree());
		TEST(EmptyPackage());
		TEST(PackageInPackage());
	}
};

// Run all compilation tests
void SuiteCompilation()
{
	SUITE(SuiteSyntaxTree);
}
