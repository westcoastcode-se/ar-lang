#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncBody;
	class SyntaxTreeNodeFuncArg;

	// A function definition node
	class SyntaxTreeNodeFuncDef : public ISyntaxTreeNodeFuncDef
	{
	public:
		SyntaxTreeNodeFuncDef(SourceCodeView sourceCode, ReadOnlyString name);

		~SyntaxTreeNodeFuncDef() final;

		// ISyntaxTreeNodeFuncDef
		const ID& GetID() const final { return _id; }
		ReadOnlyString GetName() const final { return _name; }
		ISyntaxTree* GetSyntaxTree() const final { return _parent->GetSyntaxTree(); }
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyArray<ISyntaxTreeNodeFuncArg*> GetArguments() const final { return _arguments; }
		ISyntaxTreeNodeType* GetReturnType() const final { return _returnType; }
		bool IsVoidReturn() const final { return _returnType == nullptr; }
		ISyntaxTreeNodeFuncBody* GetBody() const final;
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker) final;
		ISyntaxTreeNodePackage* GetPackage() final;

	public:
		// add the supplied node
		void AddNode(ISyntaxTreeNode* node);

		// Set the body for this function
		void SetBody(SyntaxTreeNodeFuncBody* body);

		// Add an argument
		void AddArgument(ISyntaxTreeNodeFuncArg* arg);

		// Add a return statement
		void SetReturnType(ISyntaxTreeNodeType* returnType);

		// Get the symbol
		Builder::Function* GetSymbol() { return _symbol; }

		// Parse source code into a function definition node. Will throw ParseError if parsing of the 
		// source code failed
		static SyntaxTreeNodeFuncDef* Parse(ParserState* state);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		ReadOnlyString _name;

		Vector<ISyntaxTreeNodeFuncArg*> _arguments;
		ISyntaxTreeNodeType* _returnType;
		SyntaxTreeNodeFuncBody* _body;

		Builder::Function* _symbol;
	};
}