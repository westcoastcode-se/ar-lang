#include "SyntaxTreeNodeFuncArg.h"
#include "Types/SyntaxTreeNodeTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeFuncArg::SyntaxTreeNodeFuncArg(SourceCodeView view, ReadOnlyString name, SyntaxTreeNodeTypeRef* type)
	: SyntaxTreeNode(view), _name(name), _type(type)
{
	AddChild(type);
}

void SyntaxTreeNodeFuncArg::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "FuncArg(name=" << _name << ", type=" << _type->GetID() << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

SyntaxTreeNodeFuncArg* SyntaxTreeNodeFuncArg::Parse(ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// The name of the argument
	const ReadOnlyString name = t->GetString();
	t->Next();

	auto type = SyntaxTreeNodeTypeRef::Parse(state);
	return ARLANG_NEW SyntaxTreeNodeFuncArg(SourceCodeView(state->sourceCode, t), name, type);
}
