#pragma once

#include "SyntaxTree.h"
#include "SourceCodeParser.h"

namespace WestCoastCode::Compilation
{
	// Represents a return statement
	class SyntaxTreeNodeOpReturn : public ISyntaxTreeNodeOpReturn
	{
	public:
		SyntaxTreeNodeOpReturn(SourceCodeView sourceCode, ISyntaxTreeNodeFuncDef* function)
			: _parent(nullptr), _sourceCode(sourceCode), _function(function) {}

		~SyntaxTreeNodeOpReturn() final;

		// Inherited via ISyntaxTreeNodeOpReturn
		virtual void ToString(StringStream& s, int indent) const override;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		virtual ISyntaxTreeNode* GetParent() const override { return _parent; }
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		virtual ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const override { return _children; }
		VisitResult Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, VisitFlags flags) final {
			return VisitResult::Continue;
		}
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, QuerySearchFlags flags) final {
			return false;
		}
		virtual const SourceCodeView* GetSourceCode() const override;

	public:
		// Add a new node to be returned
		void AddNode(ISyntaxTreeNode* node);

	private:
		ISyntaxTreeNode* _parent;
		ISyntaxTreeNodeFuncDef* _function;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
	};
}
