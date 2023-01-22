#include "SyntaxTreeNodeMultiType.h"
#include "SyntaxTreeNodeTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeMultiType::SyntaxTreeNodeMultiType(SourceCodeView view)
	: SyntaxTreeNodeTypeDef(view)
{
}

void SyntaxTreeNodeMultiType::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "MiltiType()" << std::endl;
	SyntaxTreeNodeTypeDef::ToString(s, indent);
}

SyntaxTreeNodeMultiType* SyntaxTreeNodeMultiType::Parse(const ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity && t->GetType() != TokenType::ParantLeft)
		throw ParseErrorExpectedIdentity(state);

	auto types = ARLANG_NEW SyntaxTreeNodeMultiType(SourceCodeView(state->sourceCode, t));
	auto guard = MemoryGuard(types);

	while (t->Next() != TokenType::ParantRight)
	{
		types->AddChild(SyntaxTreeNodeTypeRef::Parse(state));
		// Ignore comma
		if (t->GetType() == TokenType::Comma) {
			t->Next();
		}
	}
	return guard.Done();
}
