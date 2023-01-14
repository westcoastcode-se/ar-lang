#pragma once

#include "SyntaxTreeNode.h"
#include "SourceCodeParser.h"

namespace WestCoastCode::Compilation
{
	// Represents a return statement
	class SyntaxTreeNodeScope : public ISyntaxTreeNodeScope
	{
	public:
		SyntaxTreeNodeScope(SourceCodeView sourceCode, ISyntaxTreeNodeFuncDef* function)
			: _parent(nullptr), _sourceCode(sourceCode), _function(function) {}

		~SyntaxTreeNodeScope() final;

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
		void Compile(Builder::Linker* linker, Builder::Instructions& instructions) final;
		ISyntaxTreeNodeFuncDef* GetFunction() final { return _function; }
		ISyntaxTreeNodePackage* GetPackage() final { return _function->GetPackage(); }

	public:
		// Add a new node to be returned
		void AddOp(ISyntaxTreeNodeOp* node);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		ISyntaxTreeNodeFuncDef* _function;
		Vector<ISyntaxTreeNodeOp*> _children;
		SourceCodeView _sourceCode;
	};
}
