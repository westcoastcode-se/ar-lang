#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeFuncArg.h"
#include "SyntaxTreeNodeFuncRet.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncDef::SyntaxTreeNodeFuncDef(SourceCodeView sourceCode, ReadOnlyString name)
	: _parent(nullptr), _sourceCode(sourceCode), _name(name), _body(nullptr)
{
}

SyntaxTreeNodeFuncDef::~SyntaxTreeNodeFuncDef()
{
	for (auto&& n : _children)
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

bool SyntaxTreeNodeFuncDef::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	if (!visitor->Visit(this))
		return false;
	for (auto&& n : _children)
		n->Visit(visitor);
	return true;
}

bool SyntaxTreeNodeFuncDef::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	if (!visitor->Visit(this))
		return false;
	for (auto&& n : _children)
		n->Visit(visitor);
	return true;
}

bool SyntaxTreeNodeFuncDef::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

ISyntaxTreeNodeFuncBody* SyntaxTreeNodeFuncDef::GetBody() const
{
	return _body;
}

void SyntaxTreeNodeFuncDef::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "FuncDef(name=" << _name << ")" << std::endl;
	for (auto&& i : _children) {
		i->ToString(s, indent + 1);
	}
}

void SyntaxTreeNodeFuncDef::AddNode(ISyntaxTreeNode* node)
{
	_children.push_back(node);
	node->SetParent(this);
}

void SyntaxTreeNodeFuncDef::SetBody(SyntaxTreeNodeFuncBody* body)
{
	assert(_body == nullptr &&
		"A function is only allowed to have one body");
	AddNode(body);
	_body = body;
}

void SyntaxTreeNodeFuncDef::AddArgument(ISyntaxTreeNodeFuncArg* arg)
{
	AddNode(arg);
	_arguments.push_back(arg);
}

void SyntaxTreeNodeFuncDef::AddReturn(ISyntaxTreeNodeFuncRet* ret)
{
	AddNode(ret);
	_returns.push_back(ret);
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

	// Parse return types. Paranteses are optional if you only have one return. 
	// You are also allowed to not have a return type at all.
	if (t->GetType() == TokenType::Identity)
		funcdef->AddReturn(SyntaxTreeNodeFuncRet::Parse(state));
	else if (t->GetType() == TokenType::ParantLeft)
	{
		while (t->Next() != TokenType::ParantRight)
		{
			funcdef->AddReturn(SyntaxTreeNodeFuncRet::Parse(state));

			// Ignore comma
			if (t->GetType() == TokenType::Comma) {
				t->Next();
			}
		}
	}

	return guard.Done();
}
