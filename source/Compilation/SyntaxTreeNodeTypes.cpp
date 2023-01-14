#include "SyntaxTreeNodeTypes.h"
#include "SyntaxTreeNodeTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeTypes::SyntaxTreeNodeTypes(SourceCodeView sourceCode)
	: _parent(nullptr), _sourceCode(sourceCode)
{
}

SyntaxTreeNodeTypes::~SyntaxTreeNodeTypes()
{
	for (int i = 0; i < _children.Size(); ++i)
		delete _children[i];
}

void SyntaxTreeNodeTypes::ToString(StringStream& s, int indent) const
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

ISyntaxTree* SyntaxTreeNodeTypes::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeTypes::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeTypes::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeTypes::ResolveReferences()
{
	if (!_definitions.IsEmpty())
		return;

	// Resolve children first
	Default::ResolveReferences(this);
}

void SyntaxTreeNodeTypes::AddNode(ISyntaxTreeNode* node)
{
	node->SetParent(this);
	_children.Add(node);
}

SyntaxTreeNodeTypes* SyntaxTreeNodeTypes::Parse(const ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity && t->GetType() != TokenType::ParantLeft)
		throw ParseErrorExpectedIdentity(state);

	auto types = new SyntaxTreeNodeTypes(SourceCodeView(state->sourceCode, t));
	auto guard = MemoryGuard(types);

	// Parse return types. Paranteses are optional if you only have one return. 
	// You are also allowed to not have a return type at all.
	if (t->GetType() == TokenType::Identity)
	{
		types->AddNode(SyntaxTreeNodeTypeRef::Parse(state));
	}
	else if (t->GetType() == TokenType::ParantLeft)
	{
		while (t->Next() != TokenType::ParantRight)
		{
			types->AddNode(SyntaxTreeNodeTypeRef::Parse(state));
			// Ignore comma
			if (t->GetType() == TokenType::Comma) {
				t->Next();
			}
		}
	}
	return guard.Done();
}
