#include "Compiler.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodePrimitive.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTree::SyntaxTree()
{
}

SyntaxTree::~SyntaxTree()
{
	for (auto&& n : _children)
		delete n;
}

void SyntaxTree::ToString(StringStream& s) const
{
	for (auto&& n : _children) {
		n->ToString(s, 0);
	}
}

void SyntaxTree::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	for (auto&& n : _children)
		if (!n->Visit(visitor))
			break;
}

void SyntaxTree::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	for (auto&& n : _children)
		if (!n->Visit(visitor))
			break;
}

ISyntaxTreeNodePackage* SyntaxTree::GetRootNode()
{
	assert(dynamic_cast<SyntaxTreeNodePackage*>(_children[0]) &&
		"The root node should not be anything else than a package");
	return _children[0];
}

void SyntaxTree::AddPackage(SyntaxTreeNodePackage* node)
{
	_children.push_back(node);
	node->SetSyntaxTree(this);
}

Compiler::Compiler()
	: _syntaxTree(new SyntaxTree())
{
	// Add root package
	auto root = new SyntaxTreeNodePackage(SourceCodeView(nullptr, nullptr), ReadOnlyString("<root>"));

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

	_syntaxTree->AddPackage(root);
}

Compiler::~Compiler()
{
	delete _syntaxTree;
}

bool Compiler::AddSourceCode(SourceCode* sourceCode)
{
	_sourceCodes.push_back(sourceCode);

	const Lexer lexer(sourceCode->GetText());
	Token token(&lexer);
	return ParseTokens(sourceCode, &token);
}

bool Compiler::ParseTokens(SourceCode* sourceCode, Token* t)
{
	ParserState state(this, t, sourceCode);

	while (true)
	{
		switch(t->GetType())
		{
		case TokenType::Package:
			SyntaxTreeNodePackage* const package = SyntaxTreeNodePackage::Parse(&state);
			_syntaxTree->AddPackage(package);
			break;
		
		}
		if (t->Is(TokenType::Package)) {

		}
	}

	return false;
}