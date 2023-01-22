#include "SyntaxTreeNodeRef.h"
#include "SyntaxTreeNodePackage.h"
#include "SyntaxTreeNodeFuncDef.h"
#include "SyntaxTreeNodePrimitive.h"
#include "SyntaxTreeNodeFuncArg.h"

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
String ToStringQT(SyntaxTreeNodeRef::DefinitionQueryTypes op) {
	String value;
	for (I32 i = 0; i < 32; ++i) {
		const I32 bit = Bit(i);
		if (op & bit) {
			switch ((SyntaxTreeNodeRef::DefinitionQueryType)(bit))
			{
			case SyntaxTreeNodeRef::DefinitionQueryType::Package:
				value += "Package,";
				break;
			case SyntaxTreeNodeRef::DefinitionQueryType::Class:
				value += "Class,";
				break;
			case SyntaxTreeNodeRef::DefinitionQueryType::Func:
				value += "Func,";
				break;
			case SyntaxTreeNodeRef::DefinitionQueryType::Arg:
				value += "Arg,";
				break;
			case SyntaxTreeNodeRef::DefinitionQueryType::Local:
				value += "Local,";
				break;
			case SyntaxTreeNodeRef::DefinitionQueryType::Global:
				value += "Global,";
				break;
			case SyntaxTreeNodeRef::DefinitionQueryType::Member:
				value += "Member,";
				break;
			case SyntaxTreeNodeRef::DefinitionQueryType::Primitive:
				value += "Primitive,";
				break;
			}
		}
	}
	return value;
}

void SyntaxTreeNodeRef::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
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
	SyntaxTreeNode::ToString(s, indent);
}

void SyntaxTreeNodeRef::Resolve()
{
	if (!_definitions.IsEmpty())
		return;

	// Start resolving all references
	ResolveFromParent(GetParent());

	// Did we resolve anything?
	if (_definitions.IsEmpty())
		throw CompileErrorUnresolvedReference(this);

	// Get the last reference in the reference chain
	auto leaf = this;
	while (true) {
		auto children = leaf->GetChildren();
		if (children.IsEmpty())
			break;
		leaf = static_cast<SyntaxTreeNodeRef*>(children[0]);
	}

	// Verify that we resolved anything
	if (leaf->_definitions.IsEmpty())
		throw CompileErrorUnresolvedReference(this);

	// Take ownership of the definitions
	if (leaf == this)
		return;
	_definitions = leaf->_definitions;
}

SyntaxTreeNodeRef* SyntaxTreeNodeRef::Parse(const ParserState* state, DefinitionQueryTypes queryType,
	DefinitionQueryTypes sectionTypes)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Get the start of the string that represents the type we are trying to resolve
	const ReadOnlyString first = t->GetString();
	
	auto ref = ARLANG_NEW SyntaxTreeNodeRef(SourceCodeView(state->sourceCode, t), sectionTypes);
	ref->_name = t->GetString();
	auto mem = MemoryGuard(ref);
	auto section = ref;
	
	// Does the reference point to anyting more specific (using the dot delimiter)?
	t->Next();
	while (t->GetType() == TokenType::Dot) {
		if (t->Next() != TokenType::Identity)
			throw ParseErrorExpectedIdentity(state);
		auto newSection = ARLANG_NEW SyntaxTreeNodeRef(SourceCodeView(state->sourceCode, t), sectionTypes);
		newSection->_name = t->GetString();
		section->AddChild(newSection);
		section = newSection;
		t->Next();
	}
	section->_queryTypes = queryType;

	ref->_name = ReadOnlyString(first.data(), section->GetName().data() + section->GetName().length());
	return mem.Done();
}

void SyntaxTreeNodeRef::ResolveFromParent(SyntaxTreeNode* parent)
{
	class QueryTypeVisitor : public ISyntaxTreeNodeVisitor
	{
		SyntaxTreeNodeRef* section;
		const DefinitionQueryTypes queryTypes;
		const ReadOnlyString name;

	public:
		QueryTypeVisitor(SyntaxTreeNodeRef* section, DefinitionQueryTypes queryTypes,
			ReadOnlyString name)
			: section(section), queryTypes(queryTypes), name(name) {}

		VisitorResult Visit(SyntaxTreeNode* node) final {
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Package)) {
				auto impl = dynamic_cast<SyntaxTreeNodePackage*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Func)) {
				auto impl = dynamic_cast<SyntaxTreeNodeFuncDef*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Primitive)) {
				auto impl = dynamic_cast<SyntaxTreeNodePrimitive*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			if (BIT_ISSET(queryTypes, DefinitionQueryType::Arg)) {
				auto impl = dynamic_cast<SyntaxTreeNodeFuncArg*>(node);
				if (impl && impl->GetName() == name) {
					section->_definitions.Add(node);
					return VisitorResult::Continue;
				}
			}
			return VisitorResult::Continue;
		}

	} visitor(this, _queryTypes, _name);

	// If the first reference in the reference chain aren't a child of another reference
	// then perform a standard "upwards/backwards" query
	if (dynamic_cast<SyntaxTreeNodeRef*>(GetParent()) == nullptr) {
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
