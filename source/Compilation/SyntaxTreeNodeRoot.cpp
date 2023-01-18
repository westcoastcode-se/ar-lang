#include "SyntaxTreeNodeRoot.h"
#include "SyntaxTreeNodePrimitive.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeRoot::~SyntaxTreeNodeRoot()
{
}

void SyntaxTreeNodeRoot::AddPrimitive(ISyntaxTreeNodePrimitive* primitive)
{
	AddNode(primitive);
	_primitives[primitive->GetName()] = primitive;
}

ISyntaxTreeNodePrimitive* SyntaxTreeNodeRoot::FindPrimitive(ReadOnlyString name)
{
	auto it = _primitives.find(name);
	if (it == _primitives.end())
		return nullptr;
	return it->second;
}

SyntaxTreeNodeRoot* SyntaxTreeNodeRoot::Create()
{
	auto root = ARLANG_NEW SyntaxTreeNodeRoot();

	auto voidPtr = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(void*), PrimitiveType::Ptr, ReadOnlyString("*void"));
	root->AddPrimitive(voidPtr);

	auto i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(IB), PrimitiveType::Bool, ReadOnlyString("bool"));
	root->AddPrimitive(i);
	auto ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(IB*), PrimitiveType::Ptr, ReadOnlyString("*bool"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I8), PrimitiveType::I8, ReadOnlyString("int8"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I8*), PrimitiveType::Ptr, ReadOnlyString("*int8"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U8), PrimitiveType::U8, ReadOnlyString("uint8"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U8*), PrimitiveType::Ptr, ReadOnlyString("*uint8"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I16), PrimitiveType::I16, ReadOnlyString("int16"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I16*), PrimitiveType::Ptr, ReadOnlyString("*int16"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U16), PrimitiveType::U16, ReadOnlyString("uint16"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U16*), PrimitiveType::Ptr, ReadOnlyString("*uint16"), voidPtr, i);
	root->AddPrimitive(ip);

	SyntaxTreeNodePrimitive* int32;
	int32 = i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I32), PrimitiveType::I32, ReadOnlyString("int32"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I32*), PrimitiveType::Ptr, ReadOnlyString("*int32"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I32), PrimitiveType::I32, ReadOnlyString("int"), int32, nullptr);
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I32*), PrimitiveType::Ptr, ReadOnlyString("*int"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U32), PrimitiveType::U32, ReadOnlyString("uint32"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U32*), PrimitiveType::Ptr, ReadOnlyString("*uint32"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I64), PrimitiveType::I64, ReadOnlyString("int64"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(I64*), PrimitiveType::Ptr, ReadOnlyString("*int64"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U64), PrimitiveType::U64, ReadOnlyString("uint64"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(U64*), PrimitiveType::Ptr, ReadOnlyString("*uint64"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(F32), PrimitiveType::F32, ReadOnlyString("float32"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(F32*), PrimitiveType::Ptr, ReadOnlyString("*float32"), voidPtr, i);
	root->AddPrimitive(ip);

	i = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(F64), PrimitiveType::F64, ReadOnlyString("float64"));
	root->AddPrimitive(i);
	ip = ARLANG_NEW SyntaxTreeNodePrimitive(root, sizeof(F64*), PrimitiveType::Ptr, ReadOnlyString("*float64"), voidPtr, i);
	root->AddPrimitive(ip);

	return root;
}

SyntaxTreeNodeRoot::SyntaxTreeNodeRoot()
	: SyntaxTreeNodePackage(SourceCodeView(), ReadOnlyString("<root>")), _syntaxTree(nullptr)
{
}
