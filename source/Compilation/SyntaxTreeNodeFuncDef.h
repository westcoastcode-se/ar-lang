#pragma once

#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncBody;
	class SyntaxTreeNodeFuncArg;
	class SyntaxTreeNodeFuncRet;

	// A function definition node
	class SyntaxTreeNodeFuncDef : public ISyntaxTreeNodeFuncDef
	{
	public:
		SyntaxTreeNodeFuncDef(SourceCodeView sourceCode, ReadOnlyString name);

		~SyntaxTreeNodeFuncDef() final;

		// ISyntaxTreeNodeFuncDef
		ReadOnlyString GetName() const final { return _name; }
		ISyntaxTree* GetSyntaxTree() const final { return _parent->GetSyntaxTree(); }
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		VisitResult Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, VisitFlags flags) final {
			return VisitResult::Continue;
		}
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, QuerySearchFlags flags) final {
			return false;
		}
		ReadOnlyArray<ISyntaxTreeNodeFuncArg*> GetArguments() const final { return _arguments; }
		ReadOnlyArray<ISyntaxTreeNodeFuncRet*> GetReturns() const final { return _returns; }
		bool IsVoidReturn() const final { return _returns.IsEmpty(); }
		ISyntaxTreeNodeFuncBody* GetBody() const final;
		void ToString(StringStream& s, int indent) const final;

	public:
		// add the supplied node
		void AddNode(ISyntaxTreeNode* node);

		// Set the body for this function
		void SetBody(SyntaxTreeNodeFuncBody* body);

		// Add an argument
		void AddArgument(ISyntaxTreeNodeFuncArg* arg);

		// Add a return statement
		void AddReturn(ISyntaxTreeNodeFuncRet* ret);

		// Parse source code into a function definition node. Will throw ParseError if parsing of the 
		// source code failed
		static SyntaxTreeNodeFuncDef* Parse(ParserState* state);

	private:
		ISyntaxTreeNode* _parent;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		ReadOnlyString _name;

		Vector<ISyntaxTreeNodeFuncArg*> _arguments;
		Vector<ISyntaxTreeNodeFuncRet*> _returns;
		SyntaxTreeNodeFuncBody* _body;

	};
}
