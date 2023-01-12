#pragma once

#include "SourceCode.h"
#include "Lexer.h"

namespace WestCoastCode::Compilation
{
	class Compiler;
	class ISyntaxTreeNode;
	class SyntaxTreeNodePackage;
	class SyntaxTreeNodeFuncDef;
	class SyntaxTreeNodeClassDef;

	// State used when parsing the source code
	struct ParserState
	{
		// Compiler used when 
		Compiler* compiler;

		// The token
		Token* token;

		// The source code we are parsing
		SourceCode* sourceCode;

		// The closest parent node
		ISyntaxTreeNode* parentNode;

		// The closest package node
		SyntaxTreeNodePackage* package;

		// The closest function definition
		SyntaxTreeNodeFuncDef* function;

		// The closest class definition
		SyntaxTreeNodeClassDef* clazz;

		ParserState(Compiler* c, Token* t, SourceCode* sc)
			: compiler(c), token(t), sourceCode(sc), parentNode(nullptr), 
			package(nullptr), function(nullptr), clazz(nullptr) {}

		ParserState(ParserState* state, SyntaxTreeNodePackage* package);

		ParserState(ParserState* state, SyntaxTreeNodeFuncDef* function);

		ParserState(ParserState* state, SyntaxTreeNodeClassDef* clazz);
	};
}
