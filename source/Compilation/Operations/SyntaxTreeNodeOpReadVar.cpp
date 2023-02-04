#include "SyntaxTreeNodeOpReadVar.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpReadVar::SyntaxTreeNodeOpReadVar(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body)
	: SyntaxTreeNodeOp(view, body)
{
}

SyntaxTreeNodeVarRef* SyntaxTreeNodeOpReadVar::GetVariable()
{
	auto children = GetChildren();
	if (children.Size() == 0)
		return nullptr;
	return dynamic_cast<SyntaxTreeNodeVarRef*>(children[0]);
}

SyntaxTreeNodeType* SyntaxTreeNodeOpReadVar::GetType()
{
	auto type = GetVariable();
	if (type == nullptr)
		return nullptr;
	return type->GetType();
}

void SyntaxTreeNodeOpReadVar::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "ReadVar()" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

void SyntaxTreeNodeOpReadVar::Compile(Builder::Linker* linker, Builder::Instructions& target)
{
}

