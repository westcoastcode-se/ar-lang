#include "SyntaxTreeNodeFunc.h"
#include "SyntaxTreeNodeFuncArg.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFunc::SyntaxTreeNodeFunc(SourceCodeView view, ReadOnlyString name)
	: SyntaxTreeNode(view), _name(name), _returnType(nullptr)
{
}

void SyntaxTreeNodeFunc::AddArgument(SyntaxTreeNodeFuncArg* arg)
{
	AddChild(arg);
	_arguments.Add(arg);
}

void SyntaxTreeNodeFunc::SetReturnType(SyntaxTreeNodeType* returnType)
{
	AddChild(returnType);
	_returnType = returnType;
}
