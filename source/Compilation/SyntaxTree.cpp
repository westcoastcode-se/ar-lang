#include "SyntaxTree.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTree::SyntaxTree()
	: _root(SyntaxTreeNodeRoot::Create())
{
	_root->SetSyntaxTree(this);
}

SyntaxTree::~SyntaxTree()
{
	delete _root;
}

void SyntaxTree::ToString(StringStream& s) const
{
	if (_root)
		_root->ToString(s, 1);
}

void SyntaxTree::Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags)
{
	_root->Visit(visitor, flags);
}

void SyntaxTree::Resolve()
{
	_root->Resolve();
}

void SyntaxTree::Compile(Builder::Linker* linker)
{
	_root->Compile(linker);
}

void SyntaxTree::Optimize(ISyntaxTreeNodeOptimizer* optimizer)
{
	_root->Optimize(optimizer);
}
