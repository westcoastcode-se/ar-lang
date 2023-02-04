#include "SyntaxTreeNodeVarArg.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeVarArg::SyntaxTreeNodeVarArg(SourceCodeView sourceCode, ReadOnlyString name)
	: SyntaxTreeNodeVarDef(sourceCode, name), _function(nullptr), _type(nullptr)
{
}

void SyntaxTreeNodeVarArg::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "VarArg(name=" << GetName() << ")" << std::endl;
	SyntaxTreeNodeVarDef::ToString(s, indent);
}

bool SyntaxTreeNodeVarArg::Resolve(RecursiveDetector* detector)
{
	if (!SyntaxTreeNodeVarDef::Resolve(detector))
		return false;
	return true;
}
