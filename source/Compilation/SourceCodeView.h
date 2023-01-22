#pragma once

#include "SourceCode.h"
#include "Lexer.h"

namespace WestCoastCode::Compilation
{

	// A view inside the supplied source code
	class SourceCodeView
	{
	public:
		SourceCodeView()
			: _sourceCode(nullptr), _line(0), _lineOffset(0), _lineStart(nullptr),
			_offset(-1) {}

		SourceCodeView(const SourceCode* sourceCode, const Token* t)
			: _sourceCode(sourceCode), _line(t->GetLine()), _lineOffset(t->GetLineOffset()),
			_lineStart(t->GetLineStart()), _offset(t->GetOffset()) {}

		// The source code
		const SourceCode* GetSourceCode() const {
			return _sourceCode;
		}

		// Get the line 
		int GetLine() const { return _line; }

		// Get the line 
		int GetLineOffset() const { return _lineOffset; }

		const Char* GetLineStart() const { return _lineStart; }

		// Get the line 
		int GetOffset() const { return _offset; }

	private:
		const SourceCode* _sourceCode;
		int _line;
		int _lineOffset;
		const Char* _lineStart;
		int _offset;
	};
}
