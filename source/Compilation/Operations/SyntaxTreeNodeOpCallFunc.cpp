#include "SyntaxTreeNodeOpCallFunc.h"
#include "../SyntaxTreeNodeFuncArg.h"
#include "../Functions/SyntaxTreeNodeFuncDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeOpCallFunc::SyntaxTreeNodeOpCallFunc(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body)
	: SyntaxTreeNodeOp(view, body), _function(nullptr)
{
}

SyntaxTreeNodeFuncRef* SyntaxTreeNodeOpCallFunc::GetFunction()
{
	return _function;
}

void SyntaxTreeNodeOpCallFunc::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "CallFunc()" << std::endl;
	SyntaxTreeNodeOp::ToString(s, indent);
}

void SyntaxTreeNodeOpCallFunc::Compile(Builder::Linker* linker, Builder::Instructions& target)
{
	// Compile all children except the function reference, since that's not an operation
	auto children = GetChildren();
	for(I8 i = 0; i < children.Size() - 1; ++i)
		static_cast<SyntaxTreeNodeOp*>(children[i])->Compile(linker, target);

	SyntaxTreeNodeFuncDef* def = nullptr;
	if (_function != nullptr) {
		// Assume that the best matching definition is at the top
		auto definitions = _function->GetDefinitions();
		def = dynamic_cast<SyntaxTreeNodeFuncDef*>(definitions[0]);
	}

	if (def == nullptr)
		throw CompileErrorUnresolvedFuncReference(_function);
	target.Call(def->GetSymbol());
}

SyntaxTreeNodeType* SyntaxTreeNodeOpCallFunc::GetType()
{
	auto funcDef = _function->GetDefinitions()[0];
	auto type = funcDef->GetReturnType();
	return type->GetType();
}

void SyntaxTreeNodeOpCallFunc::Resolve()
{
	if (_function == nullptr) {
		throw CompileErrorNotImplemented(this, "Missing function reference");
	}

	// Resolve all children except the function reference itself
	auto children = GetChildren();
	if (!children.IsEmpty()) {
		auto count = children.Size();
		for (auto i = 0; i < count - 1; ++i) {
			auto op = dynamic_cast<SyntaxTreeNodeOp*>(children[i]);
			op->Resolve();

			_function->AddArgument(ARLANG_NEW SyntaxTreeNodeFuncArg(GetSourceCode(),
				ReadOnlyString(), op->GetType()));
		}
	}

	// Now when all children are resolved, let's resolve the function since
	// we now know the types for all children
	_function->Resolve();
}

void SyntaxTreeNodeOpCallFunc::SetFunction(SyntaxTreeNodeFuncRef* func)
{
	assert(_function == nullptr &&
		"a function is already set");
	AddChild(func);
	_function = func;
}

