#include "CompileError.h"
#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodeRef.h"
#include "Types/SyntaxTreeNodeTypeRef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

CompileError::CompileError(SourceCodeView sourceCode, CompileErrorType type)
	: CompilationError(), _sourceCode(sourceCode), _type(type)
{
}

CompileErrorUnresolvedReference::CompileErrorUnresolvedReference(const SyntaxTreeNodeRef* reference)
	: CompileError(reference->GetSourceCode(), CompileErrorType::UnresolvedReference)
{
	StringStream s;
	s << "reference '" << reference->GetName() << "' could not be resolved";
	SetError(s.str());
}

CompileErrorUnresolvedTypeReference::CompileErrorUnresolvedTypeReference(const SyntaxTreeNodeTypeRef* reference)
	: CompileError(reference->GetSourceCode(), CompileErrorType::UnresolvedTypeReference)
{
	StringStream s;
	s << "reference '" << reference->GetName() << "' could not be resolved";
	SetError(s.str());
}

CompileErrorNotImplemented::CompileErrorNotImplemented(const SyntaxTreeNode* node, const Char* feature)
	: CompileError(node->GetSourceCode(), CompileErrorType::NotImplemented)
{
	StringStream s;
	s << "feature '" << feature << "' is not implemented yet";
	SetError(s.str());
}
