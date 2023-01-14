#include "SyntaxTreeNodeTypeRef.h"
#include "SyntaxTreeNodeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeTypeRef::SyntaxTreeNodeTypeRef(SourceCodeView sourceCode)
	: _parent(nullptr), _sourceCode(sourceCode)
{
}

SyntaxTreeNodeTypeRef::~SyntaxTreeNodeTypeRef()
{
	for (int i = 0; i < _children.Size(); ++i)
		delete _children[i];
}

void SyntaxTreeNodeTypeRef::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "TypeRef(name=" << _fullName << ", definitions=[";
	for (int i = 0; i < _definitions.Size(); ++i) {
		if (i != 0)
			s << ",";
		char tmp[32];
		sprintf(tmp, "%p", _definitions[i]);
		s << tmp;
	}
	s << "])" << std::endl;
	for (int i = 0; i < _children.Size(); ++i)
		_children[i]->ToString(s, indent + 1);
}

ISyntaxTree* SyntaxTreeNodeTypeRef::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeTypeRef::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeTypeRef::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

void SyntaxTreeNodeTypeRef::AddNode(ISyntaxTreeNode* node)
{
	_children.Add(node);
	node->SetParent(this);
}

SyntaxTreeNodeTypeRef* SyntaxTreeNodeTypeRef::Parse(ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	auto typeref = new SyntaxTreeNodeTypeRef(SourceCodeView(state->sourceCode, t));
	auto mem = MemoryGuard(typeref);
	auto ref = SyntaxTreeNodeRef::Parse(state, DefinitionQueryType::Type, 
		(DefinitionQueryTypes)DefinitionQueryType::All);
	typeref->AddNode(ref);
	typeref->_fullName = ref->GetName();
	return mem.Done();
}
