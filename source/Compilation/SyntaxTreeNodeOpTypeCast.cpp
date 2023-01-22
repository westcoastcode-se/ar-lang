#include "SyntaxTreeNodeOpTypeCast.h"
#include "SyntaxTreeNodeConstant.h"
#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeTypeDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpTypeCast::SyntaxTreeNodeOpTypeCast(SourceCodeView view, SyntaxTreeNodeFuncBody* body)
	: SyntaxTreeNodeOp(view, body)
{
}

void SyntaxTreeNodeOpTypeCast::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "OpTypeCast(to=" << (GetChildren()[0])->GetID() << ")";
	s << std::endl;
	SyntaxTreeNodeOp::ToString(s, indent);
}

void SyntaxTreeNodeOpTypeCast::Compile(Builder::Linker* linker, Builder::Instructions& target)
{
	// The second child is what we want to cast
	static_cast<SyntaxTreeNodeOp*>(GetChildren()[1])->Compile(linker, target);

	auto f1 = FromType();
	auto g1 = GetType()->GetType();

	auto fromType = dynamic_cast<SyntaxTreeNodePrimitive*>(f1);
	auto toType = dynamic_cast<SyntaxTreeNodePrimitive*>(g1);

	if (fromType == nullptr || toType == nullptr) {
		throw CompileErrorNotImplemented(this, "TypeCast only supports casting between primitives ftm");
	}

	target.Conv(fromType->GetSymbol(), toType->GetSymbol());
}

SyntaxTreeNodeTypeDef* SyntaxTreeNodeOpTypeCast::GetType()
{
	// The first child is the type that the value will have after casting is done
	return static_cast<SyntaxTreeNodeTypeDef*>(GetChildren()[0]);
}

SyntaxTreeNodeOpTypeCast* SyntaxTreeNodeOpTypeCast::Cast(SourceCodeView view, SyntaxTreeNodeFuncBody* body,
	SyntaxTreeNodeTypeDef* type, SyntaxTreeNodeOp* op)
{
	auto node = ARLANG_NEW SyntaxTreeNodeOpTypeCast(view, body);
	auto guard = MemoryGuard(node);
	node->AddChild(type);
	node->AddChild(op);
	return guard.Done();
}

SyntaxTreeNodeTypeDef* SyntaxTreeNodeOpTypeCast::FromType()
{
	// Second child is what we are casting from
	return static_cast<SyntaxTreeNodeOp*>(GetChildren()[1])->GetType();
}

Vector<SyntaxTreeNodeOp*> SyntaxTreeNodeOpTypeCast::Optimize0_Merge::Optimize(SyntaxTreeNodeOp* node)
{
	auto impl = dynamic_cast<SyntaxTreeNodeOpTypeCast*>(node);
	if (impl == nullptr)
		return Vector<SyntaxTreeNodeOp*>();

	// The child-node
	auto child = static_cast<SyntaxTreeNodeOp*>(impl->GetChildren()[1]);

	// Is the child a constant? If so, then try to perform the type-casting during compile time
	// and return the constant as the merged child
	if (dynamic_cast<SyntaxTreeNodeConstant*>(child)) {
		count++;
		auto constant = static_cast<SyntaxTreeNodeConstant*>(child);
		return constant->Cast(impl->GetType());
	}
	return Vector<SyntaxTreeNodeOp*>();
}
