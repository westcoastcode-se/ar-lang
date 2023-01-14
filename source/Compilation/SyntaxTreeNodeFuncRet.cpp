#include "SyntaxTreeNodeFuncRet.h"
#include "SyntaxTreeNodeTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncRet::SyntaxTreeNodeFuncRet(SourceCodeView sourceCode)
	: _parent(nullptr), _sourceCode(sourceCode), _returnType(nullptr)
{
}

SyntaxTreeNodeFuncRet::~SyntaxTreeNodeFuncRet()
{
	for (auto&& c : _children)
		delete c;
}

void SyntaxTreeNodeFuncRet::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "FuncRet(type=";
	if (_returnType) {
		char tmp[32];
		sprintf(tmp, "%p", _returnType);
		s << tmp;
	}
	s << ")" << std::endl;
	for (auto&& c : _children)
		c->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeFuncRet::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeFuncRet::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeFuncRet::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeFuncRet::AddNode(ISyntaxTreeNode* node)
{
	_children.Add(node);
	node->SetParent(this);
}

void SyntaxTreeNodeFuncRet::SetReturnType(ISyntaxTreeNodeTypeRef* type)
{
	assert(_returnType == nullptr &&
		"Expected a type to not bet set twice");
	AddNode(type);
	_returnType = type;
}

SyntaxTreeNodeFuncRet* SyntaxTreeNodeFuncRet::Parse(ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	auto ret = new SyntaxTreeNodeFuncRet(SourceCodeView(state->sourceCode, t));
	auto guard = MemoryGuard(ret);
	ret->SetReturnType(SyntaxTreeNodeTypeRef::Parse(state));
	return guard.Done();
}
