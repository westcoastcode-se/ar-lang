#include "ParseError.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ParseError::ParseError(const SourceCode* sourceCode,
	Token* token, ParseErrorType type)
	: exception(), _sourceCode(sourceCode), _type(type), _line(token->GetLine()),
	_lineOffset(token->GetLineOffset()), _offset(token->GetOffset())
{
}

const ReadOnlyString ParseError::GetFilename() const
{
	return _sourceCode->GetFilename();
}

void ParseError::SetErrorf(const char* fmt, ...)
{
	_error = fmt;
}

void ParseError::SetError(const String& error)
{
	_error = error;
}

ParseErrorExpectedIdentity::ParseErrorExpectedIdentity(ParserState* state)
	: ParseError(state->sourceCode, state->token, ParseErrorType::ExpectedIdentity)
{
	StringStream s;
	s << "expected identity but was '" << state->token->GetString() << "'";
	SetError(s.str());
}

ParseErrorNotImplemented::ParseErrorNotImplemented(ParserState* state)
	: ParseError(state->sourceCode, state->token, ParseErrorType::NotImplemented)
{
	StringStream s;
	s << "not implemented '" << state->token->GetString() << "'";
	SetError(s.str());
}

ParseErrorSyntaxError::ParseErrorSyntaxError(ParserState* state, const char* prefix)
	: ParseError(state->sourceCode, state->token, ParseErrorType::SyntaxError)
{
	StringStream s;
	s << prefix << " but was '" << state->token->GetString() << "'";
	SetError(s.str());
}
