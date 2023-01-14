#pragma once

#include <exception>
#include <cstdarg>
#include "SourceCodeParser.h"

namespace WestCoastCode::Compilation
{
	enum class ParseErrorType : int
	{
		ExpectedIdentity,
		ExpectedConstant,
		NotImplemented,
		SyntaxError,
	};

	// Error raised if parsing of a specific source code failed
	class ParseError : public std::exception
	{
	public:
		ParseError(const SourceCode* sourceCode,
			const Token* token, ParseErrorType type);

		// The source code that generated the actual error
		const SourceCode* GetSourceCode() const {
			return _sourceCode;
		}

		// Get the filename where the error originated from
		const ReadOnlyString GetFilename() const;

		// Get the line which the error originated from
		const int GetLine() const { return _line; }

		// Where, on the line, the error originated from
		const int GetLineOffset() const { return _lineOffset; }

		// Where in the source code the error originated from
		const int GetOffset() const { return _offset; }

		char const* what() const final { return _error.c_str(); }

	protected:
		// Set the error message
		void SetErrorf(const char* fmt, ...);

		// Set the error message
		void SetError(const String& error);

	private:
		const SourceCode* const _sourceCode;
		String _error;
		const ParseErrorType _type;
		const int _line;
		const int _lineOffset;
		const int _offset;
	};

	// Error raised if we expected an identity
	class ParseErrorExpectedIdentity : public ParseError
	{
	public:
		ParseErrorExpectedIdentity(const ParserState* state);
	};

	// Error raised if we expected constant
	class ParseErrorExpectedConstant : public ParseError
	{
	public:
		ParseErrorExpectedConstant(const ParserState* state);
	};

	// Error raised if we've reached functionality that's not implemented yet
	class ParseErrorNotImplemented : public ParseError
	{
	public:
		ParseErrorNotImplemented(const ParserState* state);
	};

	// Error raised if we've reached functionality that's not implemented yet
	class ParseErrorSyntaxError : public ParseError
	{
	public:
		ParseErrorSyntaxError(const ParserState* state, const char* prefix);
	};
}
