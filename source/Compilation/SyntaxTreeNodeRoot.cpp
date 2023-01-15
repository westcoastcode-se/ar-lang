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
	auto root = new SyntaxTreeNodeRoot();

	auto voidPtr = new SyntaxTreeNodePrimitive(root, sizeof(void*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*void"));
	root->AddPrimitive(voidPtr);

	auto i = new SyntaxTreeNodePrimitive(root, sizeof(IB), Interpreter::PrimitiveType::Bool, ReadOnlyString("bool"));
	root->AddPrimitive(i);
	auto ip = new SyntaxTreeNodePrimitive(root, sizeof(IB*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*bool"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I8), Interpreter::PrimitiveType::I8, ReadOnlyString("int8"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I8*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int8"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U8), Interpreter::PrimitiveType::U8, ReadOnlyString("uint8"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U8*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint8"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I16), Interpreter::PrimitiveType::I16, ReadOnlyString("int16"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I16*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int16"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U16), Interpreter::PrimitiveType::U16, ReadOnlyString("uint16"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U16*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint16"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I32), Interpreter::PrimitiveType::I32, ReadOnlyString("int32"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I32*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int32"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U32), Interpreter::PrimitiveType::U32, ReadOnlyString("uint32"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U32*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint32"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(I64), Interpreter::PrimitiveType::I64, ReadOnlyString("int64"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(I64*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*int64"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(U64), Interpreter::PrimitiveType::U64, ReadOnlyString("uint64"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(U64*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*uint64"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(F32), Interpreter::PrimitiveType::F32, ReadOnlyString("float32"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(F32*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*float32"), voidPtr, i);
	root->AddPrimitive(ip);

	i = new SyntaxTreeNodePrimitive(root, sizeof(F64), Interpreter::PrimitiveType::F64, ReadOnlyString("float64"));
	root->AddPrimitive(i);
	ip = new SyntaxTreeNodePrimitive(root, sizeof(F64*), Interpreter::PrimitiveType::Ptr, ReadOnlyString("*float64"), voidPtr, i);
	root->AddPrimitive(ip);

	return root;
}

SyntaxTreeNodeRoot::SyntaxTreeNodeRoot()
	: SyntaxTreeNodePackage(SourceCodeView(), ReadOnlyString("<root>")), _syntaxTree(nullptr)
{
}
