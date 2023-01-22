#include "Lexer.h"
#include "../Utils.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

namespace
{
	// Is the supplied character a valid "first" keyword character
	inline bool IsPotentiallyKeyword(Char c)
	{
		switch (c) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
			return true;
		default:
			return false;
		}
	}

	inline bool IsDigit(Char c)
	{
		switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
		default:
			return false;
		}
	}

	inline bool IsHex(Char c)
	{
		switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			return true;
		default:
			return false;
		}
	}

	inline bool IsChar(Char c)
	{
		return IsPotentiallyKeyword(c) || IsDigit(c);
	}


	// Test if rune is a whitespace. Newlines are not considered white-space in this function
	inline bool IsWhitespace(Char c)
	{
		switch (c) {
		case ' ':
		case '\t':
		case '\r':
			return true;
		default:
			return false;
		}
	}

	inline bool IsNewline(Char c)
	{
		return c == '\n';
	}
}

int Token::GetLineOffset() const
{
	return (int)(_pos - _lineOffset - GetString().length());
}

int Token::GetOffset() const
{
	return (int)(_pos - _lexer->GetFirst() - GetString().length());
}

TokenType Token::Next()
{
	Char c = *_pos;
	if (c == 0) {
		Eof();
	}
	else {
		while (IsWhitespace(c)) {
			if (IsNewline(c)) {
				_line++;
				_lineOffset = _pos;
			}
			c = *++_pos;
		}
		Next0();
	}
	return _type;
}

bool Token::IsKeyword(TokenType type)
{
	return (int)type > (int)TokenType::Keyword_Start &&
		(int)type < (int)TokenType::Keyword_End;
}

bool Token::IsValue(TokenType type)
{
	return (int)type > (int)TokenType::Value_Start &&
		(int)type < (int)TokenType::Value_End;
}

IB Token::ToBool() const
{
	return Strcmp(_stringStart, (int)(_stringEnd - _stringStart), "true", 4);
}

I64 Token::ToI64() const
{
	return Strtoi64(_stringStart, (int)(_stringEnd - _stringStart));
}

U64 Token::ToU64() const
{
	return Strtou64(_stringStart, (int)(_stringEnd - _stringStart));
}

F32 Token::ToF32() const
{
	char temp[64];
	temp[sprintf(temp, "%.*s", (int)(_stringEnd - _stringStart), _stringStart)] = 0;

	const F32 v = strtof(temp, NULL);
	if (v < (FLT_MAX - FLT_EPSILON) && v >(FLT_MIN + FLT_EPSILON))
		return v;
	else
		return 0.0f;
}

F64 Token::ToF64() const
{
	char temp[64];
	temp[sprintf(temp, "%.*s", (int)(_stringEnd - _stringStart), _stringStart)] = 0;

	const F64 v = strtod(temp, NULL);
	if (v < (DBL_MAX - DBL_EPSILON) && v >(DBL_MIN + DBL_EPSILON))
		return v;
	else
		return 0.0;
}

void Token::MoveTo(Token* rhs)
{
	_pos = rhs->_pos;
	_type = rhs->_type;
	_modifier = rhs->_modifier;
	_stringStart = rhs->_stringStart;
	_stringEnd = rhs->_stringEnd;
	_line = rhs->_line;
	_lineOffset = rhs->_lineOffset;
}

void Token::Next0()
{
	Char c = *_pos;

	_modifier = TokenModifier::None;

	// Is this a potential keyword or identity?
	if (IsPotentiallyKeyword(c)) {
		NextKeyword();
		return;
	}

	// Is this a potential number?
	if (IsDigit(c)) {
		NextNumber();
		return;
	}

	switch (c)
	{
	case '\n':
		Atom(TokenType::Newline);
		_line++;
		_lineOffset = _pos;
		break;
	case '+':
		NextPlusOrIncrement();
		break;
	case '-':
		NextMinusOrDecrement();
		break;
	case '*':
		Atom(TokenType::OpMult);
		break;
	case '/':
		Atom(TokenType::OpDiv);
		break;
	case '(':
		Atom(TokenType::ParantLeft);
		break;
	case ')':
		Atom(TokenType::ParantRight);
		break;
	case '[':
		Atom(TokenType::SquareLeft);
		break;
	case ']':
		Atom(TokenType::SquareRight);
		break;
	case '{':
		Atom(TokenType::BracketLeft);
		break;
	case '}':
		Atom(TokenType::BracketRight);
		break;
	case '.':
		Atom(TokenType::Dot);
		break;
	case ',':
		Atom(TokenType::Comma);
		break;
	case '=':
		NextAssignOrEquals();
		break;
	case '<':
		NextLessThen();
		break;
	case '>':
		NextGreaterThen();
		break;
	case '~':
		Atom(TokenType::BitNot);
		break;
	case ':':
		NextKeySeparatorDeclAndAssign();
		break;
	case 0:
		Eof();
		break;
	default:
		Unknown();
		break;
	}
}

void Token::NextKeyword()
{
	// Remember the first character and step to next char
	const Char* start = _pos;
	_pos++;

	// Ignore all characters
	while (IsChar(*_pos)) _pos++;

	_stringStart = start;
	_stringEnd = _pos;
	_type = FindKeywordType();
}

void Token::Atom(TokenType type)
{
	_type = type;
	_modifier = TokenModifier::None;
	_stringStart = _pos;
	_stringEnd = ++_pos;
}

void Token::Unknown()
{
	_type = TokenType::Unknown;
	_modifier = TokenModifier::None;
	_stringStart = _pos;
	_stringEnd = ++_pos;
}

void Token::Eof()
{
	_stringStart = _stringEnd = _lexer->GetLast();
	_type = TokenType::Eof;
}

void Token::NextNumber()
{
	// Remember the first character and step to next char
	const Char* start = _pos;
	_pos++;

	// Ignore all digits
	while (IsDigit(*_pos)) _pos++;

	// Time to figure out if this is an integer, decimal or a hex value
	_type = TokenType::Int;

	// Is this is a decimal?
	if (*_pos == '.') {
		_type = TokenType::Decimal;
		_pos++;

		// Ignore all digits
		while (IsDigit(*_pos)) _pos++;

		// This might be a value with format -3.402823466e+38f
		if (*_pos == 'e') {
			char peek = *(_pos + 1);
			if (peek == '+' || peek == '-') {
				_pos += 2;

				// Ignore all numbers
				while (IsDigit(*_pos)) _pos++;
			}
		}

		if (*_pos == 'f') {
			_modifier = TokenModifier::HintFloat;
			_pos++;
		}

		_stringStart = start;
		_stringEnd = _pos;
		return;
	}
	
	// Hex-decimal value (must start with a 0)
	if (*start == '0' && * _pos == 'x') {
		_type = TokenType::Hex;
		_pos++;

		// Ignore all numbers
		while (IsHex(*_pos)) _pos++;
	}

	// Hint that this value is an unsigned constant
	if (*_pos == 'u') {
		_modifier = TokenModifier::HintUnsigned;
		_pos++;
	}

	_stringStart = start;
	_stringEnd = _pos;
}

TokenType Token::FindKeywordType() const
{
	const auto str = GetString();
	const auto len = str.length();
	if (len == 2) {
		static const String IF("if");
		if (str.compare(IF) == 0) {
			return TokenType::If;
		}
	}
	else if (len == 3) {
		static const String VAR("var");
		static const String FOR("for");
		if (str.compare(VAR) == 0) {
			return TokenType::Var;
		}
		else if (str.compare(FOR) == 0) {
			return TokenType::For;
		}
	}
	else if (len == 4) {
		static const String FUNC("func");
		static const String TYPE("type");
		static const String ELSE("else");
		if (str.compare(FUNC) == 0) {
			return TokenType::Func;
		}
		else if (str.compare(TYPE) == 0) {
			return TokenType::Type;
		}
		else if (str.compare(ELSE) == 0) {
			return TokenType::Else;
		}
	}
	else if (len == 5) {
		static const String CONST("const");
		static const String CLASS("class");
		if (str.compare(CONST) == 0) {
			return TokenType::Const;
		}
		else if (str.compare(CLASS) == 0) {
			return TokenType::Class;
		}
	}
	else if (len == 6) {
		static const String IMPORT("import");
		static const String RETURN("return");
		static const String EXTERN("extern");
		if (str.compare(IMPORT) == 0) {
			return TokenType::Import;
		}
		else if (str.compare(RETURN) == 0) {
			return TokenType::Return;
		}
		else if (str.compare(EXTERN) == 0) {
			return TokenType::Extern;
		}
	}
	else if (len == 7) {
		static const String PACKAGE("package");
		if (str.compare(PACKAGE) == 0) {
			return TokenType::Package;
		}
	}
	else if (len == 9) {
		static const String INTERFACE("interface");
		if (str.compare(INTERFACE) == 0) {
			return TokenType::Interface;
		}
	}
	return TokenType::Identity;
}

void Token::NextPlusOrIncrement()
{
	const Char* peek = _pos + 1;
	if (*peek == '+') {
		_pos++;
		Atom(TokenType::OpIncrement);
		return;
	}

	Atom(TokenType::OpPlus);
}

void Token::NextMinusOrDecrement()
{
	const Char* peek = _pos + 1;
	if (*peek == '-') {
		_pos++;
		Atom(TokenType::OpDecrement);
		return;
	}

	Atom(TokenType::OpMinus);
}

void Token::NextAssignOrEquals()
{
	const Char* peek = _pos + 1;
	if (*peek == '=') {
		_pos++;
		Atom(TokenType::TestEquals);
		return;
	}

	Atom(TokenType::Assign);
}

void Token::NextKeySeparatorDeclAndAssign()
{
	const Char* peek = _pos + 1;
	if (*peek == '=') {
		_pos++;
		Atom(TokenType::DeclAndAssign);
		return;
	}

	Atom(TokenType::KeySeparator);
}

void Token::NextLessThen()
{
	const Char* peek = _pos + 1;
	if (*peek == '=') {
		_pos++;
		Atom(TokenType::TestLte);
		return;
	}

	Atom(TokenType::TestLt);
}

void Token::NextGreaterThen()
{
	const Char* peek = _pos + 1;
	if (*peek == '=') {
		_pos++;
		Atom(TokenType::TestGte);
		return;
	}

	Atom(TokenType::TestGt);
}
