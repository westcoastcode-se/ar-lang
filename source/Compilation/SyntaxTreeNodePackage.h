#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	// A package node
	class SyntaxTreeNodePackage : public ISyntaxTreeNodePackage
	{
	public:
		SyntaxTreeNodePackage(SourceCodeView sourceCode, ReadOnlyString name)
			: _syntaxTree(nullptr), _parent(nullptr), _sourceCode(sourceCode), _name(name), _symbol(nullptr) {}

		~SyntaxTreeNodePackage() final;

		// Inherited via ISyntaxTreeNodePackage
		const ID& GetID() const final { return _id; }
		ReadOnlyString GetName() const final { return _name; }
		ISyntaxTree* GetSyntaxTree() const final { return _syntaxTree; }
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		VisitResult Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags) final;
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker) final;

	public:
		// set which syntax tree this node is part of
		void SetSyntaxTree(ISyntaxTree* syntaxTree) { _syntaxTree = syntaxTree; }
		
		// add the supplied node
		void AddNode(ISyntaxTreeNode* node);

		// Get the builder symbol for this package
		Builder::Package* GetSymbol() const { return _symbol; }

		// Parse source code into a package node. Will throw ParseError if parsing of the 
		// source code failed
		static SyntaxTreeNodePackage* Parse(const ParserState* state);

	private:
		const ID _id;
		ISyntaxTree* _syntaxTree;
		SyntaxTreeNodePackage* _parent;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		ReadOnlyString _name;

		Builder::Package* _symbol;
	};
}
