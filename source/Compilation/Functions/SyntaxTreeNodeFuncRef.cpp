#include "SyntaxTreeNodeFuncRef.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "../SyntaxTreeNodeFuncArg.h"
#include "../SyntaxTreeNodeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncRef::SyntaxTreeNodeFuncRef(SourceCodeView sourceCode, ReadOnlyString name)
	: SyntaxTreeNodeFunc(sourceCode, name)
{
}

void SyntaxTreeNodeFuncRef::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "FuncRef(name=" << GetName() << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

SyntaxTreeNodeFuncRef* SyntaxTreeNodeFuncRef::Parse(const ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);
	auto ref = SyntaxTreeNodeRef::Parse(state, SyntaxTreeNodeRef::Func);
	auto funcRef = ARLANG_NEW SyntaxTreeNodeFuncRef(state->GetView(), ref->GetName());
	funcRef->AddChild(ref);
	return funcRef;
}

void SyntaxTreeNodeFuncRef::Resolve()
{
	SyntaxTreeNodeFunc::Resolve();

	if (!_definitions.IsEmpty())
		return;
	auto children = GetChildren();
	if (children.IsEmpty())
		return;

	// Find all definitions that's a type
	auto definitions = dynamic_cast<SyntaxTreeNodeRef*>(children[0])->GetDefinitions();
	for (auto def : definitions) {
		auto typeDef = dynamic_cast<SyntaxTreeNodeFuncDef*>(def);
		if (typeDef) {
			// Verify argument types
			auto expected = GetArguments();
			auto found = typeDef->GetArguments();
			if (expected.Size() != found.Size())
				continue;
			if (expected.Size() == 0)
				_definitions.Add(typeDef);
			for (auto i = 0; i < expected.Size(); ++i) {
				auto foundType = found[i]->GetType();
				auto expectedType = expected[i]->GetType();
				if (foundType->IsCompatibleWith(expectedType)) {
					_definitions.Add(typeDef);
				}
			}
		}
	}

	// Did we find at least one definition? If so then cleanup afterwards since the
	// references are no longer needed
	if (!_definitions.IsEmpty()) {
		DestroyChildren();
	}
}
