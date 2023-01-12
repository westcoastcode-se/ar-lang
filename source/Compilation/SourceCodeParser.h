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

		ParserState(Compiler* c, Token* t, SourceCode* sc, SyntaxTreeNodePackage* root);

		ParserState(const ParserState* state, SyntaxTreeNodePackage* package);

		ParserState(const ParserState* state, SyntaxTreeNodeFuncDef* function);

		ParserState(const ParserState* state, SyntaxTreeNodeClassDef* clazz);
	};
}
