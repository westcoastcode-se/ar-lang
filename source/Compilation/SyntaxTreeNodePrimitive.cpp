#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodePackage.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ISyntaxTree* SyntaxTreeNodePrimitive::GetSyntaxTree() const
{
	return _package->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodePrimitive::GetParent() const
{
	return _package;
}

void SyntaxTreeNodePrimitive::SetParent(ISyntaxTreeNode* parent)
{
	// Don't do anything since this is done during construction of this type
}

ReadOnlyArray<ISyntaxTreeNode*> SyntaxTreeNodePrimitive::GetChildren() const
{
	return ReadOnlyArray<ISyntaxTreeNode*>();
}

void SyntaxTreeNodePrimitive::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "Primitive(name=" << _name << ", size=" << _stackSize << "";
	if (_inheritsFrom)
		s << ", inheritsFrom=" << _inheritsFrom->GetName();
	if (_unrefInto)
		s << ", unrefInto=" << _unrefInto->GetName();
	s << ")" << std::endl;
}

ISyntaxTreeNode* SyntaxTreeNodePrimitive::GetRootNode()
{
	return _package;
}

void SyntaxTreeNodePrimitive::Compile(Builder::Linker* linker)
{
	if (_symbol != nullptr)
		return;

	Builder::TypeFlags flags = (Builder::TypeFlags)Builder::TypeFlag::Primitive;
	if (_primitiveType == Interpreter::PrimitiveType::Ptr)
		flags |= (Builder::TypeFlags)Builder::TypeFlag::Ptr;
	_symbol = new Builder::Type(_name, _stackSize, flags, _primitiveType);
	_package->GetSymbol()->Add(_symbol);
}
