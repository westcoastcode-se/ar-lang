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

bool SyntaxTreeNodeFuncRef::Resolve(RecursiveDetector* detector)
{
	if (!SyntaxTreeNodeFunc::Resolve(detector))
		return false;

	auto children = GetChildren();
	if (children.IsEmpty())
		return true;

	// Find all definitions that's a type
	auto definitions = dynamic_cast<SyntaxTreeNodeRef*>(children[0])->GetDefinitions();
	for (auto def : definitions) {
		auto funcDef = dynamic_cast<SyntaxTreeNodeFuncDef*>(def);
		if (funcDef) {
			auto expected = GetArguments();
			auto found = funcDef->GetArguments();
			// Verify argument count
			if (expected.Size() != found.Size())
				continue;

			// Resolve this definition
			funcDef->Resolve(detector);

			// Verify that the arguments are compatible
			if (expected.Size() > 0)
			{
				for (auto i = 0; i < expected.Size(); ++i) {
					auto foundType = found[i]->GetType();
					auto expectedType = expected[i]->GetType();
					if (foundType->IsCompatibleWith(expectedType)) {
						_definitions.Add(funcDef);
					}
				}
			}
			else
			{
				_definitions.Add(funcDef);
			}

		}
	}

	// Did we find at least one definition? If so then cleanup afterwards since the
	// references are no longer needed
	if (!_definitions.IsEmpty()) {
		DestroyChildren();
	}

	return true;
}
