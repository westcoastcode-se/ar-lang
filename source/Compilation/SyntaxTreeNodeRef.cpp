#include "SyntaxTreeNodeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeRef::~SyntaxTreeNodeRef()
{
	for (auto&& i : _children)
		delete i;
}

ISyntaxTree* SyntaxTreeNodeRef::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

void SyntaxTreeNodeRef::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

bool SyntaxTreeNodeRef::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	return false;
}

bool SyntaxTreeNodeRef::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

bool SyntaxTreeNodeRef::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

void SyntaxTreeNodeRef::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "Ref(";
	s << "queryTypes=" << _queryTypes << ", ";
	s << "definitions=[";
	for (int i = 0; i < _definitions.size(); ++i) {
		if (i != 0)
			s << ",";
		char tmp[32];
		sprintf(tmp, "%p", _definitions[i]);
		s << tmp;
	}
	s << "])";
	s << std::endl;
	for (auto&& i : _children) {
		i->ToString(s, indent + 1);
	}
}

ISyntaxTreeNode* SyntaxTreeNodeRef::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeRef::AddNode(SyntaxTreeNodeRefSection* section)
{
	_children.push_back(section);
	section->SetParent(this);
}

SyntaxTreeNodeRef* SyntaxTreeNodeRef::Parse(ParserState* state, DefinitionQueryType queryType,
	DefinitionQueryTypes sectionTypes)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Get the start of the string that represents the type we are trying to resolve
	const ReadOnlyString first = t->GetString();

	auto ref = new SyntaxTreeNodeRef(SourceCodeView(state->sourceCode, t), queryType);
	auto mem = MemoryGuard(ref);
	
	auto section = SyntaxTreeNodeRefSection::Parse(state, sectionTypes);
	ref->AddNode(section);
	
	t->Next();
	if (t->GetType() != TokenType::Dot) {
		section->SetQueryTypes((DefinitionQueryTypes)queryType);
	}
	else {
		while (t->GetType() == TokenType::Dot) {
			t->Next();
			auto newSection = SyntaxTreeNodeRefSection::Parse(state, sectionTypes);
			section->AddNode(section);
			section = newSection;
			t->Next();
		}
	}
	ref->_name = ReadOnlyString(first.data(), t->GetString().data());
	return mem.Done();
}

SyntaxTreeNodeRef* SyntaxTreeNodeRef::Parse(ParserState* state)
{
	return Parse(state, DefinitionQueryType::All, (DefinitionQueryTypes)DefinitionQueryType::All);
}

SyntaxTreeNodeRefSection::~SyntaxTreeNodeRefSection()
{
	for (auto&& i : _children)
		delete i;
}

void SyntaxTreeNodeRefSection::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "RefSection(";
	s << "queryTypes=" << _queryTypes << ", ";
	s << "definitions=[";
	for (int i = 0; i < _definitions.size(); ++i) {
		if (i != 0)
			s << ",";
		char tmp[32];
		sprintf(tmp, "%p", _definitions[i]);
		s << tmp;
	}
	s << "])";
	s << std::endl;
	for (auto&& i : _children) {
		i->ToString(s, indent + 1);
	}
}

ISyntaxTree* SyntaxTreeNodeRefSection::GetSyntaxTree() const
{
	return _parent->GetSyntaxTree();
}

ISyntaxTreeNode* SyntaxTreeNodeRefSection::GetRootNode()
{
	if (_parent)
		return _parent->GetRootNode();
	return this;
}

void SyntaxTreeNodeRefSection::SetParent(ISyntaxTreeNode* parent)
{
	_parent = parent;
}

bool SyntaxTreeNodeRefSection::Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const
{
	return false;
}

bool SyntaxTreeNodeRefSection::Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

bool SyntaxTreeNodeRefSection::Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor)
{
	return false;
}

void SyntaxTreeNodeRefSection::AddNode(SyntaxTreeNodeRefSection* section)
{
	_children.push_back(section);
	section->SetParent(this);
}

SyntaxTreeNodeRefSection* SyntaxTreeNodeRefSection::Parse(ParserState* state,
	DefinitionQueryTypes queryTypes)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	auto section = new SyntaxTreeNodeRefSection(SourceCodeView(state->sourceCode, t),
		t->GetString(), queryTypes);
	auto mem = MemoryGuard(section);

	return mem.Done();
}
