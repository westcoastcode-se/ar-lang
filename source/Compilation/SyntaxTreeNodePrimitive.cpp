#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ISyntaxTree* SyntaxTreeNodePrimitive::GetSyntaxTree() const
{
	return _package->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodePrimitive::GetParent() const
{
	return _package;
}

void SyntaxTreeNodePrimitive::SetParent(ISyntaxTreeNode* parent)
{
	// Don't do anything since this is done during construction of this type
}

const List<ISyntaxTreeNode*>& SyntaxTreeNodePrimitive::GetChildren() const
{
	static const List<ISyntaxTreeNode*> empty;
	return empty;
}

void SyntaxTreeNodePrimitive::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "Primitive(name=" << _name << ", size=" << _stackSize << "";
	if (_inheritsFrom)
		s << ", inheritsFrom=" << _inheritsFrom->GetName();
	if (_unrefInto)
		s << ", unrefInto=" << _unrefInto->GetName();
	s << ")" << std::endl;
}

ISyntaxTreeNode* SyntaxTreeNodePrimitive::GetRootNode()
{
	return _package;
}

bool SyntaxTreeNodePrimitive::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	return false;
}

bool SyntaxTreeNodePrimitive::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

bool SyntaxTreeNodePrimitive::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}
