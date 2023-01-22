#include "SyntaxTreeNodeOpUnaryop.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeTypeRef.h"
#include "SyntaxTreeNodeConstant.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpUnaryop::SyntaxTreeNodeOpUnaryop(SourceCodeView view, SyntaxTreeNodeFuncBody* body, Op op)
	: SyntaxTreeNodeOp(view, body), _op(op)
{
}

void SyntaxTreeNodeOpUnaryop::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "Unaryop(op=" << ToString(_op) << ")" << std::endl;
	SyntaxTreeNodeOp::ToString(s, indent);
}

void SyntaxTreeNodeOpUnaryop::Compile(Builder::Linker* linker, Builder::Instructions& target)
{
	GetRight()->Compile(linker, target);

	auto stackType = GetType()->GetType();
	if (stackType == nullptr || dynamic_cast<SyntaxTreeNodePrimitive*>(stackType) == nullptr)
		throw CompileErrorNotImplemented(this, "Unaryop");
	auto primitive = static_cast<SyntaxTreeNodePrimitive*>(stackType);

	switch (_op)
	{
	case Op::Minus:
		target.Neg(primitive->GetSymbol());
		break;
	case Op::BitNot:
		target.BitNot(primitive->GetSymbol());
		break;
	case Op::Plus:
		break;
	default:
		throw CompileErrorNotImplemented(this, "Unaryop");
	}
}

SyntaxTreeNodeTypeDef* SyntaxTreeNodeOpUnaryop::GetType()
{
	return static_cast<SyntaxTreeNodeOp*>(GetChildren()[0])->GetType();
}

Vector<SyntaxTreeNodeOp*> SyntaxTreeNodeOpUnaryop::Optimize0_Merge::Optimize(SyntaxTreeNodeOp* node)
{
	auto impl = dynamic_cast<SyntaxTreeNodeOpUnaryop*>(node);
	if (impl == nullptr)
		return Vector<SyntaxTreeNodeOp*>();

	auto right = impl->GetRight();
	
	// Compile-time resolve negative values
	if (dynamic_cast<SyntaxTreeNodeConstant*>(right)) {
		auto rightConst = static_cast<SyntaxTreeNodeConstant*>(right);
		auto stackType = static_cast<SyntaxTreeNodePrimitive*>(rightConst->GetType());

		PrimitiveValue newValue = rightConst->GetValue();
		switch (impl->_op)
		{
		case Op::Minus:
			if (PrimitiveValue::Neg(&newValue)) {
				auto combined = ARLANG_NEW SyntaxTreeNodeConstant(impl->GetSourceCode(), impl->GetBody(),
					newValue, stackType);
				count++;
				return Vector<SyntaxTreeNodeOp*>(combined);
			}
			break;
		case Op::Plus:
		default:
			break;
		}
	}

	return Vector<SyntaxTreeNodeOp*>();
}
