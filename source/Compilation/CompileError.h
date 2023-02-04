#pragma once

#include "Error.h"
#include "SourceCodeView.h"

namespace WestCoastCode::Compilation
{
	enum class CompileErrorType : int
	{
		UnresolvedReference,
		UnresolvedVarReference,
		UnresolvedTypeReference,
		UnresolvedFuncReference,
		IncompatibleTypes,
		RecursionDetected,
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

		void PrintToStderr() const final;

	private:
		SourceCodeView _sourceCode;
		const CompileErrorType _type;
	};

	class SyntaxTreeNodeRef;

	// Error raised if we couldn't resolve a specific reference
	class CompileErrorUnresolvedReference : public CompileError
	{
	public:
		CompileErrorUnresolvedReference(const SyntaxTreeNodeRef* reference);
	};

	class SyntaxTreeNodeVarRef;
	class SyntaxTreeNodeVarTypeRef;

	// Error raised if we couldn't resolve a specific variable reference
	class CompileErrorUnresolvedVarReference : public CompileError
	{
	public:
		CompileErrorUnresolvedVarReference(const SyntaxTreeNodeVarTypeRef* reference);
		CompileErrorUnresolvedVarReference(const SyntaxTreeNodeVarRef* reference);
	};

	class SyntaxTreeNodeTypeRef;

	// Error raised if we couldn't resolve a specific type reference
	class CompileErrorUnresolvedTypeReference : public CompileError
	{
	public:
		CompileErrorUnresolvedTypeReference(const SyntaxTreeNodeTypeRef* reference);
	};

	class SyntaxTreeNodeFuncRef;

	// Error raised if we couldn't resolve a specific function reference
	class CompileErrorUnresolvedFuncReference : public CompileError
	{
	public:
		CompileErrorUnresolvedFuncReference(const SyntaxTreeNodeFuncRef* reference);
	};

	class SyntaxTreeNode;

	// Error raised if we've reached functionality that's not implemented yet
	class CompileErrorNotImplemented : public CompileError
	{
	public:
		CompileErrorNotImplemented(const SyntaxTreeNode* node, const Char* feature);
	};

	class SyntaxTreeNodeType;

	/// @brief Error raised if two types are incompatible with each other
	class CompileErrorIncompatibleTypes : public CompileError
	{
	public:
		CompileErrorIncompatibleTypes(const SyntaxTreeNode* node, const SyntaxTreeNodeType* type1,
			const SyntaxTreeNodeType* type2);
	};

	/// @brief Error raised if invalid recursion is detected during compilation
	class CompileErrorRecursionDetected : public CompileError
	{
	public:
		CompileErrorRecursionDetected(const SyntaxTreeNode* node);
	};

}
