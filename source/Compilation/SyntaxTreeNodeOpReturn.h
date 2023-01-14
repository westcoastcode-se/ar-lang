#pragma once

#include "SyntaxTreeNode.h"
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
		const ID& GetID() const final { return _id; }
		virtual void ToString(StringStream& s, int indent) const override;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		virtual ISyntaxTreeNode* GetParent() const override { return _parent; }
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		virtual ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const override { return _children; }
		const SourceCodeView* GetSourceCode() const final {
			return &_sourceCode;
		}

	public:
		// Add a new node to be returned
		void AddNode(ISyntaxTreeNode* node);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		ISyntaxTreeNodeFuncDef* _function;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
	};
}
