#pragma once

#include "Error.h"
#include "SourceCodeParser.h"

namespace WestCoastCode::Compilation
{
	enum class ParseErrorType : int
	{
		ExpectedPackage,
		ExpectedIdentity,
		ExpectedConstant,
		NotImplemented,
		SyntaxError,
		IncompatibleTypes
	};

	// Error raised if parsing of a specific source code failed
	class ParseError : public CompilationError
	{
	public:
		ParseError(const SourceCode* sourceCode,
			const Token* token, ParseErrorType type);

		// The source code that generated the actual error
		const SourceCode* GetSourceCode() const {
			return _sourceCode;
		}

		// Return what type of error this is
		const ParseErrorType GetType() const { return _type; }

		// Get the filename where the error originated from
		const ReadOnlyString GetFilename() const;

		// Get the line which the error originated from
		const int GetLine() const { return _line; }

		// Where, on the line, the error originated from
		const int GetLineOffset() const { return _lineOffset; }

		// Where in the source code the error originated from
		const int GetOffset() const { return _offset; }

		// Print this error to stderr
		void PrintToStderr() const;

	private:
		const SourceCode* const _sourceCode;
		const ParseErrorType _type;
		const int _line;
		const int _lineOffset;
		const Char* _lineStart;
		const int _offset;
	};

	/// @brief Error raised if we expected a package
	class ParseErrorExpectedPackage : public ParseError
	{
	public:
		ParseErrorExpectedPackage(const ParserState* state);
	};

	/// @brief Error raised if we expected an identity
	class ParseErrorExpectedIdentity : public ParseError
	{
	public:
		ParseErrorExpectedIdentity(const ParserState* state);
	};

	/// @brief Error raised if we expected constant
	class ParseErrorExpectedConstant : public ParseError
	{
	public:
		ParseErrorExpectedConstant(const ParserState* state);
	};

	/// @brief Error raised if we've reached functionality that's not implemented yet
	class ParseErrorNotImplemented : public ParseError
	{
	public:
		ParseErrorNotImplemented(const ParserState* state);
	};

	/// @brief Error raised if the source contains basic syntax errors
	class ParseErrorSyntaxError : public ParseError
	{
	public:
		ParseErrorSyntaxError(const ParserState* state, const char* prefix);
	};

	class SyntaxTreeNodeType;

	/// @brief Error raised if two types are incompatible with each other
	class ParseErrorIncompatibleTypes : public ParseError
	{
	public:
		ParseErrorIncompatibleTypes(const ParserState* state, const SyntaxTreeNodeType* type1,
			const SyntaxTreeNodeType* type2);
	};
}
