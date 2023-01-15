#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	// Binary operator node
	class SyntaxTreeNodeOpBinop : public ISyntaxTreeNodeOpBinop
	{
	public:
		SyntaxTreeNodeOpBinop(SourceCodeView sourceCode, ISyntaxTreeNodeFuncDef* function,
			ISyntaxTreeNodeOp* left, ISyntaxTreeNodeOp* right, Op op)
			: _parent(nullptr), _function(function), _children(left, right), _sourceCode(sourceCode), _op(op) {}

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
		const ID& GetID() const final { return _id; }
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ISyntaxTreeNodeOp* GetLeft() const final { return _children[0]; }
		ISyntaxTreeNodeOp* GetRight() const final { return _children[1]; }
		Op GetOperator() const final { return _op; }
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const override;
		ISyntaxTreeNode* GetRootNode() override;
		void SetParent(ISyntaxTreeNode* parent) override;
		ISyntaxTreeNodeFuncDef* GetFunction() final { return _function; }
		ISyntaxTreeNodePackage* GetPackage() final { return _function->GetPackage(); }
		ISyntaxTreeNodeType* GetStackType() final;
		void Compile(Builder::Linker* linker, Builder::Instructions& instructions) final;
		Vector<ISyntaxTreeNodeOp*> OptimizeOp(ISyntaxTreeNodeOptimizer* optimizer) final;

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		ISyntaxTreeNodeFuncDef* _function;
		Array<ISyntaxTreeNodeOp*, 2> _children;
		SourceCodeView _sourceCode;
		Op _op;
	};
}
