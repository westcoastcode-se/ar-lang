#include "SyntaxTreeNodeOpTypeCast.h"
#include "SyntaxTreeNodeConstant.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpTypeCast::~SyntaxTreeNodeOpTypeCast()
{
	for (auto c : _children)
		delete c;
}

void SyntaxTreeNodeOpTypeCast::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "OpTypeCast(to=" << _children[0]->GetID() << ")";
	s << std::endl;
	for (auto c : _children)
		c->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeOpTypeCast::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeOpTypeCast::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeOpTypeCast::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeOpTypeCast::Compile(Builder::Linker* linker, Builder::Instructions& instructions)
{
	// The second child is what we want to cast
	static_cast<ISyntaxTreeNodeOp*>(_children[1])->Compile(linker, instructions);	
}

ISyntaxTreeNodeType* SyntaxTreeNodeOpTypeCast::GetStackType()
{
	// The first child is the type reference
	return static_cast<ISyntaxTreeNodeType*>(_children[0]);
}

ISyntaxTreeNodeType* SyntaxTreeNodeOpTypeCast::FromType()
{
	return static_cast<ISyntaxTreeNodeOp*>(_children[1])->GetStackType();
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeOpTypeCast::OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer)
{
	auto optimized = static_cast<ISyntaxTreeNodeOp*>(_children[1])->OptimizeOp(optimizer);
	if (!optimized.IsEmpty()) {
		if (optimized.Size() != 1)
			throw CompileErrorNotImplemented(this, "Unaryop optimize is not allowed expand");
		delete _children[1];
		_children[1] = optimized[0];
	}
	return optimizer->Optimize(this);
}

void SyntaxTreeNodeOpTypeCast::SetNewType(ISyntaxTreeNodeType* type)
{
	_children[0] = type;
	type->SetParent(this);
}

void SyntaxTreeNodeOpTypeCast::SetOp(ISyntaxTreeNodeOp* op)
{
	_children[1] = op;
	op->SetParent(this);
}

Vector<ISyntaxTreeNodeOp*> SyntaxTreeNodeOpTypeCast::Optimize0_Merge::Optimize(ISyntaxTreeNodeOp* node)
{
	auto impl = dynamic_cast<SyntaxTreeNodeOpTypeCast*>(node);
	if (impl == nullptr)
		return Vector<ISyntaxTreeNodeOp*>();

	// The child-node
	auto child = static_cast<ISyntaxTreeNodeOp*>(impl->_children[1]);

	// Is the child a constant? If so, then try to perform the type-casting during compile time
	// and return the constant as the merged child
	if (dynamic_cast<SyntaxTreeNodeConstant*>(child)) {
		auto constant = static_cast<SyntaxTreeNodeConstant*>(child);
		return constant->Cast(impl->GetStackType());
	}
	return Vector<ISyntaxTreeNodeOp*>();
}
