#pragma once

#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	// Unary operator node
	class SyntaxTreeNodeOpUnaryop : public ISyntaxTreeNodeOpUnaryop
	{
	public:
		SyntaxTreeNodeOpUnaryop(SourceCodeView sourceCode,
			ISyntaxTreeNodeFuncDef* function,
			ISyntaxTreeNode* right, Op op)
			: _parent(nullptr), _children{ {right} }, _sourceCode(sourceCode), _op(op), _function(function) {}

		~SyntaxTreeNodeOpUnaryop() final;

		// Stringify the operator
		static const ReadOnlyString ToString(const Op op) {
			ENUM_STRING_START(op);
			ENUM_STRING(Plus);
			ENUM_STRING(Minus);
			ENUM_STRING(BitNot);
			ENUM_STRING_END();
		}

		// Inherited via ISyntaxTreeNodeOpUnaryop
		ISyntaxTreeNode* GetRight() const final { return *_children.begin(); }
		Op GetOperator() const final { return _op; }
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		const List<ISyntaxTreeNode*>& GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		void ToString(StringStream& s, int indent) const final;
		virtual ISyntaxTree* GetSyntaxTree() const override;
		virtual ISyntaxTreeNode* GetRootNode() override;
		virtual void SetParent(ISyntaxTreeNode* parent) override;
		virtual bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const override;
		virtual bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override;
		virtual bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override;
		virtual ISyntaxTreeNodeFuncDef* GetFunction() const final { return _function; }
		virtual ISyntaxTreeNodePackage* GetPackage() const final;

	private:
		ISyntaxTreeNode* _parent;
		List<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		Op _op;
		ISyntaxTreeNodeFuncDef* _function;

	};
}
