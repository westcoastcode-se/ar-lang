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

void SyntaxTreeNodeFuncRet::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

bool SyntaxTreeNodeFuncRet::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	if (!visitor->Visit(this))
		return false;
	for (auto&& n : _children)
		n->Visit(visitor);
	return true;
}

bool SyntaxTreeNodeFuncRet::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	if (!visitor->Visit(this))
		return false;
	for (auto&& n : _children)
		n->Visit(visitor);
	return true;
}

bool SyntaxTreeNodeFuncRet::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

void SyntaxTreeNodeFuncRet::AddNode(ISyntaxTreeNode* node)
{
	_children.push_back(node);
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

	auto ret = new SyntaxTreeNodeFuncRet(SourceCodeView(state->sourceCode, t));
	auto guard = MemoryGuard(ret);
	t->Next();
	ret->SetReturnType(SyntaxTreeNodeTypeRef::Parse(state));
	return guard.Done();
}
