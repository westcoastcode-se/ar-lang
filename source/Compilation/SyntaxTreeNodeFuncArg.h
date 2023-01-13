#pragma once

#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncDef;

	// An argument node for a function
	class SyntaxTreeNodeFuncArg : public ISyntaxTreeNodeFuncArg
	{
	public:
		SyntaxTreeNodeFuncArg(SourceCodeView sourceCode, ReadOnlyString name);

		~SyntaxTreeNodeFuncArg();

		// Inherited via ISyntaxTreeNodeFuncArg
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		VisitResult Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, VisitFlags flags) final {
			return VisitResult::Continue;
		}
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, QuerySearchFlags flags) final {
			return false;
		}
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyString GetName() const final { return _name; }
		ISyntaxTreeNodeTypeRef* GetVariableType() const final { return _variableType; }
	
	public:
		// Set the variable type
		void SetVariableType(ISyntaxTreeNodeTypeRef* type);

		// Add a child node
		void AddNode(ISyntaxTreeNode* node);

		// Parse a new function definition argument
		static SyntaxTreeNodeFuncArg* Parse(ParserState* state);

	private:
		ISyntaxTreeNode* _parent;
		ReadOnlyString _name;
		SourceCodeView _sourceCode;
		Vector<ISyntaxTreeNode*> _children;
		ISyntaxTreeNodeTypeRef* _variableType;

	};
}
