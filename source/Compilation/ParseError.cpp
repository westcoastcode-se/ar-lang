#include "ParseError.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ParseError::ParseError(const SourceCode* sourceCode,
	const Token* token, ParseErrorType type)
	: CompilationError(), _sourceCode(sourceCode), _type(type), _line(token->GetLine()),
	_lineOffset(token->GetLineOffset()), _offset(token->GetOffset())
{
}

const ReadOnlyString ParseError::GetFilename() const
{
	return _sourceCode->GetFilename();
}

ParseErrorExpectedIdentity::ParseErrorExpectedIdentity(const ParserState* state)
	: ParseError(state->sourceCode, state->token, ParseErrorType::ExpectedIdentity)
{
	StringStream s;
	s << "expected identity but was '" << state->token->GetString() << "'";
	SetError(s.str());
}

ParseErrorExpectedConstant::ParseErrorExpectedConstant(const ParserState* state)
	: ParseError(state->sourceCode, state->token, ParseErrorType::ExpectedConstant)
{
	StringStream s;
	s << "expected constant but was '" << state->token->GetString() << "'";
	SetError(s.str());
}

ParseErrorNotImplemented::ParseErrorNotImplemented(const ParserState* state)
	: ParseError(state->sourceCode, state->token, ParseErrorType::NotImplemented)
{
	StringStream s;
	s << "not implemented '" << state->token->GetString() << "'";
	SetError(s.str());
}

ParseErrorSyntaxError::ParseErrorSyntaxError(const ParserState* state, const char* prefix)
	: ParseError(state->sourceCode, state->token, ParseErrorType::SyntaxError)
{
	StringStream s;
	s << prefix << " but was '" << state->token->GetString() << "'";
	SetError(s.str());
}
