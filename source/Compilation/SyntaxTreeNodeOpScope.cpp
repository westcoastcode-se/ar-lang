#include "SyntaxTreeNodeOpScope.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

void SyntaxTreeNodeOpScope::ToString(StringStream& s, int indent) const
{
	s << GetID() << Indent(indent);
	s << "Scope(locals=?)" << std::endl;
	SyntaxTreeNodeOp::ToString(s, indent);
}
