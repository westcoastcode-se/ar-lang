#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeFuncArg.h"
#include "SyntaxTreeNodeTypeRef.h"
#include "SyntaxTreeNodeMultiType.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodePrimitive.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncDef::SyntaxTreeNodeFuncDef(SourceCodeView sourceCode, ReadOnlyString name)
	: _parent(nullptr), _sourceCode(sourceCode), _name(name), _returnType(nullptr), _body(nullptr), _symbol(nullptr)
{
}

SyntaxTreeNodeFuncDef::~SyntaxTreeNodeFuncDef()
{
	for (auto n : _children)
		delete n;
}

ISyntaxTreeNode* SyntaxTreeNodeFuncDef::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeFuncDef::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

ISyntaxTreeNodeFuncBody* SyntaxTreeNodeFuncDef::GetBody() const
{
	return _body;
}

void SyntaxTreeNodeFuncDef::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "FuncDef(name=" << _name << ")" << std::endl;
	for (auto i : _children) {
		i->ToString(s, indent + 1);
	}
}

void SyntaxTreeNodeFuncDef::Compile(Builder::Linker* linker)
{
	if (_symbol == nullptr) {
		_symbol = new Builder::Function(_name);

		// TODO: Add support for functions inside another function
		auto package = GetPackage();
		auto symbol = static_cast<SyntaxTreeNodePackage*>(package)->GetSymbol();
		symbol->Add(_symbol);
	}

	// Compile children so that we have valid symbols for arguments and return types
	for (auto child : _children)
		child->Compile(linker);

	// Attach return types
	if (dynamic_cast<ISyntaxTreeNodeTypeRef*>(_returnType)) {
		auto definitions =
			static_cast<ISyntaxTreeNodeTypeRef*>(_returnType)->GetDefinitions();
		auto definition = dynamic_cast<SyntaxTreeNodePrimitive*>(definitions[0]);
		_symbol->AddReturn(definition->GetSymbol());
	}
	else {

	}
}

ISyntaxTreeNodePackage* SyntaxTreeNodeFuncDef::GetPackage()
{
	// TODO: Add support for functions inside another function
	return dynamic_cast<SyntaxTreeNodePackage*>(_parent);
}

void SyntaxTreeNodeFuncDef::AddNode(ISyntaxTreeNode* node)
{
	_children.Add(node);
	node->SetParent(this);
}

void SyntaxTreeNodeFuncDef::SetBody(SyntaxTreeNodeFuncBody* body)
{
	assert(_body == nullptr &&
		"A function is only allowed to have one body");
	_body = body;
}

void SyntaxTreeNodeFuncDef::AddArgument(ISyntaxTreeNodeFuncArg* arg)
{
	AddNode(arg);
	_arguments.Add(arg);
}

void SyntaxTreeNodeFuncDef::SetReturnType(ISyntaxTreeNodeType* returnType)
{
	AddNode(returnType);
	_returnType = returnType;
}

SyntaxTreeNodeFuncDef* SyntaxTreeNodeFuncDef::Parse(ParserState* state)
{
	Token* const t = state->token;
	if (t->Next() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Function
	auto funcdef = ARLANG_NEW SyntaxTreeNodeFuncDef(SourceCodeView(state->sourceCode, t), t->GetString());
	auto guard = MemoryGuard(funcdef);

	// Function arguments list has to start with a (
	if (t->Next() != TokenType::ParantLeft)
		throw ParseErrorSyntaxError(state, "expected '('");
	
	// Parse all arguments: (name type[, ...])
	while (t->Next() != TokenType::ParantRight)
	{
		funcdef->AddArgument(SyntaxTreeNodeFuncArg::Parse(state));

		// Ignore comma
		if (t->GetType() == TokenType::Comma) {
			t->Next();
		}
	}
	t->Next();

	// Parse return types. Paranteses are optional if you only have one return. 
	// You are also allowed to not have a return type at all.
	if (t->GetType() == TokenType::Identity)
		funcdef->SetReturnType(SyntaxTreeNodeTypeRef::Parse(state));
	else if (t->GetType() == TokenType::ParantLeft)
		funcdef->SetReturnType(SyntaxTreeNodeMultiType::Parse(state));
	return guard.Done();
}
