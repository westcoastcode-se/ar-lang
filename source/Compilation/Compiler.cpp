#include "Compiler.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodePrimitive.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTree::SyntaxTree()
	: _root(nullptr)
{
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

ISyntaxTreeNodePackage* SyntaxTree::GetRootNode()
{
	assert(dynamic_cast<SyntaxTreeNodePackage*>(_root) &&
		"The root node should not be anything else than a package");
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

void SyntaxTree::SetRootPackage(SyntaxTreeNodePackage* node)
{
	_root = node;
	node->SetSyntaxTree(this);
}

Compiler::Compiler()
	: _syntaxTree(new SyntaxTree())
{
	// Add root package
	auto root = new SyntaxTreeNodePackage(SourceCodeView(), ReadOnlyString("<root>"));

	auto voidPtr = new SyntaxTreeNodePrimitive(root, sizeof(void*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*void"));
	root->AddNode(voidPtr);

	auto i = new SyntaxTreeNodePrimitive(root, sizeof(IB), Interpreter::PrimitiveType::Bool, ReadOnlyString("bool"));
	root->AddNode(i);
	auto ip = new SyntaxTreeNodePrimitive(root, sizeof(IB*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*bool"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I8), Interpreter::PrimitiveType::I8, ReadOnlyString("int8"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I8*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int8"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U8), Interpreter::PrimitiveType::U8, ReadOnlyString("uint8"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U8*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint8"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I16), Interpreter::PrimitiveType::I16, ReadOnlyString("int16"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I16*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int16"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U16), Interpreter::PrimitiveType::U16, ReadOnlyString("uint16"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U16*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint16"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I32), Interpreter::PrimitiveType::I32, ReadOnlyString("int32"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I32*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int32"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U32), Interpreter::PrimitiveType::U32, ReadOnlyString("uint32"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U32*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint32"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I64), Interpreter::PrimitiveType::I64, ReadOnlyString("int64"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I64*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int64"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U64), Interpreter::PrimitiveType::U64, ReadOnlyString("uint64"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U64*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint64"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(F32), Interpreter::PrimitiveType::F32, ReadOnlyString("float32"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(F32*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*float32"), voidPtr, i);
	root->AddNode(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(F64), Interpreter::PrimitiveType::F64, ReadOnlyString("float64"));
	root->AddNode(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(F64*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*float64"), voidPtr, i);
	root->AddNode(ip);

	_syntaxTree->SetRootPackage(root);
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
