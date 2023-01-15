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
		SyntaxTreeNodeRoot* GetRootNode() final;

		// Resolve references
		void ResolveReferences();

		// Compile the syntaxtree and link it together into bytecode
		void Compile(Builder::Linker* linker);
		
		// Optimize the syntax tree. Level 0 is only basic constant merge, while Level 3 is the highest optimization
		void Optimize(ISyntaxTreeNodeOptimizer* optimizer);

	private:
		SyntaxTreeNodeRoot* const _root;
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
		// this method.
		//
		// You can also specify which optimization level you wish to perform on the compiled source code. Please
		// note that the higher level the longer time it will take to compile it
		Byte* Compile(int optimizationLevel = 0);

		// Find the primitive with the supplied name
		ISyntaxTreeNodePrimitive* FindPrimitive(ReadOnlyString name);

	private:
		// Start parsing tokens and convert them into syntax tree nodes
		void ParseTokens(SourceCode* sourceCode, Token* t);

	private:
		Vector<SourceCode*> _sourceCodes;
		SyntaxTree* _syntaxTree;
	};
}
