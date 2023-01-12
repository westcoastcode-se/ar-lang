#include "SyntaxTreeNodeImport.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeImport::SyntaxTreeNodeImport(SourceCodeView sourceCode, SyntaxTreeNodePackage* package)
	: _sourceCode(sourceCode), _parent(package)
{
}

SyntaxTreeNodeImport::~SyntaxTreeNodeImport()
{
	for (auto& n : _children)
		delete n;
}

void SyntaxTreeNodeImport::ToString(StringStream& s, int indent) const
{
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
}

bool SyntaxTreeNodeImport::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	return false;
}

bool SyntaxTreeNodeImport::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

bool SyntaxTreeNodeImport::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}
