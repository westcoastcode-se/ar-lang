#include "SyntaxTreeNodeVarTypeRef.h"
#include "../SyntaxTreeNodeVar.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeVarTypeRef::SyntaxTreeNodeVarTypeRef(SourceCodeView sourceCode, SyntaxTreeNodeRef* ref)
	: SyntaxTreeNodeType(sourceCode), _reference(ref)
{
}

void SyntaxTreeNodeVarTypeRef::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "VarTypeRef(ref=" << _reference->GetID() << ")" << std::endl;
	SyntaxTreeNodeType::ToString(s, indent);
}

bool SyntaxTreeNodeVarTypeRef::Resolve(RecursiveDetector* detector)
{
	if (!SyntaxTreeNodeType::Resolve(detector))
		return false;

	// Resolve the reference
	_reference->Resolve(detector);

	// Definitions found are variables. So we don't really know the type yet
	auto definitions = _reference->GetDefinitions();
	for (auto def : definitions) {
		auto var = dynamic_cast<SyntaxTreeNodeVar*>(def);
		if (var) {
			var->Resolve(detector);
			_definitions.Add(var);
		}
	}

	if (_definitions.IsEmpty()) {
		throw CompileErrorUnresolvedVarReference(this);
	}

	return true;
}

SyntaxTreeNodeType* SyntaxTreeNodeVarTypeRef::GetType()
{
	if (_definitions.Size() == 0)
		return nullptr;
	return _definitions[0]->GetType();
}
