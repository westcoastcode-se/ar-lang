#include "SyntaxTreeNodeOpBinop.h"
#include "../SyntaxTreeNodePackage.h"
#include "../Types/SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeConstant.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpBinop::SyntaxTreeNodeOpBinop(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body, Op op)
	:SyntaxTreeNodeOp(view, body), _op(op)
{
}

void SyntaxTreeNodeOpBinop::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "Binop(op=" << ToString(_op) << ")" << std::endl;
	SyntaxTreeNodeOp::ToString(s, indent);
}

SyntaxTreeNodeType* SyntaxTreeNodeOpBinop::GetType()
{
	return SyntaxTreeNodeOpBinop::GetRight()->GetType();
}

void SyntaxTreeNodeOpBinop::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
	GetLeft()->Compile(linker, instructions);
	GetRight()->Compile(linker, instructions);

	auto stackType = GetType();
	if (stackType == nullptr || dynamic_cast<SyntaxTreeNodePrimitive*>(stackType) == nullptr)
		throw CompileErrorNotImplemented(this, "Binop");
	auto primitive = static_cast<SyntaxTreeNodePrimitive*>(stackType);

	switch (_op)
	{
	case Op::Plus:
		instructions.Add(primitive->GetSymbol());
		break;
	case Op::Minus:
		instructions.Sub(primitive->GetSymbol());
		break;
	case Op::Mult:
		instructions.Mult(primitive->GetSymbol());
		break;
	case Op::Div:
		instructions.Div(primitive->GetSymbol());
		break;
	default:
		throw CompileErrorNotImplemented(this, "Binop");
	}
}

Vector<SyntaxTreeNodeOp*> SyntaxTreeNodeOpBinop::Optimize0_Merge::Optimize(SyntaxTreeNodeOpBinop* node)
{
	auto left = node->GetLeft();
	auto right = node->GetRight();

	// Compile-time combin ethe two constants using this operator
	if (dynamic_cast<SyntaxTreeNodeConstant*>(left) && dynamic_cast<SyntaxTreeNodeConstant*>(right)) {
		auto leftConst = static_cast<SyntaxTreeNodeConstant*>(left);
		auto rightConst = static_cast<SyntaxTreeNodeConstant*>(right);

		PrimitiveValue newValue = leftConst->GetValue();
		PrimitiveValue::OpFn fn;
		switch (node->_op)
		{
		case Op::Plus:
			fn = PrimitiveValue::Add;
			break;
		case Op::Minus:
			fn = PrimitiveValue::Sub;
			break;
		case Op::Mult:
			fn = PrimitiveValue::Mult;
			break;
		case Op::Div:
			fn = PrimitiveValue::Div;
			break;
		default:
			return Vector<SyntaxTreeNodeOp*>();
		}

		if (fn(&newValue, &rightConst->GetValue())) {
			// TODO: Add support for converting the new type into the appropriate
			auto leftType = static_cast<SyntaxTreeNodePrimitive*>(leftConst->GetType());
			auto rightType = static_cast<SyntaxTreeNodePrimitive*>(rightConst->GetType());
			auto newType = rightType;
			if (newValue.type != newType->GetPrimitiveType())
				newType = leftType;
			auto combined = ARLANG_NEW SyntaxTreeNodeConstant(node->GetSourceCode(), node->GetBody(),
				newValue, newType);
			count++;
			return Vector<SyntaxTreeNodeOp*>(combined);
		}
	}

	return Vector<SyntaxTreeNodeOp*>();
}
