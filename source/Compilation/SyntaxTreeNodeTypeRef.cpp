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
	for (auto c : _children)
		delete c;
}

void SyntaxTreeNodeTypeRef::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
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

void SyntaxTreeNodeTypeRef::ResolveReferences()
{
	if (!_definitions.IsEmpty())
		return;

	Default::ResolveReferences(this);

	auto definitions = dynamic_cast<SyntaxTreeNodeRef*>(_children[0])->GetDefinitions();
	for (auto def : definitions) {
		auto typeDef = dynamic_cast<ISyntaxTreeNodeType*>(def);
		if (typeDef)
			_definitions.Add(typeDef);
	}

	if (!_definitions.IsEmpty()) {
		// No need for references if we've found definitions
		for (auto c : _children)
			delete c;
		_children.Clear();
	}
}

void SyntaxTreeNodeTypeRef::AddNode(ISyntaxTreeNode* node)
{
	_children.Add(node);
	node->SetParent(this);
}

SyntaxTreeNodeTypeRef* SyntaxTreeNodeTypeRef::Parse(const ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	auto typeref = ARLANG_NEW SyntaxTreeNodeTypeRef(SourceCodeView(state->sourceCode, t));
	auto mem = MemoryGuard(typeref);
	auto ref = SyntaxTreeNodeRef::Parse(state, ISyntaxTreeNodeRef::Type, ISyntaxTreeNodeRef::All);
	typeref->AddNode(ref);
	typeref->_fullName = ref->GetName();
	return mem.Done();
}
