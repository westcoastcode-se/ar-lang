#include "SyntaxTreeNodeOpReturn.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpReturn::~SyntaxTreeNodeOpReturn()
{
	for (auto& n : _children)
		delete n;
}

void SyntaxTreeNodeOpReturn::ToString(StringStream& s, int indent) const
{
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
}

bool SyntaxTreeNodeOpReturn::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	return false;
}

bool SyntaxTreeNodeOpReturn::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

bool SyntaxTreeNodeOpReturn::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

const SourceCodeView* SyntaxTreeNodeOpReturn::GetSourceCode() const
{
	return nullptr;
}

void SyntaxTreeNodeOpReturn::AddNode(ISyntaxTreeNode* node)
{
	_children.Add(node);
	node->SetParent(this);
}
