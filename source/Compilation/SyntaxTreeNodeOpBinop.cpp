#include "SyntaxTreeNodeOpBinop.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpBinop::~SyntaxTreeNodeOpBinop()
{
	for (auto i : _children)
		delete i;
}

void SyntaxTreeNodeOpBinop::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "Binop(op=" << ToString(_op) << ")" << std::endl;
	for (auto i : _children) {
		i->ToString(s, indent + 1);
	}
}

ISyntaxTree* SyntaxTreeNodeOpBinop::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeOpBinop::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeOpBinop::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

ISyntaxTreeNodePackage* SyntaxTreeNodeOpBinop::GetPackage() const
{
	if (dynamic_cast<SyntaxTreeNodePackage*>(_function->GetParent()))
		return static_cast<SyntaxTreeNodePackage*>(_function->GetParent());
	return nullptr;
}
