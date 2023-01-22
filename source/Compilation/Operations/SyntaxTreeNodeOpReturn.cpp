#include "SyntaxTreeNodeOpReturn.h"
#include "../SyntaxTreeNodeFuncBody.h"
#include "../SyntaxTreeNodeFunc.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpReturn::SyntaxTreeNodeOpReturn(SourceCodeView view, SyntaxTreeNodeFuncBody* body)
	: SyntaxTreeNodeOp(view, body)
{
}

void SyntaxTreeNodeOpReturn::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "OpReturn()" << std::endl;
	SyntaxTreeNodeOp::ToString(s, indent);
}


void SyntaxTreeNodeOpReturn::Compile(Builder::Linker* linker, Builder::Instructions& target)
{
	SyntaxTreeNodeOp::Compile(linker, target);
	target.Ret();
}

SyntaxTreeNodeType* SyntaxTreeNodeOpReturn::GetType()
{
	if (GetChildren().Size() == 1) {
		return static_cast<SyntaxTreeNodeOp*>(GetChildren()[0])->GetType();
	}
	throw CompileErrorNotImplemented(this, "MultiReturn");
}

void SyntaxTreeNodeOpReturn::Resolve()
{
	SyntaxTreeNodeOp::Resolve();

	auto function = GetBody()->GetFunction();
	auto expectedType = function->GetReturnType()->GetType();
	auto returnedType = GetType();
	// Perform post-resolve type validation
	if (!expectedType->IsCompatibleWith(returnedType)) {
		throw CompileErrorIncompatibleTypes(this, expectedType, returnedType);
	}
}
