#include "SyntaxTreeNodeImport.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeImport::SyntaxTreeNodeImport(SourceCodeView sourceCode, SyntaxTreeNodePackage* import)
	: _sourceCode(sourceCode), _parent(nullptr), _import(import)
{
}

SyntaxTreeNodeImport::~SyntaxTreeNodeImport()
{
	for (auto& n : _children)
		delete n;
}

void SyntaxTreeNodeImport::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "Import(package=";
	if (_import)
		s << _import->GetID();
	s << ")" << std::endl;
	for (auto&& i : _children) {
		i->ToString(s, indent + 1);
	}
}

ISyntaxTree* SyntaxTreeNodeImport::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeImport::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

ISyntaxTreeNode* SyntaxTreeNodeImport::GetParent() const
{
	return _parent;
}

void SyntaxTreeNodeImport::SetParent(ISyntaxTreeNode* parent)
{
	assert(dynamic_cast<ISyntaxTreeNodePackage*>(parent) &&
		"imports should only be done on package level");
	_parent = dynamic_cast<ISyntaxTreeNodePackage*>(parent);
}

VisitResult SyntaxTreeNodeImport::Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags)
{
	if (_import)
		return _import->Query(visitor, flags | (I32)QuerySearchFlag::TraverseChildren);
	else
		return VisitResult::Continue;
}
