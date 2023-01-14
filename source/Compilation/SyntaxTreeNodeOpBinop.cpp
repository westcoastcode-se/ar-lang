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
	s << _id << Indent(indent);
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

void SyntaxTreeNodeOpBinop::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
/*	GetLeft()->Compile(linker, instructions);
	GetRight()->Compile(linker, instructions);

	switch (_op)
	{
	case Op::Plus:
		//instructions.Add(GetRight());
		break;
	}
	*/
	throw CompileErrorNotImplemented(this, "Binop");
}
