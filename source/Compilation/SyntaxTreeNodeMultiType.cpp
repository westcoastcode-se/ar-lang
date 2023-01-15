#include "SyntaxTreeNodeMultiType.h"
#include "SyntaxTreeNodeTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeMultiType::SyntaxTreeNodeMultiType(SourceCodeView sourceCode)
	: _parent(nullptr), _sourceCode(sourceCode)
{
}

SyntaxTreeNodeMultiType::~SyntaxTreeNodeMultiType()
{
	for (int i = 0; i < _children.Size(); ++i)
		delete _children[i];
}

void SyntaxTreeNodeMultiType::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "Types(definitions=[";
	for (int i = 0; i < _definitions.Size(); ++i) {
		if (i != 0)
			s << ",";
		s << _definitions[i]->GetID();
	}
	s << "])" << std::endl;
	for (int i = 0; i < _children.Size(); ++i)
		_children[i]->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeMultiType::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeMultiType::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeMultiType::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeMultiType::ResolveReferences()
{
	if (!_definitions.IsEmpty())
		return;

	// Resolve children first
	Default::ResolveReferences(this);
}

void SyntaxTreeNodeMultiType::AddType(ISyntaxTreeNodeType* type)
{
	type->SetParent(this);
	_children.Add(type);
}

SyntaxTreeNodeMultiType* SyntaxTreeNodeMultiType::Parse(const ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity && t->GetType() != TokenType::ParantLeft)
		throw ParseErrorExpectedIdentity(state);

	auto types = new SyntaxTreeNodeMultiType(SourceCodeView(state->sourceCode, t));
	auto guard = MemoryGuard(types);

	while (t->Next() != TokenType::ParantRight)
	{
		types->AddType(SyntaxTreeNodeTypeRef::Parse(state));
		// Ignore comma
		if (t->GetType() == TokenType::Comma) {
			t->Next();
		}
	}
	return guard.Done();
}
