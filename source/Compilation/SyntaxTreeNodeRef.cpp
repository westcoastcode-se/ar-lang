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
	s << _id << Indent(indent);
	s << "Ref(";
	s << "name=" << _name << ", ";
	s << "queryTypes=[" << ToStringQT(_queryTypes) << "], ";
	s << "definitions=[";
	for (int i = 0; i < _definitions.Size(); ++i) {
		if (i != 0)
			s << ",";
		s << _definitions[i]->GetID();
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

void SyntaxTreeNodeRef::ResolveReferences()
{
	if (!_definitions.IsEmpty())
		return;

	if (_children[0] == nullptr)
		throw CompileErrorUnresolvedReference(this);

	// Start resolving sections
	static_cast<SyntaxTreeNodeRefSection*>(_children[0])->ResolveFromParent(_parent);

	// Verify that we've found at least one defintiion
	if (static_cast<SyntaxTreeNodeRefSection*>(_children[0])->GetDefinitions().IsEmpty())
		throw CompileErrorUnresolvedReference(this);

	// Fetch the leaf node
	auto leaf = _children[0];
	while (true) {
		if (leaf->GetChildren().IsEmpty())
			break;
		leaf = leaf->GetChildren()[0];
	}

	_definitions = static_cast<SyntaxTreeNodeRefSection*>(leaf)->GetDefinitions();
	if (_definitions.IsEmpty())
		throw CompileErrorUnresolvedReference(this);
}

void SyntaxTreeNodeRef::SetSectionNode(SyntaxTreeNodeRefSection* section)
{
	_children.Add(section);
	section->SetParent(this);
}

SyntaxTreeNodeRef* SyntaxTreeNodeRef::Parse(const ParserState* state, DefinitionQueryTypes queryType,
	DefinitionQueryTypes sectionTypes)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Get the start of the string that represents the type we are trying to resolve
	const ReadOnlyString first = t->GetString();

	auto ref = ARLANG_NEW SyntaxTreeNodeRef(SourceCodeView(state->sourceCode, t), queryType);
	auto mem = MemoryGuard(ref);
	
	auto section = SyntaxTreeNodeRefSection::Parse(state, sectionTypes);
	ref->SetSectionNode(section);
	
	t->Next();

	while (t->GetType() == TokenType::Dot) {
		t->Next();
		auto newSection = SyntaxTreeNodeRefSection::Parse(state, sectionTypes);
		section->SetSubSection(newSection);
		section = newSection;
		t->Next();
	}
	section->SetQueryTypes(queryType);

	ref->_name = ReadOnlyString(first.data(), section->GetName().data() + section->GetName().length());
	return mem.Done();
}

SyntaxTreeNodeRef* SyntaxTreeNodeRef::Parse(const ParserState* state)
{
	return Parse(state, All, All);
}

SyntaxTreeNodeRefSection::~SyntaxTreeNodeRefSection()
{
	for (auto i : _children)
		delete i;
}

void SyntaxTreeNodeRefSection::ToString(StringStream& s, int indent) const
{
	s << _id << Indent(indent);
	s << "RefSection(";
	s << "name=" << _name << ", ";
	s << "queryTypes=[" << ToStringQT(_queryTypes) << "], ";
	s << "definitions=[";
	for (int i = 0; i < _definitions.Size(); ++i) {
		if (i != 0)
			s << ",";
		s << _definitions[i]->GetID();
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

void SyntaxTreeNodeRefSection::SetSubSection(SyntaxTreeNodeRefSection* section)
{
	_children.Add(section);
	section->SetParent(this);
}

void SyntaxTreeNodeRefSection::ResolveFromParent(ISyntaxTreeNode* parent)
{
	class QueryTypeVisitor : public ISyntaxTreeNodeVisitor
	{
		SyntaxTreeNodeRefSection* section;
		const DefinitionQueryTypes queryTypes;
		const ReadOnlyString name;

	public:
		QueryTypeVisitor(SyntaxTreeNodeRefSection* section, DefinitionQueryTypes queryTypes,
			ReadOnlyString name)
			: section(section), queryTypes(queryTypes), name(name) {}

		VisitorResult Visit(ISyntaxTreeNode* node) final {
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Package)) {
				auto impl = dynamic_cast<ISyntaxTreeNodePackage*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Func)) {
				auto impl = dynamic_cast<ISyntaxTreeNodeFuncDef*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Primitive)) {
				auto impl = dynamic_cast<ISyntaxTreeNodePrimitive*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Arg)) {
				auto impl = dynamic_cast<ISyntaxTreeNodeFuncArg*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			return VisitorResult::Continue;
		}

	} visitor(this, _queryTypes, _name);

	if (dynamic_cast<SyntaxTreeNodeRef*>(_parent)) {
		const QuerySearchFlags flags = (QuerySearchFlags)QuerySearchFlag::TraverseChildren
			| (QuerySearchFlags)QuerySearchFlag::TraverseParent
			| (QuerySearchFlags)QuerySearchFlag::Backwards
			| (QuerySearchFlags)QuerySearchFlag::TraverseImports;
		parent->Query(&visitor, flags);
	}
	else {
		const QuerySearchFlags flags = (QuerySearchFlags)QuerySearchFlag::TraverseChildren;
		parent->Query(&visitor, flags);
	}

}

SyntaxTreeNodeRefSection* SyntaxTreeNodeRefSection::Parse(const ParserState* state,
	DefinitionQueryTypes queryTypes)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	auto section = ARLANG_NEW SyntaxTreeNodeRefSection(SourceCodeView(state->sourceCode, t),
		t->GetString(), queryTypes);
	auto mem = MemoryGuard(section);

	return mem.Done();
}
