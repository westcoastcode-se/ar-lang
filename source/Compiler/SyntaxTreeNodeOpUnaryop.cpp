#include "SyntaxTreeNodeOpUnaryop.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpUnaryop::~SyntaxTreeNodeOpUnaryop()
{
	for (auto&& i : _children)
		delete i;
}

void SyntaxTreeNodeOpUnaryop::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "Unaryop(op=" << ToString(_op) << ")" << std::endl;
	for (auto&& i : _children) {
		i->ToString(s, indent + 1);
	}
}

ISyntaxTree* SyntaxTreeNodeOpUnaryop::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeOpUnaryop::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeOpUnaryop::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

bool SyntaxTreeNodeOpUnaryop::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	return false;
}

bool SyntaxTreeNodeOpUnaryop::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

bool SyntaxTreeNodeOpUnaryop::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

ISyntaxTreeNodePackage* SyntaxTreeNodeOpUnaryop::GetPackage() const
{
	if (dynamic_cast<SyntaxTreeNodePackage*>(_function->GetParent()))
		return static_cast<SyntaxTreeNodePackage*>(_function->GetParent());
	return nullptr;
}
