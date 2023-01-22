#include "SyntaxTreeNodeRoot.h"
#include "SyntaxTreeNodePrimitive.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeRoot::SyntaxTreeNodeRoot()
	: SyntaxTreeNodePackage(SourceCodeView(), ReadOnlyString("<root>")), _syntaxTree(nullptr)
{
}

void SyntaxTreeNodeRoot::AddPrimitive(SyntaxTreeNodePrimitive* primitive)
{
	AddChild(primitive);
	_primitives[primitive->GetName()] = primitive;
}

SyntaxTreeNodePrimitive* SyntaxTreeNodeRoot::AddPrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name)
{
	auto prim = ARLANG_NEW SyntaxTreeNodePrimitive(size, primitiveType, name);
	AddPrimitive(prim);
	return prim;
}

SyntaxTreeNodePrimitive* SyntaxTreeNodeRoot::AddPrimitive(I32 size, PrimitiveType primitiveType, ReadOnlyString name,
	SyntaxTreeNodePrimitive* inheritsFrom, SyntaxTreeNodePrimitive* unrefInto)
{
	auto prim = ARLANG_NEW SyntaxTreeNodePrimitive(size, primitiveType, name, inheritsFrom, unrefInto);
	AddPrimitive(prim);
	return prim;
}

SyntaxTreeNodePrimitive* SyntaxTreeNodeRoot::FindPrimitive(ReadOnlyString name)
{
	auto it = _primitives.find(name);
	if (it == _primitives.end())
		return nullptr;
	return it->second;
}

void SyntaxTreeNodeRoot::Compile(Builder::Linker* linker)
{
	SyntaxTreeNodePackage::Compile(linker);
}

SyntaxTreeNodeRoot* SyntaxTreeNodeRoot::Create()
{
	auto root = ARLANG_NEW SyntaxTreeNodeRoot();

	// All pointers inherits from the type that represents a memory address
	auto voidPtr = root->AddPrimitive(sizeof(void*), PrimitiveType::Ptr, ReadOnlyString("*void"));

	root->AddPrimitive(sizeof(IB*), PrimitiveType::Ptr, ReadOnlyString("*bool"), voidPtr, 
		root->AddPrimitive(sizeof(IB), PrimitiveType::Bool, ReadOnlyString("bool")));

	root->AddPrimitive(sizeof(I8*), PrimitiveType::Ptr, ReadOnlyString("*int8"), voidPtr, 
		root->AddPrimitive(sizeof(I8), PrimitiveType::I8, ReadOnlyString("int8")));

	root->AddPrimitive(sizeof(U8*), PrimitiveType::Ptr, ReadOnlyString("*uint8"), voidPtr, 
		root->AddPrimitive(sizeof(U8), PrimitiveType::U8, ReadOnlyString("uint8")));

	root->AddPrimitive(sizeof(I16*), PrimitiveType::Ptr, ReadOnlyString("*int16"), voidPtr, 
		root->AddPrimitive(sizeof(I16), PrimitiveType::I16, ReadOnlyString("int16")));

	root->AddPrimitive(sizeof(U16*), PrimitiveType::Ptr, ReadOnlyString("*uint16"), voidPtr, 
		root->AddPrimitive(sizeof(U16), PrimitiveType::U16, ReadOnlyString("uint16")));

	SyntaxTreeNodePrimitive* int32;
	SyntaxTreeNodePrimitive* int32ptr;
	int32 = root->AddPrimitive(sizeof(I32), PrimitiveType::I32, ReadOnlyString("int32"));
	int32ptr = root->AddPrimitive(sizeof(I32*), PrimitiveType::Ptr, ReadOnlyString("*int32"), voidPtr, int32);

	root->AddPrimitive(sizeof(I32*), PrimitiveType::Ptr, ReadOnlyString("*int"), int32ptr, 
		root->AddPrimitive(sizeof(I32), PrimitiveType::I32, ReadOnlyString("int"), int32, nullptr));

	root->AddPrimitive(sizeof(U32*), PrimitiveType::Ptr, ReadOnlyString("*uint32"), voidPtr, 
		root->AddPrimitive(sizeof(U32), PrimitiveType::U32, ReadOnlyString("uint32")));

	root->AddPrimitive(sizeof(I64*), PrimitiveType::Ptr, ReadOnlyString("*int64"), voidPtr, 
		root->AddPrimitive(sizeof(I64), PrimitiveType::I64, ReadOnlyString("int64")));

	root->AddPrimitive(sizeof(U64*), PrimitiveType::Ptr, ReadOnlyString("*uint64"), voidPtr, 
		root->AddPrimitive(sizeof(U64), PrimitiveType::U64, ReadOnlyString("uint64")));

	SyntaxTreeNodePrimitive* float32;
	SyntaxTreeNodePrimitive* float32ptr;
	float32 = root->AddPrimitive(sizeof(F32), PrimitiveType::F32, ReadOnlyString("float32"));
	float32ptr = root->AddPrimitive(sizeof(F32*), PrimitiveType::Ptr, ReadOnlyString("*float32"), voidPtr, float32);

	root->AddPrimitive(sizeof(F32*), PrimitiveType::Ptr, ReadOnlyString("*float"), float32ptr, 
		root->AddPrimitive(sizeof(F32), PrimitiveType::F32, ReadOnlyString("float"), float32, nullptr));

	root->AddPrimitive(sizeof(F64*), PrimitiveType::Ptr, ReadOnlyString("*float64"), voidPtr, 
		root->AddPrimitive(sizeof(F64), PrimitiveType::F64, ReadOnlyString("float64")));

	return root;
}
