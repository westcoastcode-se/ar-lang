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


		const Vector<ISyntaxTreeNodePackage*>& GetPackages() const final { return _children; }
		void ToString(StringStream& s) const final;
		void Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const final;
		void Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		ISyntaxTreeNodePackage* GetRootNode() final;

		// Add the supplied package
		void AddPackage(SyntaxTreeNodePackage* package);

	private:
		Vector<ISyntaxTreeNodePackage*> _children;
	};

	// The compiler used
	class Compiler
	{
	public:
		Compiler();

		~Compiler();

		// Add a source code block to the compiler. Returns true if the source code
		// could be parsed into a valid syntax tree block
		bool AddSourceCode(SourceCode* sourceCode);

		// Get the syntax tree
		SyntaxTree* GetSyntaxTree() const { return _syntaxTree; }

	private:
		// Start parsing tokens and convert them into syntax tree nodes
		bool ParseTokens(SourceCode* sourceCode, Token* t);

	private:
		Vector<SourceCode*> _sourceCodes;
		SyntaxTree* _syntaxTree;
	};
}
