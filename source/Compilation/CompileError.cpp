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

void CompileError::PrintToStderr() const
{
	auto source = GetSourceCode();
	fprintf(stderr, "Failed to compile %.*s:\n\n",
		(I32)source->GetSourceCode()->GetFilename().length(), source->GetSourceCode()->GetFilename().data());

	auto text = source->GetSourceCode()->GetText();
	const char* s = text.data();
	const char* end = s + text.length();
	int line = 0;
	const auto lineOffset = _sourceCode.GetLineOffset();
	const auto lineStart = _sourceCode.GetLineStart();
	for (; s != end; ++s) {
		fprintf(stderr, "%c", *s);
		if (*s == '\n') {
			if ((++line) == _sourceCode.GetLine() + 1) {
				for (int i = 0; i < lineOffset; ++i) {
					if (lineStart[i] == '\t')
						fprintf(stderr, "\t");
					else
						fprintf(stderr, " ");
				}
				fprintf(stderr, "^ C%06d: %s\n", (I32)_type, what());
			}
		}
	}
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

CompileErrorIncompatibleTypes::CompileErrorIncompatibleTypes(const SyntaxTreeNode* node, 
	const SyntaxTreeNodeType* type1,
	const SyntaxTreeNodeType* type2)
	: CompileError(node->GetSourceCode(), CompileErrorType::NotImplemented)
{
	StringStream s;
	s << type1->GetName() << " is not compatible with " << type2->GetName();
	SetError(s.str());
}
