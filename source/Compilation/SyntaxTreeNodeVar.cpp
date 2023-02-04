#include "SyntaxTreeNodeVar.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

SyntaxTreeNodeVar::SyntaxTreeNodeVar(SourceCodeView sourceCode, ReadOnlyString name)
	: SyntaxTreeNode(sourceCode), _name(name)
{
}
