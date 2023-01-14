#include "SyntaxTreeNodeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

namespace
{
	static I32 Bit(int i)
	{
		return 1 << i;
	}
}

// Stringify the operator
String ToStringQT(ISyntaxTreeNodeRef::DefinitionQueryTypes op) {
	String value;
	for (I32 i = 0; i < 32; ++i) {
		const I32 bit = Bit(i);
		if (op & bit) {
			switch ((ISyntaxTreeNodeRef::DefinitionQueryType)(bit))
			{
			case ISyntaxTreeNodeRef::DefinitionQueryType::Package:
				value += "Package,";
				break;
			case ISyntaxTreeNodeRef::DefinitionQueryType::Class:
				value += "Class,";
				break;
			case ISyntaxTreeNodeRef::DefinitionQueryType::Func:
				value += "Func,";
				break;
			case ISyntaxTreeNodeRef::DefinitionQueryType::Arg:
				value += "Arg,";
				break;
			case ISyntaxTreeNodeRef::DefinitionQueryType::Local:
				value += "Local,";
				break;
			case ISyntaxTreeNodeRef::DefinitionQueryType::Global:
				value += "Global,";
				break;
			case ISyntaxTreeNodeRef::DefinitionQueryType::Member:
				value += "Member,";
				break;
			case ISyntaxTreeNodeRef::DefinitionQueryType::Primitive:
				value += "Primitive,";
				break;
			}
		}
	}
	return value;
}

SyntaxTreeNodeRef::~SyntaxTreeNodeRef()
{
	for (auto i : _children)
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

void SyntaxTreeNodeRef::ToString(StringStream& s, int indent) const
{
	s << Indent(indent);
	s << "Ref(";
	s << "name=" << _name << ", ";
	s << "queryTypes=[" << ToStringQT(_queryTypes) << "], ";
	s << "definitions=[";
	for (int i = 0; i < _definitions.Size(); ++i) {
		if (i != 0)
			s << ",";
		char tmp[32];
		sprintf(tmp, "%p", _definitions[i]);
		s << tmp;
	}
	s << "])";
	s << std::endl;
	for (auto i : _children) {
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
	_children.Add(section);
	section->SetParent(this);
}

SyntaxTreeNodeRef* SyntaxTreeNodeRef::Parse(ParserState* state, DefinitionQueryTypes queryType,
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

	while (t->GetType() == TokenType::Dot) {
		t->Next();
		auto newSection = SyntaxTreeNodeRefSection::Parse(state, sectionTypes);
		section->AddNode(newSection);
		section = newSection;
		t->Next();
	}
	section->SetQueryTypes(queryType);

	ref->_name = ReadOnlyString(first.data(), section->GetName().data() + section->GetName().length());
	return mem.Done();
}

SyntaxTreeNodeRef* SyntaxTreeNodeRef::Parse(ParserState* state)
{
	return Parse(state, All, All);
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
	s << "name=" << _name << ", ";
	s << "queryTypes=[" << ToStringQT(_queryTypes) << "], ";
	s << "definitions=[";
	for (int i = 0; i < _definitions.Size(); ++i) {
		if (i != 0)
			s << ",";
		char tmp[32];
		sprintf(tmp, "%p", _definitions[i]);
		s << tmp;
	}
	s << "])";
	s << std::endl;
	for (auto i : _children) {
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

void SyntaxTreeNodeRefSection::AddNode(SyntaxTreeNodeRefSection* section)
{
	_children.Add(section);
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
