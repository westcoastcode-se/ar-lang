#include "Compiler.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodePrimitive.h"

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
		_root->ToString(s, 0);
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

Byte* Compiler::Compile()
{
	// Start by resolving all references
	_syntaxTree->ResolveReferences();

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