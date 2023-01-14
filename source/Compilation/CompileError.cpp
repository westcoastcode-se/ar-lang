#include "CompileError.h"
#include "SyntaxTreeNode.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

CompileError::CompileError(SourceCodeView sourceCode, CompileErrorType type)
	: CompilationError(), _sourceCode(sourceCode), _type(type)
{
}

CompileErrorUnresolvedReference::CompileErrorUnresolvedReference(const ISyntaxTreeNodeRef* reference)
	: CompileError(*reference->GetSourceCode(), CompileErrorType::UnresolvedReference)
{
	StringStream s;
	s << "reference '" << reference->GetName() << "' could not be resolved";
	SetError(s.str());
}
