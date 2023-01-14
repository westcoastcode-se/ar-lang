#pragma once

#include "Error.h"
#include "SourceCodeView.h"

namespace WestCoastCode::Compilation
{
	enum class CompileErrorType : int
	{
		UnresolvedReference,
		NotImplemented,
	};

	// Error raised if parsing of a specific source code failed
	class CompileError : public CompilationError
	{
	public:
		CompileError(SourceCodeView sourceCode, CompileErrorType type);

		// The source code that generated the actual error
		const SourceCodeView* GetSourceCode() const {
			return &_sourceCode;
		}

		// Get the filename where the error originated from
		const ReadOnlyString GetFilename() const { return _sourceCode.GetSourceCode()->GetFilename(); }

		// Get the line which the error originated from
		const int GetLine() const { return _sourceCode.GetLine(); }

		// Where, on the line, the error originated from
		const int GetLineOffset() const { return _sourceCode.GetLineOffset(); }

		// Where in the source code the error originated from
		const int GetOffset() const { return _sourceCode.GetOffset(); }

	private:
		SourceCodeView _sourceCode;
		const CompileErrorType _type;
	};

	class ISyntaxTreeNodeRef;

	// Error raised if we couldn't resolve a specific reference
	class CompileErrorUnresolvedReference : public CompileError
	{
	public:
		CompileErrorUnresolvedReference(const ISyntaxTreeNodeRef* reference);
	};
}
