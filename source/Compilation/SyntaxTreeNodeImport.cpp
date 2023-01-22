#include "SyntaxTreeNodeImport.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeImport::SyntaxTreeNodeImport(SourceCodeView view, SyntaxTreeNodePackage* import)
	: SyntaxTreeNode(view), _import(import)
{
	ASSERT_NOT_NULL(import);
}

void SyntaxTreeNodeImport::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "Import(package=" << _import->GetID() << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

VisitResult SyntaxTreeNodeImport::Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags)
{
	if (_import)
		return _import->Query(visitor, flags | (I32)QuerySearchFlag::TraverseChildren);
	else
		return VisitResult::Continue;
}
