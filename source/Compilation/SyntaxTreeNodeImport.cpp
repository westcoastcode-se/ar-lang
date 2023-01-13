#include "SyntaxTreeNodeImport.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeImport::SyntaxTreeNodeImport(SourceCodeView sourceCode, SyntaxTreeNodePackage* package)
	: _sourceCode(sourceCode), _parent(nullptr)
{
}

SyntaxTreeNodeImport::~SyntaxTreeNodeImport()
{
	for (auto& n : _children)
		delete n;
}

void SyntaxTreeNodeImport::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "Import()" << std::endl;
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
