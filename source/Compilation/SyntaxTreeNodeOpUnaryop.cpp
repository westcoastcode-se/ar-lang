#include "SyntaxTreeNodeOpUnaryop.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeTypeRef.h"
#include "SyntaxTreeNodeConstant.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpUnaryop::SyntaxTreeNodeOpUnaryop(SourceCodeView sourceCode, ISyntaxTreeNodeFuncDef* function, ISyntaxTreeNodeOp* right, Op op)
	: _parent(nullptr), _children(right), _sourceCode(sourceCode), _op(op), _function(function)
{
	right->SetParent(this);
}

SyntaxTreeNodeOpUnaryop::~SyntaxTreeNodeOpUnaryop()
{
	for (auto c : _children)
		delete c;
}

void SyntaxTreeNodeOpUnaryop::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "Unaryop(op=" << ToString(_op) << ")" << std::endl;
	for (int i = 0; i < _children.Size(); ++i)
		_children[i]->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeOpUnaryop::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeOpUnaryop::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeOpUnaryop::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeOpUnaryop::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
	GetRight()->Compile(linker, instructions);

	auto stackType = GetStackType();
	if (stackType == nullptr)
		throw CompileErrorNotImplemented(this, "Unaryop");

	SyntaxTreeNodePrimitive* primitive = nullptr;
	if (dynamic_cast<SyntaxTreeNodePrimitive*>(stackType) != nullptr)
		primitive = static_cast<SyntaxTreeNodePrimitive*>(stackType);
	if (dynamic_cast<SyntaxTreeNodeTypeRef*>(stackType) != nullptr) {
		auto definitions = static_cast<SyntaxTreeNodeTypeRef*>(stackType)->GetDefinitions();
		for (auto d : definitions) {
			if (dynamic_cast<SyntaxTreeNodePrimitive*>(d)) {
				primitive = static_cast<SyntaxTreeNodePrimitive*>(d);
				break;
			}
		}
	}

	// Verify that we've found a primitive type
	if (primitive == nullptr)
		throw CompileErrorNotImplemented(this, "Unaryop");

	switch (_op)
	{
	case Op::Minus:
		instructions.Neg(primitive->GetSymbol());
		break;
	case Op::BitNot:
		instructions.BitNot(primitive->GetSymbol());
		break;
	case Op::Plus:
		break;
	default:
		throw CompileErrorNotImplemented(this, "Unaryop");
	}
}

ISyntaxTreeNodeType* SyntaxTreeNodeOpUnaryop::GetStackType()
{
	return _children[0]->GetStackType();
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeOpUnaryop::OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer)
{
	for (int i = 0; i < _children.Size(); ++i) {
		auto optimized = _children[i]->OptimizeOp(optimizer);
		if (!optimized.IsEmpty()) {
			if (optimized.Size() != 1)
				throw CompileErrorNotImplemented(this, "Unaryop optimize is not allowed expand");
			delete _children[i];
			_children[i] = optimized[0];
		}
	}
	return optimizer->Optimize(this);
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeOpUnaryop::Optimize0_Merge::Optimize(ISyntaxTreeNodeOp* node)
{
	auto impl = dynamic_cast<SyntaxTreeNodeOpUnaryop*>(node);
	if (impl == nullptr)
		return Vector<ISyntaxTreeNodeOp*>();

	auto right = static_cast<ISyntaxTreeNodeOp*>(impl->GetRight());
	
	// Compile-time resolve negative values
	if (dynamic_cast<SyntaxTreeNodeConstant*>(right)) {
		auto rightConst = static_cast<SyntaxTreeNodeConstant*>(right);
		auto stackType = static_cast<ISyntaxTreeNodePrimitive*>(rightConst->GetStackType());

		PrimitiveValue newValue = rightConst->GetValue();
		switch (impl->_op)
		{
		case Op::Minus:
			if (PrimitiveValue::Neg(&newValue)) {
				auto combined = ARLANG_NEW SyntaxTreeNodeConstant(impl->_function,
					impl->_sourceCode, newValue,
					stackType);
				count++;
				return Vector<ISyntaxTreeNodeOp*>(combined);
			}
			break;
		case Op::Plus:
		default:
			break;
		}
	}

	return Vector<ISyntaxTreeNodeOp*>();
}
