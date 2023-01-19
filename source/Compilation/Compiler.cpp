#include "Compiler.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeOpBinop.h"
#include "SyntaxTreeNodeOpUnaryop.h"
#include "SyntaxTreeNodeOpTypeCast.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTree::SyntaxTree()
	: _root(SyntaxTreeNodeRoot::Create())
{
	_root->SetSyntaxTree(this);
}

SyntaxTree::~SyntaxTree()
{
	delete _root;
}

void SyntaxTree::ToString(StringStream& s) const
{
	if (_root)
		_root->ToString(s, 1);
}

void SyntaxTree::Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags)
{
	_root->Visit(visitor, flags);
}

SyntaxTreeNodeRoot* SyntaxTree::GetRootNode()
{
	return _root;
}

void SyntaxTree::ResolveReferences()
{
	_root->ResolveReferences();
}

void SyntaxTree::Compile(Builder::Linker* linker)
{
	_root->Compile(linker);
}

void SyntaxTree::Optimize(ISyntaxTreeNodeOptimizer* optimizer)
{
	_root->Optimize(optimizer);
}

Compiler::Compiler()
	: _syntaxTree(new SyntaxTree())
{
}

Compiler::~Compiler()
{
	delete _syntaxTree;
	for (auto& s : _sourceCodes)
		delete s;
}

SyntaxTree* Compiler::AddSourceCode(SourceCode* sourceCode)
{
	_sourceCodes.Add(sourceCode);

	const Lexer lexer(sourceCode->GetText());
	Token token(&lexer);
	ParseTokens(sourceCode, &token);
	return _syntaxTree;
}

Byte* Compiler::Compile(int optimizationLevel)
{
	// Start by resolving all references
	_syntaxTree->ResolveReferences();
	Optimize(optimizationLevel);

	// Link the bytecode together and return the bytecode
	auto linker = new Builder::Linker();
	try {
		_syntaxTree->Compile(linker);
		auto bytecode = linker->Link();
		delete linker;
		return bytecode;
	}
	catch (const std::exception&)
	{
		delete linker;
		throw;
	}
	
	return nullptr;
}

void Compiler::Optimize(int level)
{
	// Optimize the syntax tree
	if (level > 0) {
		// TODO: Add maximum iterations?
		while (true) {
			SyntaxTreeNodeOpBinop::Optimize0_Merge optimizer0_0;
			SyntaxTreeNodeOpUnaryop::Optimize0_Merge optimizer0_1;
			SyntaxTreeNodeOpTypeCast::Optimize0_Merge optimizer0_2;
			_syntaxTree->Optimize(&optimizer0_0);
			_syntaxTree->Optimize(&optimizer0_1);
			_syntaxTree->Optimize(&optimizer0_2);

			// We can't optimize the tree any more
			if (optimizer0_0.count == 0 && optimizer0_1.count == 0 && optimizer0_2.count == 0)
				break;
			optimizer0_0.count = 0;
			optimizer0_1.count = 0;
			optimizer0_2.count = 0;
		}
	}

	if (level > 1) {
		// TODO: Add inline optimzations for level = 2 and above if function complexity is lower than X
		// TODO: Merge scopes variable memory together
	}

	if (level > 2) {
		// TODO: Reuse same scope memory if variables are not used 
		// TODO: Create hidden functions where all code related to a return value is 
		//		 removed if the return values are discarded by the caller.
	}
}

ISyntaxTreeNodePrimitive* Compiler::FindPrimitive(ReadOnlyString name)
{
	return _syntaxTree->GetRootNode()->FindPrimitive(name);
}

void Compiler::ParseTokens(SourceCode* sourceCode, Token* t)
{
	const ParserState state(this, t, sourceCode, (SyntaxTreeNodePackage*)_syntaxTree->GetRootNode());

	while (true)
	{
		switch(t->GetType())
		{
		case TokenType::Package: {
			SyntaxTreeNodePackage::Parse(&state);
			break;
		}
		case TokenType::Eof:
			return;
		default:
			t->Next();
		}
	}
}
