#include "SyntaxTreeNodeFunc.h"
#include "SyntaxTreeNodeFuncBody.h"
#include "SyntaxTreeNodeFuncArg.h"
#include "SyntaxTreeNodeTypeRef.h"
#include "SyntaxTreeNodeMultiType.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFunc::SyntaxTreeNodeFunc(SourceCodeView view, ReadOnlyString name)
	: SyntaxTreeNode(view), _name(name), _returnType(nullptr), _body(nullptr), _symbol(nullptr)
{
}

void SyntaxTreeNodeFunc::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "FuncDef(name=" << _name << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

void SyntaxTreeNodeFunc::Compile(Builder::Linker* linker)
{
	if (_symbol == nullptr) {
		_symbol = new Builder::Function(_name);

		// TODO: Add support for functions inside another function
		auto package = GetPackage();
		auto symbol = static_cast<SyntaxTreeNodePackage*>(package)->GetSymbol();
		symbol->Add(_symbol);
	}

	// Compile children so that we have valid symbols for arguments and return types
	for (auto child : GetChildren())
		child->Compile(linker);
	
	// Does this function have a return type?
	if (_returnType) {
		// Assume that the _returnType is a SyntaxTreeNodeTypeRef if the type is null
		auto type = _returnType->GetType();
		if (type == nullptr)
			throw CompileErrorUnresolvedTypeReference(static_cast<SyntaxTreeNodeTypeRef*>(_returnType));

		// TODO: Add support for other types than primitives
		auto definition = dynamic_cast<SyntaxTreeNodePrimitive*>(type);
		assert(definition != nullptr && "only primitives are allowed at the moment");
		_symbol->AddReturn(definition->GetSymbol());
	}
}

SyntaxTreeNodePackage* SyntaxTreeNodeFunc::GetPackage()
{
	// TODO: Add support for functions inside another function
	return dynamic_cast<SyntaxTreeNodePackage*>(GetParent());
}

void SyntaxTreeNodeFunc::SetBody(SyntaxTreeNodeFuncBody* body)
{
	_body = body;
}

void SyntaxTreeNodeFunc::AddArgument(SyntaxTreeNodeFuncArg* arg)
{
	AddChild(arg);
	_arguments.Add(arg);
}

void SyntaxTreeNodeFunc::SetReturnType(SyntaxTreeNodeTypeDef* returnType)
{
	AddChild(returnType);
	_returnType = returnType;
}

SyntaxTreeNodeFunc* SyntaxTreeNodeFunc::Parse(ParserState* state)
{
	Token* const t = state->token;
	if (t->Next() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Function
	auto funcdef = ARLANG_NEW SyntaxTreeNodeFunc(SourceCodeView(state->sourceCode, t), t->GetString());
	auto guard = MemoryGuard(funcdef);

	// Function arguments list has to start with a (
	if (t->Next() != TokenType::ParantLeft)
		throw ParseErrorSyntaxError(state, "expected '('");
	
	// Parse all arguments: (name type[, ...])
	if (t->Next() != TokenType::ParantRight)
{
		while (true)
		{
			funcdef->AddArgument(SyntaxTreeNodeFuncArg::Parse(state));

			// Ignore comma
			if (t->GetType() == TokenType::Comma) {
				t->Next();
			}
			else if (t->GetType() == TokenType::ParantRight) {
				break;
			}
		}
	}
	t->Next();

	// Parse return types. Paranteses are optional if you only have one return. 
	// You are also allowed to not have a return type at all.
	if (t->GetType() == TokenType::Identity)
		funcdef->SetReturnType(SyntaxTreeNodeTypeRef::Parse(state));
	else if (t->GetType() == TokenType::ParantLeft)
		funcdef->SetReturnType(SyntaxTreeNodeMultiType::Parse(state));
	return guard.Done();
}
