#include "SyntaxTreeNodeTypeRef.h"
#include "../SyntaxTreeNodeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeTypeRef::SyntaxTreeNodeTypeRef(SourceCodeView sourceCode, ReadOnlyString name)
	: SyntaxTreeNodeType(sourceCode), _name(name)
{
}

void SyntaxTreeNodeTypeRef::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "TypeRef(name=" << _name << ", definitions=[";
	for (int i = 0; i < _definitions.Size(); ++i) {
		if (i != 0)
			s << ",";
		s << _definitions[i]->GetID();
	}
	s << "])" << std::endl;
	SyntaxTreeNodeType::ToString(s, indent);
}

SyntaxTreeNodeType* SyntaxTreeNodeTypeRef::GetType()
{
	if (_definitions.IsEmpty())
		return this;
	return _definitions[0];
}

bool SyntaxTreeNodeTypeRef::Resolve(RecursiveDetector* detector)
{
	if (!SyntaxTreeNodeType::Resolve(detector))
		return false;

	auto children = GetChildren();
	if (children.IsEmpty())
		return true;

	// Find all definitions that's a type
	auto definitions = dynamic_cast<SyntaxTreeNodeRef*>(children[0])->GetDefinitions();
	for (auto def : definitions) {
		auto typeDef = dynamic_cast<SyntaxTreeNodeType*>(def);
		if (typeDef) {
			// Resolve this definition
			typeDef->Resolve(detector);
			_definitions.Add(typeDef);
		}
	}

	// Did we find at least one definition? If so then cleanup afterwards since the
	// references are no longer needed
	if (!_definitions.IsEmpty()) {
		DestroyChildren();
	}

	return true;
}

void SyntaxTreeNodeTypeRef::OnChildAdded(SyntaxTreeNode* child)
{
	assert(dynamic_cast<SyntaxTreeNodeRef*>(child) != nullptr &&
		"a SyntaxTreeNodeTypeRef is onlty allowed to have SyntaxTreeNodeRef children");
}

SyntaxTreeNodeTypeRef* SyntaxTreeNodeTypeRef::Parse(const ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	auto ref = SyntaxTreeNodeRef::Parse(state, SyntaxTreeNodeRef::Type);

	auto typeref = ARLANG_NEW SyntaxTreeNodeTypeRef(SourceCodeView(state->sourceCode, t), ref->GetName());
	auto mem = MemoryGuard(typeref);
	typeref->AddChild(ref);
	return mem.Done();
}
