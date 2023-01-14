#include "SyntaxTreeNodeOpReturn.h"
#include "SyntaxTreeNodeFuncBody.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpReturn::~SyntaxTreeNodeOpReturn()
{
	for (auto n : _children)
		delete n;
}

void SyntaxTreeNodeOpReturn::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "OpReturn()" << std::endl;
	for (auto i : _children)
		i->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeOpReturn::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeOpReturn::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeOpReturn::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeOpReturn::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
	instructions.Ret();
}

void SyntaxTreeNodeOpReturn::AddOp(ISyntaxTreeNodeOp* node)
{
	_children.Add(node);
	node->SetParent(this);
}
