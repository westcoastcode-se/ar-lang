#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeFuncArg.h"
#include "SyntaxTreeNodeTypeRef.h"
#include "SyntaxTreeNodeTypes.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncDef::SyntaxTreeNodeFuncDef(SourceCodeView sourceCode, ReadOnlyString name)
	: _parent(nullptr), _sourceCode(sourceCode), _name(name), _returns(nullptr), _body(nullptr), _symbol(nullptr)
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

ReadOnlyArray<ISyntaxTreeNode*> SyntaxTreeNodeFuncDef::GetReturns() const
{
	if (_returns)
		return _returns->GetChildren();
	return ReadOnlyArray<ISyntaxTreeNode*>();
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
		if (dynamic_cast<SyntaxTreeNodePackage*>(_parent)) {
			auto symbol = static_cast<SyntaxTreeNodePackage*>(_parent)->GetSymbol();
			symbol->Add(_symbol);
		}
	}
	Default::Compile(this, linker);
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
	_body->SetFunction(this);
}

void SyntaxTreeNodeFuncDef::AddArgument(ISyntaxTreeNodeFuncArg* arg)
{
	AddNode(arg);
	_arguments.Add(arg);
}

void SyntaxTreeNodeFuncDef::SetReturns(ISyntaxTreeNodeTypes* ret)
{
	AddNode(ret);
	_returns = ret;
}

SyntaxTreeNodeFuncDef* SyntaxTreeNodeFuncDef::Parse(ParserState* state)
{
	Token* const t = state->token;
	if (t->Next() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Function
	auto funcdef = new SyntaxTreeNodeFuncDef(SourceCodeView(state->sourceCode, t), t->GetString());
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
	funcdef->SetReturns(SyntaxTreeNodeTypes::Parse(state));
	return guard.Done();
}
