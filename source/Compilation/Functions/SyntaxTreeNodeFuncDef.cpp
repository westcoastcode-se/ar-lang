#include "SyntaxTreeNodeFuncDef.h"
#include "../SyntaxTreeNodePackage.h"
#include "../Types/SyntaxTreeNodePrimitive.h"
#include "../Types/SyntaxTreeNodeTypeRef.h"
#include "../Types/SyntaxTreeNodeTypeRef.h"
#include "../Types/SyntaxTreeNodeMultiType.h"
#include "../SyntaxTreeNodeFuncArg.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncDef::SyntaxTreeNodeFuncDef(SourceCodeView view, ReadOnlyString name)
	: SyntaxTreeNodeFunc(view, name), _body(nullptr), _symbol(nullptr)
{}

void SyntaxTreeNodeFuncDef::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "FuncDef(name=" << GetName() << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

void SyntaxTreeNodeFuncDef::Compile(Builder::Linker* linker)
{
	if (_symbol == nullptr) {
		_symbol = new Builder::Function(GetName());

		// TODO: Add support for functions inside another function
		auto package = dynamic_cast<SyntaxTreeNodePackage*>(GetParent());
		auto symbol = package->GetSymbol();
		symbol->Add(_symbol);
	}

	// Compile children so that we have valid symbols for arguments and return types
	for (auto child : GetChildren())
		child->Compile(linker);

	// Does this function have a return type?
	if (GetReturnType()) {
		// Assume that the _returnType is a SyntaxTreeNodeTypeRef if the type is null
		auto type = GetReturnType()->GetType();
		if (type == nullptr)
			throw CompileErrorUnresolvedTypeReference(static_cast<SyntaxTreeNodeTypeRef*>(GetReturnType()));

		// TODO: Add support for other types than primitives
		auto definition = dynamic_cast<SyntaxTreeNodePrimitive*>(type);
		assert(definition != nullptr && "only primitives are allowed at the moment");
		_symbol->AddReturn(definition->GetSymbol());
	}
}

void SyntaxTreeNodeFuncDef::SetBody(SyntaxTreeNodeFuncDefBody* body)
{
	_body = body;
}

SyntaxTreeNodeFuncDef* SyntaxTreeNodeFuncDef::Parse(ParserState* state)
{
	Token* const t = state->token;
	if (t->Next() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Function
	auto funcdef = ARLANG_NEW SyntaxTreeNodeFuncDef(SourceCodeView(state->sourceCode, t), t->GetString());
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
