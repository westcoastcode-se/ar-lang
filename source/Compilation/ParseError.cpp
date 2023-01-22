#include "ParseError.h"
#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodeTypeDef.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

ParseError::ParseError(const SourceCode* sourceCode,
	const Token* token, ParseErrorType type)
	: CompilationError(), _sourceCode(sourceCode), _type(type), _line(token->GetLine()),
	_lineOffset(token->GetLineOffset()), _lineStart(token->GetLineStart()), _offset(token->GetOffset())
{
}

const ReadOnlyString ParseError::GetFilename() const
{
	return _sourceCode->GetFilename();
}

void ParseError::PrintToStderr() const
{
	auto source = GetSourceCode();
	fprintf(stderr, "Failed to compile %.*s:\n\n", 
		(I32)source->GetFilename().length(), source->GetFilename().data());

	auto text = source->GetText();
	const char* s = text.data();
	const char* end = s + text.length();
	int line = 0;
	for (; s != end; ++s) {
		fprintf(stderr, "%c", *s);
		if (*s == '\n') {
			if ((++line) == GetLine() + 1) {
				for (int i = 0; i < _lineOffset; ++i) {
					if (_lineStart[i] == '\t')
						fprintf(stderr, "\t");
					else
						fprintf(stderr, " ");
				}
				fprintf(stderr, "^ PE%05d: %s\n", (I32)GetType(), what());
			}
		}
	}
}

ParseErrorExpectedPackage::ParseErrorExpectedPackage(const ParserState* state)
	: ParseError(state->sourceCode, state->token, ParseErrorType::ExpectedPackage)
{
	StringStream s;
	s << "expected package but was '" << state->token->GetString() << "'";
	SetError(s.str());
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

ParseErrorIncompatibleTypes::ParseErrorIncompatibleTypes(const ParserState* state, 
	const SyntaxTreeNodeTypeDef* type1,
	const SyntaxTreeNodeTypeDef* type2)
	: ParseError(state->sourceCode, state->token, ParseErrorType::IncompatibleTypes)
{
	StringStream s;
	s << type1->GetName() << " is not compatible with " << type2->GetName();
	SetError(s.str());
}
