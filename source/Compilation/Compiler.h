#pragma once

#include "../Common.h"
#include "SourceCode.h"
#include "Lexer.h"
#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	// Tree structure for where the source code is built into
	class SyntaxTree : public ISyntaxTree
	{
	public:
		SyntaxTree();

		~SyntaxTree() final;

		// Inherited via ISyntaxTree
		void ToString(StringStream& s) const final;
		void Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags) final;
		ISyntaxTreeNodePackage* GetRootNode() final;
		void ResolveReferences() final;
		void Compile(Builder::Linker* linker) final;

		// Add the supplied package
		void SetRootPackage(SyntaxTreeNodePackage* package);

	private:
		ISyntaxTreeNodePackage* _root;
	};

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
		SyntaxTree* GetSyntaxTree() const { return _syntaxTree; }

		// Compile the added source codes and return the byte code that the interpreter
		// can use. Please note that this moves the ownership of the bytecode the the one calling
		// this method
		Byte* Compile();

	private:
		// Start parsing tokens and convert them into syntax tree nodes
		void ParseTokens(SourceCode* sourceCode, Token* t);

	private:
		Vector<SourceCode*> _sourceCodes;
		SyntaxTree* _syntaxTree;
	};
}
