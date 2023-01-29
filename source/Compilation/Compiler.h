#pragma once

#include "../Common.h"
#include "SourceCode.h"
#include "Lexer.h"
#include "SyntaxTree.h"
#include "SyntaxTreeNodeImport.h"
#include "Functions/SyntaxTreeNodeFuncDef.h"
#include "Functions/SyntaxTreeNodeFuncDefBody.h"
#include "SyntaxTreeNodeType.h"
#include "SyntaxTreeNodePackage.h"

namespace WestCoastCode::Compilation
{
	// The compiler used
	class Compiler
	{
	public:
		Compiler();

		~Compiler();

		// Add a source code block to the compiler. Returns what the syntax tree looks like
		// after the source code is added to it
		SyntaxTree* AddSourceCode(SourceCode* sourceCode);

		// Get the syntax tree
		const SyntaxTree* GetSyntaxTree() const { return &_syntaxTree; }

		/// @brief Compile the added source codes and return byte code
		/// 
		/// Please note that this moves the ownership of the bytecode the the one calling this method.
		/// 
		/// You can also specify which optimization level you wish to perform on the compiled source code. Please
		/// note that the higher level the longer time it will take to compile it
		/// 
		/// @throws WestCoastCode::Compilation::CompileError
		Byte* Compile(int optimizationLevel = 0);

		// Find the primitive with the supplied name
		SyntaxTreeNodePrimitive* FindPrimitive(ReadOnlyString name);

	private:
		// Start parsing tokens and convert them into syntax tree nodes
		void ParseTokens(SourceCode* sourceCode, Token* t);

		// Perform optimization
		void Optimize(int level);

	private:
		Vector<SourceCode*> _sourceCodes;
		SyntaxTree _syntaxTree;
	};
}
