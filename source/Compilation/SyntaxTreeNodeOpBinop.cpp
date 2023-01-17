#include "SyntaxTreeNodeOpBinop.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeConstant.h"

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

ISyntaxTreeNodeType* SyntaxTreeNodeOpBinop::GetStackType()
{
	return SyntaxTreeNodeOpBinop::GetRight()->GetStackType();
}

void SyntaxTreeNodeOpBinop::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
	GetLeft()->Compile(linker, instructions);
	GetRight()->Compile(linker, instructions);

	auto stackType = GetStackType();
	if (stackType == nullptr || dynamic_cast<SyntaxTreeNodePrimitive*>(stackType) == nullptr)
		throw CompileErrorNotImplemented(this, "Binop");
	auto primitive = static_cast<SyntaxTreeNodePrimitive*>(stackType);

	switch (_op)
	{
	case Op::Plus:
		instructions.Add(primitive->GetSymbol());
		break;
	case Op::Minus:
		instructions.Add(primitive->GetSymbol());
		break;
	case Op::Mult:
		instructions.Add(primitive->GetSymbol());
		break;
	case Op::Div:
		instructions.Add(primitive->GetSymbol());
		break;
	default:
		throw CompileErrorNotImplemented(this, "Binop");
	}
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeOpBinop::OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer)
{
	for (int i = 0; i < _children.Size(); ++i) {
		auto optimized = _children[i]->OptimizeOp(optimizer);
		if (!optimized.IsEmpty()) {
			if (optimized.Size() != 1)
				throw CompileErrorNotImplemented(this, "Binop optimize is not allowed expand");
			delete _children[i];
			_children[i] = optimized[0];
		}
	}
	return optimizer->Optimize(this);
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeOpBinop::Optimize0_Merge::Optimize(ISyntaxTreeNodeOp* node)
{
	auto impl = dynamic_cast<SyntaxTreeNodeOpBinop*>(node);
	if (impl == nullptr)
		return Vector<ISyntaxTreeNodeOp*>();

	auto left = static_cast<ISyntaxTreeNodeOp*>(impl->GetLeft());
	auto right = static_cast<ISyntaxTreeNodeOp*>(impl->GetRight());

	// Compile-time combin ethe two constants using this operator
	if (dynamic_cast<SyntaxTreeNodeConstant*>(left) && dynamic_cast<SyntaxTreeNodeConstant*>(right)) {
		auto leftConst = static_cast<SyntaxTreeNodeConstant*>(left);
		auto rightConst = static_cast<SyntaxTreeNodeConstant*>(right);

		PrimitiveValue newValue = leftConst->GetValue();
		switch (impl->_op)
		{
		case Op::Plus:
			if (PrimitiveValue::Add(&newValue, &rightConst->GetValue())) {
				// TODO: Add support for converting the new type into the appropriate
				auto leftType = static_cast<ISyntaxTreeNodePrimitive*>(leftConst->GetStackType());
				auto rightType = static_cast<ISyntaxTreeNodePrimitive*>(rightConst->GetStackType());
				auto newType = rightType;
				if (newValue.type != newType->GetPrimitiveType())
					newType = leftType;
				auto combined = new SyntaxTreeNodeConstant(impl->_function,
					impl->_sourceCode, newValue, 
					newType);
				count++;
				return Vector<ISyntaxTreeNodeOp*>(combined);
			}
			break;
		default:
			break;
		}
	}

	return Vector<ISyntaxTreeNodeOp*>();
}
