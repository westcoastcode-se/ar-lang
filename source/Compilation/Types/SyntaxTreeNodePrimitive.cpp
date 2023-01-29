#include "SyntaxTreeNodePrimitive.h"
#include "../SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

void SyntaxTreeNodePrimitive::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "Primitive(name=" << _name << ", size=" << _stackSize << "";
	if (_inheritsFrom)
		s << ", inheritsFrom=" << _inheritsFrom->GetName();
	if (_unrefInto)
		s << ", unrefInto=" << _unrefInto->GetName();
	s << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

Builder::Type* SyntaxTreeNodePrimitive::GetSymbol()
{
	if (_symbol == nullptr) {
		Builder::TypeFlags flags = (Builder::TypeFlags)Builder::TypeFlag::Primitive;
		if (_primitiveType == PrimitiveType::Ptr)
			flags |= (Builder::TypeFlags)Builder::TypeFlag::Ptr;
		_symbol = new Builder::Type(_name, _stackSize, flags, _primitiveType);
	}
	return _symbol;
}

void SyntaxTreeNodePrimitive::Compile(Builder::Linker* linker)
{
	if (_symbol != nullptr)
		return;

	auto package = static_cast<SyntaxTreeNodePackage*>(GetParent())->GetSymbol();	
	package->Add(GetSymbol());
}

bool SyntaxTreeNodePrimitive::IsCompatibleWith(SyntaxTreeNodeType* def)
{
	auto prim = dynamic_cast<SyntaxTreeNodePrimitive*>(def);
	if (prim) {
		return PrimitiveValue::Result(GetPrimitiveType(), prim->GetPrimitiveType()) != PrimitiveType::Unknown;
	}
	return false;
}
