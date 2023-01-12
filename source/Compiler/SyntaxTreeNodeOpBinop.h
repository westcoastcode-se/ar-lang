#pragma once

#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	// Binary operator node
	class SyntaxTreeNodeOpBinop : public ISyntaxTreeNodeOpBinop
	{
	public:
		SyntaxTreeNodeOpBinop(SourceCodeView sourceCode, ISyntaxTreeNodeFuncDef* function,
			ISyntaxTreeNode* left, ISyntaxTreeNode* right, Op op)
			: _parent(nullptr), _function(function), _children{ { left, right } }, _sourceCode(sourceCode), _op(op) {}

		~SyntaxTreeNodeOpBinop() final;

		// Stringify the operator
		static const ReadOnlyString ToString(const Op op) {
			ENUM_STRING_START(op);
			ENUM_STRING(Plus);
			ENUM_STRING(Minus);
			ENUM_STRING(Mult);
			ENUM_STRING(Div);
			ENUM_STRING(Equals);
			ENUM_STRING(NotEquals);
			ENUM_STRING(LessThen);
			ENUM_STRING(LessThenEquals);
			ENUM_STRING(GreaterThen);
			ENUM_STRING(GreaterThenEquals);
			ENUM_STRING(BitAnd);
			ENUM_STRING_END();
		}

		// ISyntaxTreeNode
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		const List<ISyntaxTreeNode*>& GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ISyntaxTreeNode* GetLeft() const final { return *_children.begin(); }
		ISyntaxTreeNode* GetRight() const final { return *(++_children.begin()); }
		Op GetOperator() const final { return _op; }
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const override;
		ISyntaxTreeNode* GetRootNode() override;
		void SetParent(ISyntaxTreeNode* parent) override;
		bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const override;
		bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override;
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) override; 
		ISyntaxTreeNodeFuncDef* GetFunction() const final { return _function; }
		ISyntaxTreeNodePackage* GetPackage() const final;

	private:
		ISyntaxTreeNode* _parent;
		ISyntaxTreeNodeFuncDef* _function;
		List<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
		Op _op;
	};
}