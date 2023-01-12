#pragma once

#include "../Common.h"

namespace WestCoastCode::Compilation
{
	class Lexer
	{
	public:
		Lexer(ReadOnlyString text)
			: _text(text) {}

		// Get the first character
		const Char* GetFirst() const {
			return _text.data();
		}

		// Get the last character
		const Char* GetLast() const {
			return _text.data() + _text.length();
		}

	private:
		ReadOnlyString _text;
	};

	// The type of token we found
	enum class TokenType : int
	{
		Unknown,

		Identity,

		Keyword_Start,

		If,
		Var,
		For,
		Func,
		Type,
		Else,
		Const,
		Class,
		Import,
		Return,
		Extern,
		Package,
		Interface,

		Keyword_End,

		Op_Start,

		OpPlus,
		OpIncrement,
		OpMinus,
		OpDecrement,
		OpMult,
		OpDiv,

		Op_End,

		Test_Start,
		
		TestLt,
		TestLte,
		TestGt,
		TestGte,
		TestEquals,
		TestNotEquals,

		Test_End,

		Bit_Start,
		
		BitNot,
		BitAnd,
		BitOr,
		BitXor,

		Bit_End,

		Int,
		Hex,
		Bool,
		Decimal,
		String,

		ParantLeft,
		ParantRight,
		BracketLeft,
		BracketRight,
		SquareLeft,
		SquareRight,
		Dot,
		Comma,

		// !
		Not,

		// =
		Assign,

		// :=
		DeclAndAssign,

		// Is represented as a simple ":"
		KeySeparator,

		Newline,

		Eof,
	};

	// Modifiers for the token. For example if a string is multiline
	enum class TokenModifier : int
	{
		None,
	};

	class Token
	{
	public:
		Token(const Lexer* lexer)
			: _lexer(lexer), _pos(lexer->GetFirst()), _type(TokenType::Unknown), _modifier(TokenModifier::None),
			_stringStart(_pos), _stringEnd(_pos),
			_line(0), _lineOffset(_pos) {}

		Token(const Token* t)
			: _lexer(t->_lexer), _pos(t->_pos), _type(t->_type), _modifier(t->_modifier), 
			_stringStart(t->_stringStart), _stringEnd(t->_stringEnd),
			_line(t->_line), _lineOffset(t->_lineOffset) {}

		// Check to see if the supplied token is of the type
		inline bool Is(TokenType type) const { return type == _type; }

		// Get the current token type
		inline TokenType GetType() const { return _type; }

		// Get which line we are parsing at the moment
		inline int GetLine() const { return _line; }

		// Get the offset, in bytes, on the line we are parsing at the moment
		int GetLineOffset() const;

		// Get the offset, in bytes, in the source code we are parsing at the moment
		int GetOffset() const;

		// Get the next token
		TokenType Next();

		// Get the token string
		ReadOnlyString GetString() const { return ReadOnlyString(_stringStart, _stringEnd); }

		// Is the current token a keyword?
		bool IsKeyword() const {
			return IsKeyword(_type);
		}

		// Is the supplied token a keyword
		static bool IsKeyword(TokenType type);

	private:
		// Get the next token
		void Next0();

		// Get the next keyword token
		void NextKeyword();

		// Get the next number token
		void NextNumber();

		// Current token is an atom token
		void Atom(TokenType type);

		// Current token is an atom token
		void Unknown();

		// Reached the end of the string
		void Eof();

		// Figure out the token type based on the keyword
		TokenType FindKeywordType() const;

		// Get the next plus token. It might also be an increment token
		void NextPlusOrIncrement();

		// Get the next minus token. It might also be an decrement token
		void NextMinusOrDecrement();

		// Get the next assign token
		void NextAssignOrEquals();

		// Get the next jey delimiter token
		void NextKeySeparatorDeclAndAssign();
		
		// Next less then token
		void NextLessThen();

		// Next greater then token
		void NextGreaterThen();

	private:
		const Lexer* const _lexer;
		const Char* _pos;
		TokenType _type;
		TokenModifier _modifier;
		
		const Char* _stringStart;
		const Char* _stringEnd;

		int _line;
		const Char* _lineOffset;
	};
}
