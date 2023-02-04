#include "SyntaxTreeNodeVarRef.h"
#include "../SyntaxTreeNodeRef.h"
#include "SyntaxTreeNodeVarDef.h"
#include "../Types/SyntaxTreeNodeVarTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeVarRef::SyntaxTreeNodeVarRef(SourceCodeView sourceCode, ReadOnlyString name)
	: SyntaxTreeNodeVar(sourceCode, name), _type(nullptr)
{
}

void SyntaxTreeNodeVarRef::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "VarRef(name=" << GetName() << ")" << std::endl;
	SyntaxTreeNode::ToString(s, indent);
}

bool SyntaxTreeNodeVarRef::Resolve(RecursiveDetector* detector)
{
	if (!SyntaxTreeNodeVar::Resolve(detector))
		return false;

	auto resolvedType = _type->GetType();
	ReplaceChild(_type, resolvedType);
	_type = resolvedType;

	// Find all definitions and resolve them
	auto children = GetChildren();
	auto definitions = dynamic_cast<SyntaxTreeNodeRef*>(children[0])->GetDefinitions();
	for (auto def : definitions) {
		auto varDef = dynamic_cast<SyntaxTreeNodeVarDef*>(def);
		if (varDef == nullptr)
			continue;

		RecursiveDetector childDetector(detector, this);
		varDef->Resolve(&childDetector);
		_definitions.Add(varDef);
	}

	if (_definitions.IsEmpty()) {
		throw CompileErrorUnresolvedVarReference(this);
	}

	// Did we find at least one definition? If so then cleanup afterwards since the
	// references are no longer needed
	if (!_definitions.IsEmpty()) {
		DestroyChildren();
	}

	return true;
}

SyntaxTreeNodeVarRef* SyntaxTreeNodeVarRef::Parse(const ParserState* state)
{
	Token* const t = state->token;
	if (t->GetType() != TokenType::Identity)
		throw ParseErrorExpectedIdentity(state);

	// Reference
	auto ref = SyntaxTreeNodeRef::Parse(state, SyntaxTreeNodeRef::Variables);

	// The variable
	auto var = ARLANG_NEW SyntaxTreeNodeVarRef(ref->GetSourceCode(), ref->GetName());
	auto type = ARLANG_NEW SyntaxTreeNodeVarTypeRef(ref->GetSourceCode(), ref);
	var->AddChild(ref);
	var->AddChild(type);
	var->_type = type;
	return var;
}
